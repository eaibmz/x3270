/*
 * Copyright (c) 1993-2009, 2013-2017 Paul Mattes.
 * Copyright (c) 1990, Jeff Sparkes.
 * Copyright (c) 1989, Georgia Tech Research Corporation (GTRC), Atlanta,
 *  GA 30332.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the names of Paul Mattes, Jeff Sparkes, GTRC nor their
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY PAUL MATTES, JEFF SPARKES AND GTRC "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PAUL MATTES, JEFF SPARKES OR
 * GTRC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *	b3270.c
 *		A GUI back-end for a 3270 Terminal Emulator
 *		Main proceudre.
 */

#include "globals.h"
#if !defined(_WIN32) /*[*/
# include <sys/wait.h>
# include <signal.h>
#endif /*]*/
#include <errno.h>
#include "appres.h"
#include "3270ds.h"
#include "resources.h"

#include "actions.h"
#include "bind-opt.h"
#include "bscreen.h"
#include "charset.h"
#include "ctlr.h"
#include "ctlrc.h"
#include "unicodec.h"
#include "ft.h"
#include "glue.h"
#include "ui_stream.h"
#include "host.h"
#include "httpd-core.h"
#include "httpd-nodes.h"
#include "httpd-io.h"
#include "idle.h"
#include "kybd.h"
#include "lazya.h"
#include "nvt.h"
#include "nvt_gui.h"
#include "opts.h"
#include "popups.h"
#include "print_screen.h"
#include "product.h"
#include "screen.h"
#include "selectc.h"
#include "sio.h"
#include "status.h"
#include "task.h"
#include "telnet.h"
#include "toggles.h"
#include "trace.h"
#include "utils.h"
#include "varbuf.h"
#include "xio.h"

#if defined(_WIN32) /*[*/
# include "w3misc.h"
# include "windirs.h"
# include "winvers.h"
#endif /*]*/

#define STATS_POLL	(2 * 1000)

#if defined(_WIN32) /*[*/
char *instdir = NULL;
char *mydesktop = NULL;
char *mydocs3270 = NULL;
char *commondocs3270 = NULL;
unsigned windirs_flags;
#endif /*]*/

static int brcvd = 0;
static int rrcvd = 0;
static int bsent = 0;
static int rsent = 0;
static ioid_t stats_ioid = NULL_IOID;

static int our_major, our_minor, our_iteration;

static void b3270_toggle(toggle_index_t ix, enum toggle_type tt);
static toggle_register_t toggles[] = {
    { MONOCASE,		b3270_toggle,	TOGGLE_NEED_INIT },
    { ALT_CURSOR,	b3270_toggle,	TOGGLE_NEED_INIT },
    { CURSOR_BLINK,	b3270_toggle,	TOGGLE_NEED_INIT },
    { TRACING,		b3270_toggle,	TOGGLE_NEED_INIT },
    { VISIBLE_CONTROL,	b3270_toggle,	TOGGLE_NEED_INIT },
    { CROSSHAIR,	b3270_toggle,	TOGGLE_NEED_INIT },
    { OVERLAY_PASTE,	b3270_toggle,	TOGGLE_NEED_INIT }
};
static const char *cstate_name[] = {
    "not-connected",
    "ssl-password-pending",
    "resolving",
    "pending",
    "negotiating",
    "connected-initial",
    "connected-nvt",
    "connected-nvt-charmode",
    "connected-3270",
    "connected-unbound",
    "connected-e-nvt",
    "connected-sscp",
    "connected-tn3270e"
};

static void check_min_version(const char *min_version);
static void b3270_register(void);

void
usage(const char *msg)
{
    if (msg != NULL) {
	fprintf(stderr, "%s\n", msg);
    }
    fprintf(stderr, "Usage: %s [options] [profile-file.b3270]\n", programname);
    fprintf(stderr, "Options:\n");
    cmdline_help(false);
    exit(1);
}

static void
dump_stats()
{
    ui_vleaf("stats",
	    "bytes-received", lazyaf("%d", brcvd),
	    "records-received", lazyaf("%d", rrcvd),
	    "bytes-sent", lazyaf("%d", bsent),
	    "records-sent", lazyaf("%d", rsent),
	    NULL);
}

static void
stats_poll(ioid_t id _is_unused)
{
    if (brcvd != ns_brcvd ||
	rrcvd != ns_rrcvd ||
	bsent != ns_bsent ||
	rsent != ns_rsent) {
	brcvd = ns_brcvd;
	rrcvd = ns_rrcvd;
	bsent = ns_bsent;
	rsent = ns_rsent;
	dump_stats();
    }
    stats_ioid = AddTimeOut(STATS_POLL, stats_poll);
}

/**
 * Respond to a change in the connection, 3270 mode, or line mode.
 */
static void
b3270_connect(bool ignored)
{       
    static enum cstate old_cstate = (int)NOT_CONNECTED;

    if (cstate == old_cstate) {
	return;
    }

    /* If just disconnected, dump final stats. */
    if (cstate == NOT_CONNECTED && stats_ioid != NULL_IOID) {
	RemoveTimeOut(stats_ioid);
	stats_ioid = NULL_IOID;
	if (brcvd != ns_brcvd ||
	    rrcvd != ns_rrcvd ||
	    bsent != ns_bsent ||
	    rsent != ns_rsent) {
	    brcvd = ns_brcvd;
	    rrcvd = ns_rrcvd;
	    bsent = ns_bsent;
	    rsent = ns_rsent;
	    dump_stats();
	}
    }

    /* Tell the GUI about the new state. */
    if (cstate == NOT_CONNECTED) {
	ui_vleaf("connection",
		"state", cstate_name[(int)cstate],
		NULL);
    } else {
	ui_vleaf("connection",
		"state", cstate_name[(int)cstate],
		"host", current_host,
		NULL);

	/* Clear the screen. */
	if (old_cstate == NOT_CONNECTED) {
	    ctlr_erase(true);
	}
    }

    /* If just connected, dump initial stats. */
    if (cstate != NOT_CONNECTED && stats_ioid == NULL_IOID) {
	brcvd = 0;
	rrcvd = 0;
	bsent = 0;
	rsent = 0;
	dump_stats();
	stats_ioid = AddTimeOut(STATS_POLL, stats_poll);
    }

    old_cstate = cstate;
}

static void
b3270_secure(bool ignored)
{
    static bool is_secure = false;

    if (net_secure_connection() == is_secure) {
	return;
    }
    is_secure = net_secure_connection();

     ui_vleaf("ssl",
	     "secure", net_secure_connection()? "true": "false",
	     "verified",
		 net_secure_connection()? (net_secure_unverified()? "false": "true"): NULL,
	     "session", net_session_info(),
	     "host-cert", net_server_cert_info(),
	     NULL);
}

/* Translate supported SSL options to a list of names. */
static char *sio_options(void)
{
    unsigned options = sio_options_supported();
    unsigned opt;
    varbuf_t v;
    char *sep = "";

    vb_init(&v);
    while (SSL_ALL_OPTS & opt) {
	if (options & opt) {
	    const char *opt_name = sio_option_name(opt);

	    if (opt_name != NULL) {
		vb_appendf(&v, "%s%s", sep, opt_name);
		sep = " ";
	    }
	}
	opt <<= 1;
    }

    return lazya(vb_consume(&v));
}

int
main(int argc, char *argv[])
{
    const char *cl_hostname = NULL;

    if (sizeof(cstate_name)/sizeof(cstate_name[0]) != NUM_CSTATE) {
	Error("b3270 cstate_name has the wrong number of elements");
    }

#if defined(_WIN32) /*[*/
    (void) get_version_info();
    if (!get_dirs("wc3270", &instdir, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, &windirs_flags)) {
	exit(1);
    }
    if (sockstart() < 0) {
	exit(1);
    }
#endif /*]*/

    /*
     * Call the module registration functions, to build up the tables of
     * actions, options and callbacks.
     */
    charset_register();
    ctlr_register();
    ft_register();
    host_register();
    idle_register();
    kybd_register();
    macros_register();
    nvt_register();
    print_screen_register();
    b3270_register();
    toggles_register();
    trace_register();
    xio_register();
    sio_register();

    argc = parse_command_line(argc, (const char **)argv, &cl_hostname);
    if (cl_hostname != NULL) {
	usage("Unrecognized option(s)");
    }

    check_min_version(appres.min_version);

    ui_io_init();
    ui_vleaf("hello",
	    "version", lazyaf("%d.%d.%d", our_major, our_minor, our_iteration),
	    "build", build,
	    "copyright",
lazyaf("\
Copyright © 1993-%s, Paul Mattes.\n\
Copyright © 1990, Jeff Sparkes.\n\
Copyright © 1989, Georgia Tech Research Corporation (GTRC), Atlanta, GA\n\
 30332.\n\
All rights reserved.\n\
\n\
Redistribution and use in source and binary forms, with or without\n\
modification, are permitted provided that the following conditions are met:\n\
    * Redistributions of source code must retain the above copyright\n\
      notice, this list of conditions and the following disclaimer.\n\
    * Redistributions in binary form must reproduce the above copyright\n\
      notice, this list of conditions and the following disclaimer in the\n\
      documentation and/or other materials provided with the distribution.\n\
    * Neither the names of Paul Mattes, Jeff Sparkes, GTRC nor the names of\n\
      their contributors may be used to endorse or promote products derived\n\
      from this software without specific prior written permission.\n\
\n\
THIS SOFTWARE IS PROVIDED BY PAUL MATTES, JEFF SPARKES AND GTRC \"AS IS\" AND\n\
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n\
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE\n\
ARE DISCLAIMED. IN NO EVENT SHALL PAUL MATTES, JEFF SPARKES OR GTRC BE\n\
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR\n\
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF\n\
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n\
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n\
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)\n\
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n\
POSSIBILITY OF SUCH DAMAGE.", cyear),
	    NULL);

    if (charset_init(appres.charset) != CS_OKAY) {
	xs_warning("Cannot find charset \"%s\"", appres.charset);
	(void) charset_init(NULL);
    }
    model_init();
    status_reset();

    /*
     * Slam ROWS and COLS to the max right now. The ctlr code goes to a lot of
     * trouble to make these defROWS and defCOLS, probably so a host that does
     * starts out with a blind Write without an Erase will get a Model 2, but
     * I will let someone complain about that if it comes up in practice.
     *
     * b3270_connect() does an implied EraseWriteAlternate when a host
     * connects, so that would need to change, too.
     */
    ROWS = altROWS;
    COLS = altCOLS;

    screen_init();
    ctlr_init(-1);
    ctlr_reinit(-1);
    ui_vleaf("model",
	    "name", model_name,
	    NULL);
    idle_init();
    if (appres.httpd_port) {
	struct sockaddr *sa;
	socklen_t sa_len;

	if (!parse_bind_opt(appres.httpd_port, &sa, &sa_len)) {
	    xs_warning("Invalid -httpd port \"%s\"", appres.httpd_port);
	} else {
	    httpd_objects_init();
	    hio_init(sa, sa_len);
	}
    }
    ft_init();
    hostfile_init();

#if !defined(_WIN32) /*[*/
    /* Make sure we don't fall over any SIGPIPEs. */
    (void) signal(SIGPIPE, SIG_IGN);
#endif /*]*/

    /* Handle initial toggle settings. */
    initialize_toggles();

    /* Send SSL set-up */
    ui_vleaf("ssl-hello",
	    "supported", sio_supported()? "true": "false",
	    "provider", sio_provider(),
	    "options", sio_options(),
	    NULL);

    ui_vleaf("ready", NULL);

    /* Prepare to run a peer script. */
    peer_script_init();

    /* Process events forever. */
    while (1) {
	(void) process_events(true);
	screen_disp(false);
    }
}

/*
 * Model action:
 * Model(["327x-n"[,<rows>x<cols>a]])
 */
static bool
Model_action(ia_t ia, unsigned argc, const char **argv)
{
    char *color;
    char *digit;
    unsigned ovr = 0, ovc = 0;
    int model_number;

    action_debug("Model", ia, argc, argv);
    if (check_argc("Model", argc, 0, 2) < 0) {
	return false;
    }

    /* With no arguments, outputs the current model. */
    if (argc == 0) {
	char *model = lazyaf("327%c-%d",
		appres.m3279? '9': '8',
		model_num);

	if (ov_rows || ov_cols) {
	    model = lazyaf("%s,%dx%d", model, ov_rows, ov_cols);
	}
	action_output("%s", model);
	return true;
    }

    if (PCONNECTED) {
	popup_an_error("Model: Cannot change model while connected");
	return false;
    }

    /*
     * One argument changes the model number and clears oversize.
     * Two changes both.
     */
    if (strlen(argv[0]) != 6 ||
	strncmp(argv[0], "327", 3) ||
	(color = strchr("89", argv[0][3])) == NULL ||
	argv[0][4] != '-' ||
	(digit = strchr("2345", argv[0][5])) == NULL) {

	popup_an_error("Model: First parameter must be 327[89]-[2345]");
	return false;
    }
    if (argc > 1) {
	char x, junk;
	if (sscanf(argv[1], "%u%c%u%c", &ovr, &x, &ovc, &junk) != 3
		|| x != 'x') {
	    popup_an_error("Mode: Second parameter must be <rows>x<cols>");
	    return false;
	}
    }

    /* Change the screen size and emulation mode. */
    model_number = *digit - '0';
    set_rows_cols(model_number, ovc, ovr);
    ROWS = maxROWS;
    COLS = maxCOLS;
    ctlr_reinit(MODEL_CHANGE);
    appres.m3279 = *color == '9';

    /* Reset the screen state. */
    screen_init();
    ctlr_erase(true);

    return model_num == model_number
	&& ov_rows == (int)ovr
	&& ov_cols == (int)ovc;
}

/*
 * Trace action:
 *  Trace
 *  Trace On
 *  Trace On,file
 *  Trace Off
 */
static bool
Trace_action(ia_t ia, unsigned argc, const char **argv)
{
    action_debug("Trace", ia, argc, argv);
    if (check_argc("Trace", argc, 0, 2) < 0) {
	return false;
    }

    if (argc == 0) {
	if (toggled(TRACING) && tracefile_name != NULL) {
	    action_output("On,%s", tracefile_name);
	} else {
	    action_output("Off");
	}
	return true;
    }

    if (!strcasecmp(argv[0], "Off")) {

	/* Turn tracing off. */
	if (argc > 1) {
	    popup_an_error("Trace: Too many arguments for 'Off'");
	    return false;
	}
	if (toggled(TRACING)) {
	    do_toggle(TRACING);
	    action_output("Off,%s", tracefile_name);
	}
	return true;
    }

    if (strcasecmp(argv[0], "On")) {
	popup_an_error("Trace: Parameter must be On or Off");
	return false;
    }

    /* Turn tracing on. */
    if (argc > 1) {
	if (toggled(TRACING)) {
	    popup_an_error("Trace: cannot specify filename when tracing "
		    "is already on");
	    return false;
	}
	trace_set_trace_file(argv[1]);
    }
    if (!toggled(TRACING)) {
	do_toggle(TRACING);
	action_output("On,%s", tracefile_name);
    }

    return true;
}

/*
 * ClearRegion action:
 *  ClearRegion row column rows columns
 * Row and column are 1-origin.
 * Used by the UI Cut action.
 */
static bool
ClearRegion_action(ia_t ia, unsigned argc, const char **argv)
{
    int row, column, rows, columns;
    int r, c;
    int baddr;
    int ba2;

    action_debug("ClearRegion", ia, argc, argv);
    if (check_argc("ClearRegion", argc, 4, 4) < 0) {
	return false;
    }

    row = atoi(argv[0]);
    column = atoi(argv[1]);
    rows = atoi(argv[2]);
    columns = atoi(argv[3]);

    if (row <= 0 || row > ROWS || column <= 0 || column > COLS) {
	popup_an_error("ClearRegion: invalid coordinates");
    }

    if (rows < 0 || columns < 0 ||
	    row - 1 + rows > ROWS || column - 1 + columns > COLS) {
	popup_an_error("ClearRegion: invalid size");
    }

    if (rows == 0 || columns == 0) {
	return true;
    }

    baddr = ROWCOL_TO_BA(row - 1, column - 1);
    for (r = row - 1; r < row - 1 + rows; r++) {
	for (c = column - 1; c < column - 1 + columns; c++) {
	    baddr = ROWCOL_TO_BA(r, c);
	    if (ea_buf[baddr].fa ||
		    FA_IS_PROTECTED(get_field_attribute(baddr)) ||
		    ea_buf[baddr].cc == EBC_so ||
		    ea_buf[baddr].cc == EBC_si) {
		continue;
	    }
	    switch (ctlr_dbcs_state(baddr)) {
		case DBCS_NONE:
		case DBCS_SB:
		    ctlr_add(baddr, EBC_space, ea_buf[baddr].cs);
		    break;
		case DBCS_LEFT:
		    ctlr_add(baddr, EBC_space, ea_buf[baddr].cs);
		    ba2 = baddr;
		    INC_BA(ba2);
		    ctlr_add(ba2, EBC_space, ea_buf[baddr].cs);
		    break;
		case DBCS_RIGHT:
		    ba2 = baddr;
		    DEC_BA(ba2);
		    ctlr_add(ba2, EBC_space, ea_buf[baddr].cs);
		    ctlr_add(baddr, EBC_space, ea_buf[baddr].cs);
		    break;
		default:
		    break;
	    }
	    mdt_set(baddr);
	}
    }

    return true;
}

/**
 * xterm text escape
 *
 * @param[in] opcode    Operation to perform
 * @param[in] text      Associated text
 */
void
xterm_text_gui(int code, const char *text)
{
    if (code == 0 || code == 1) {
	ui_vleaf("icon-name",
		"text", text,
		NULL);
    }
    if (code == 0 || code == 2) {
	ui_vleaf("window-title",
		"text", text,
		NULL);
    }

    if (code == 50) {
	ui_vleaf("font",
		"text", text,
		NULL);
    }
}

/**
 * Set product-specific appres defaults.
 */
void
product_set_appres_defaults(void)
{
    /*
     * Set defaults like s3270 -- operator error locks the keyboard and
     * no unlock delay.
     *
     * TODO: I need a way to change these from the UI.
     */
    appres.oerr_lock = true;
    appres.unlock_delay = false;
}

/**
 * Parse a version number.
 * Version numbers are of the form: <major>.<minor>text<iteration>, such as
 *  3.4ga10 (3, 4, 10)
 *  3.5apha3 (3, 5, 3)
 * The version can be under-specified, e.g.:
 *  3.4 (3, 4, 0)
 *  3 (3, 0, 0)
 * Numbers are limited to 0..999.
 * @param[in] text		String to decode.
 * @param[out] major		Major number.
 * @param[out] minor		Minor number.
 * @param[out] iteration	Iteration.
 *
 * @return true if parse successful.
 */
#define MAX_VERSION 999
static bool
parse_version(const char *text, int *major, int *minor, int *iteration)
{
    const char *t = text;
    unsigned long n;
    char *ptr;

    *major = 0;
    *minor = 0;
    *iteration = 0;

    /* Parse the major number. */
    n = strtoul(t, &ptr, 10);
    if (ptr == t || (*ptr != '.' && *ptr != '\0') || n > MAX_VERSION) {
	return false;
    }
    *major = (int)n;

    if (*ptr == '\0') {
	/* Just a major number. */
	return true;
    }

    /* Parse the minor number. */
    t = ptr + 1;
    n = strtoul(t, &ptr, 10);
    if (ptr == text || n > MAX_VERSION) {
	return false;
    }
    *minor = (int)n;

    if (*ptr == '\0') {
	/* Just a major and minor number. */
	return true;
    }

    /* Parse the iteration. */
    t = ptr;
    while (!isdigit((unsigned char)*t) && *t != '\0')
    {
	t++;
    }
    if (*t == '\0') {
	return false;
    }

    n = strtoul(t, &ptr, 10);
    if (ptr == t || *ptr != '\0' || n > MAX_VERSION) {
	return false;
    }
    *iteration = (int)n;

    return true;
}

/**
 * Check the requested version against the actual version.
 * @param[in] min_version	Desired minimum version
 */
static void
check_min_version(const char *min_version)
{
    int min_major, min_minor, min_iteration;

    /* Parse our version. */
    if (!parse_version(build_rpq_version, &our_major, &our_minor,
		&our_iteration)) {
	fprintf(stderr, "Internal error: Can't parse version: %s\n",
		build_rpq_version);
	exit(1);
    }
    if (min_version == NULL) {
	return;
    }

    /* Parse the desired version. */
    if (!parse_version(min_version, &min_major, &min_minor, &min_iteration)) {
	fprintf(stderr, "Invalid %s: %s\n", ResMinVersion, min_version);
	exit(1);
    }

    /* Compare. */
    if (our_major < min_major ||
	our_minor < min_minor ||
	our_iteration < min_iteration)
    {
	fprintf(stderr, "Version %s < requested %s, aborting\n",
		build_rpq_version, min_version);
	exit(1);
    }
}

/**
 * Handle a toggle change.
 * @param[in] ix	Toggle index
 * @param[in] tt	Toggle type
 */
static void
b3270_toggle(toggle_index_t ix, enum toggle_type tt)
{
    int i;

    for (i = 0; toggle_names[i].name; i++) {
	if (toggle_names[i].index == ix) {
	    break;
	}
    }
    if (!toggle_names[i].name) {
	return;
    }

    ui_vleaf("toggle",
	    "name", toggle_names[i].name,
	    "value", toggled(ix)? "true": "false",
	    "file", (ix == TRACING && toggled(ix) && tracefile_name != NULL)?
		tracefile_name: NULL,
	    NULL);
}

/**
 * Main module registration.
 */
static void
b3270_register(void)
{
    static action_table_t actions[] = {
	{ "Model",	Model_action,	0 },
	{ "Trace",	Trace_action,	0 },
	{ "ClearRegion",ClearRegion_action,0 }
    };
    static opt_t b3270_opts[] = {
	{ OptScripted, OPT_NOP,     false, ResScripted,  NULL,
	    NULL, "Turn on scripting" },
	{ OptUtf8,     OPT_BOOLEAN, true,  ResUtf8,      aoffset(utf8),
	    NULL, "Force local codeset to be UTF-8" },
	{ OptMinVersion,OPT_STRING, false, ResMinVersion,aoffset(min_version),
	    "<version>", "Fail unless at this version or greater" }
    };
    static res_t b3270_resources[] = {
	{ ResIdleCommand,aoffset(idle_command),     XRM_STRING },
	{ ResIdleCommandEnabled,aoffset(idle_command_enabled),XRM_BOOLEAN },
	{ ResIdleTimeout,aoffset(idle_timeout),     XRM_STRING }
    };
    static xres_t b3270_xresources[] = {
	{ ResPrintTextScreensPerPage,	V_FLAT },
#if defined(_WIN32) /*[*/
	{ ResPrinterCodepage,		V_FLAT },
	{ ResPrinterName, 		V_FLAT },
	{ ResPrintTextFont, 		V_FLAT },
	{ ResPrintTextHorizontalMargin,	V_FLAT },
	{ ResPrintTextOrientation,	V_FLAT },
	{ ResPrintTextSize, 		V_FLAT },
	{ ResPrintTextVerticalMargin,	V_FLAT },
#else /*][*/
	{ ResPrintTextCommand,		V_FLAT },
#endif /*]*/
    };

    /* Register the toggles. */
    register_toggles(toggles, array_count(toggles));

    /* Register for state changes. */
    register_schange(ST_CONNECT, b3270_connect);
    register_schange(ST_HALF_CONNECT, b3270_connect);
    register_schange(ST_3270_MODE, b3270_connect);
    register_schange(ST_LINE_MODE, b3270_connect);
    register_schange(ST_SECURE, b3270_secure);
    register_schange(ST_CHARSET, b3270_new_charset);

    /* Register our actions. */
    register_actions(actions, array_count(actions));

    /* Register our options. */
    register_opts(b3270_opts, array_count(b3270_opts));

    /* Register our resources. */
    register_resources(b3270_resources, array_count(b3270_resources));
    register_xresources(b3270_xresources, array_count(b3270_xresources));
}
