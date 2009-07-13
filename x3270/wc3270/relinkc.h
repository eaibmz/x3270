/*
 * Copyright (c) 2006-2009, Paul Mattes.
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
 *     * Neither the name of Paul Mattes nor his contributors may be used
 *       to endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY PAUL MATTES "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PAUL MATTES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *	relinkc.h
 *		A Windows console-based 3270 Terminal Emulator
 *		Utility functions to read a session file and create a
 *		compatible shortcut.
 */

#define STR_SIZE	256

#define WIZARD_VER	2

typedef struct {
    	/* Fields for wc3270 3.3.9 (Wizard version 1) */
	char session[STR_SIZE];		/* session name */
	char host[STR_SIZE];		/* host name */
	int  port;			/* TCP port */
	char luname[STR_SIZE];		/* LU name */
	int  ssl;			/* SSL tunnel flag */
	char proxy_type[STR_SIZE];	/* proxy type */
	char proxy_host[STR_SIZE];	/*  proxy host */
	char proxy_port[STR_SIZE];	/*  proxy port */
	int  model;			/* model number */
	char charset[STR_SIZE];		/* character set name */
	int  is_dbcs;
	int  wpr3287;			/* wpr3287 flag */
	char printerlu[STR_SIZE];	/*  printer LU */
	char printer[STR_SIZE];		/*  Windows printer name */
	char printercp[STR_SIZE];	/*  wpr3287 code page */
	char keymaps[STR_SIZE];		/* keymap names */

	/* Field added for wc3270 3.3.10 (Wizard version 2) */
	int  embed_keymaps;		/* embed keymaps in file */
} session_t;

typedef struct {
	char *name;
	char *hostcp;
	int is_dbcs;
	wchar_t *codepage;
} charsets_t;
extern charsets_t charsets[];
extern size_t num_charsets;

extern int read_session(FILE *f, session_t *s);
extern HRESULT create_shortcut(session_t *session, char *exepath,
	char *linkpath, char *args, char *workingdir);
