/*
 * Modifications Copyright 1993, 1994, 1995, 1999, 2000, 2001, 2002, 2003,
 *   2005, 2006, 2008 by Paul Mattes.
 * Original X11 Port Copyright 1990 by Jeff Sparkes.
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for any purpose and without fee is hereby granted,
 *  provided that the above copyright notice appear in all copies and that
 *  both that copyright notice and this permission notice appear in
 *  supporting documentation.
 *
 * Copyright 1989 by Georgia Tech Research Corporation, Atlanta, GA 30332.
 *  All Rights Reserved.  GTRC hereby grants public use of this software.
 *  Derivative works based on this software must incorporate this copyright
 *  notice.
 *
 * x3270, c3270, s3270, tcl3270 and pr3287 are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * file LICENSE for more details.
 */

/*
 *	tables.c
 *		Translation tables between the three character sets:
 *			EBCDIC
 *			ASCII (ISO Latin-1)
 *			Character Generator ("3270" font)
 */

#include "globals.h"
#include "tablesc.h"

const unsigned short asc2cg0[256] = {
/*00*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*08*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*10*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*18*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*20*/	0x10, 0x19, 0x13, 0x2c, 0x1a, 0x2e, 0x30, 0x12,
/*28*/	0x0d, 0x0c, 0xbf, 0x35, 0x33, 0x31, 0x32, 0x14,
/*30*/	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
/*38*/	0x28, 0x29, 0x34, 0xbe, 0x09, 0x11, 0x08, 0x18,
/*40*/	0x2d, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
/*48*/	0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae,
/*50*/	0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
/*58*/	0xb7, 0xb8, 0xb9, 0x0a, 0x15, 0x0b, 0x3a, 0x2f,
/*60*/	0x3d, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86,
/*68*/	0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e,
/*70*/	0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
/*78*/	0x97, 0x98, 0x99, 0x0f, 0x16, 0x0e, 0x3b, 0x00,
/*80*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*88*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*90*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*98*/	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*a0*/	0x01, 0x6e, 0x1b, 0x1c, 0x1f, 0x1d, 0x17, 0x2b,
/*a8*/	0x3c, 0xd0, 0x6a, 0x6c, 0x36, 0x07, 0xd1, 0x37,
/*b0*/	0x38, 0xd6, 0x68, 0x69, 0x3e, 0x54, 0x1e, 0x39,
/*b8*/	0x3f, 0x67, 0x6b, 0x6d, 0x4b, 0x4c, 0x4d, 0x6f,
/*c0*/	0x60, 0x7a, 0x75, 0x65, 0x70, 0xbc, 0xba, 0xbd,
/*c8*/	0x61, 0x7b, 0x76, 0x71, 0x62, 0x7c, 0x77, 0x72,
/*d0*/	0xd7, 0x7f, 0x63, 0x7d, 0x78, 0x66, 0x73, 0x5b,
/*d8*/	0xbb, 0x64, 0x7e, 0x79, 0x74, 0x48, 0xd9, 0x2a,
/*e0*/	0x40, 0x5a, 0x55, 0x45, 0x50, 0x9c, 0x9a, 0x4f,
/*e8*/	0x41, 0x4a, 0x56, 0x51, 0x42, 0x5c, 0x57, 0x52,
/*f0*/	0xf7, 0x5f, 0x43, 0x5d, 0x58, 0x46, 0x53, 0x9d,
/*f8*/	0x9b, 0x44, 0x5e, 0x59, 0x4e, 0x49, 0xf9, 0x47
};
const unsigned short ebc2cg0[256] = {
/*00*/	0x00, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
/*08*/	0xdf, 0xdf, 0xdf, 0xdf, 0x02, 0x03, 0x00, 0x00,
/*10*/	0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0x04, 0xdf, 0xdf,
/*18*/	0xdf, 0x05, 0xdf, 0xdf, 0x9f, 0xdf, 0x9e, 0xdf,
/*20*/	0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
/*28*/	0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
/*30*/	0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
/*38*/	0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xdf,
/*40*/	0x10, 0x01, 0x55, 0x50, 0x40, 0x5a, 0x45, 0x9c,
/*48*/	0x4f, 0x5f, 0x1b, 0x32, 0x09, 0x0d, 0x35, 0x16,
/*50*/	0x30, 0x4a, 0x56, 0x51, 0x41, 0x5c, 0x57, 0x52,
/*58*/	0x42, 0x2a, 0x19, 0x1a, 0xbf, 0x0c, 0xbe, 0x36,
/*60*/	0x31, 0x14, 0x75, 0x70, 0x60, 0x7a, 0x65, 0xbc,
/*68*/	0xbd, 0x7f, 0x17, 0x33, 0x2e, 0x2f, 0x08, 0x18,
/*70*/	0x9b, 0x7b, 0x76, 0x71, 0x61, 0x7c, 0x77, 0x72,
/*78*/	0x62, 0x3d, 0x34, 0x2c, 0x2d, 0x12, 0x11, 0x13,
/*80*/	0xbb, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86,
/*88*/	0x87, 0x88, 0x6c, 0x6d, 0xf7, 0x49, 0xf9, 0xd6,
/*90*/	0x38, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
/*98*/	0x90, 0x91, 0x6a, 0x6b, 0x9a, 0x3f, 0xba, 0x1f,
/*a0*/	0x54, 0x3b, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
/*a8*/	0x98, 0x99, 0x6e, 0x6f, 0xd7, 0x48, 0xd9, 0xd1,
/*b0*/	0x3a, 0x1c, 0x1d, 0x39, 0xd0, 0x2b, 0x1e, 0x4b,
/*b8*/	0x4c, 0x4d, 0x0a, 0x0b, 0x37, 0x3c, 0x3e, 0x5b,
/*c0*/	0x0f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
/*c8*/	0xa7, 0xa8, 0x07, 0x58, 0x53, 0x43, 0x5d, 0x46,
/*d0*/	0x0e, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
/*d8*/	0xb0, 0xb1, 0x67, 0x59, 0x4e, 0x44, 0x5e, 0x47,
/*e0*/	0x15, 0x9d, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
/*e8*/	0xb8, 0xb9, 0x68, 0x78, 0x73, 0x63, 0x7d, 0x66,
/*f0*/	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
/*f8*/	0x28, 0x29, 0x69, 0x79, 0x74, 0x64, 0x7e, 0x06 };
const unsigned short ebc2asc0[256] = {
/*00*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*08*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*10*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*18*/	0x20, 0x20, 0x20, 0x20, 0x2a, 0x20, 0x3b, 0x20,
/*20*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*28*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*30*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*38*/	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*40*/	0x20, 0x20, 0xe2, 0xe4, 0xe0, 0xe1, 0xe3, 0xe5,
/*48*/	0xe7, 0xf1, 0xa2, 0x2e, 0x3c, 0x28, 0x2b, 0x7c,
/*50*/	0x26, 0xe9, 0xea, 0xeb, 0xe8, 0xed, 0xee, 0xef,
/*58*/	0xec, 0xdf, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0xac,
/*60*/	0x2d, 0x2f, 0xc2, 0xc4, 0xc0, 0xc1, 0xc3, 0xc5,
/*68*/	0xc7, 0xd1, 0xa6, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
/*70*/	0xf8, 0xc9, 0xca, 0xcb, 0xc8, 0xcd, 0xce, 0xcf,
/*78*/	0xcc, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,
/*80*/	0xd8, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
/*88*/	0x68, 0x69, 0xab, 0xbb, 0xf0, 0xfd, 0xfe, 0xb1,
/*90*/	0xb0, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
/*98*/	0x71, 0x72, 0xaa, 0xba, 0xe6, 0xb8, 0xc6, 0xa4,
/*a0*/	0xb5, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
/*a8*/	0x79, 0x7a, 0xa1, 0xbf, 0xd0, 0xdd, 0xde, 0xae,
/*b0*/	0x5e, 0xa3, 0xa5, 0xb7, 0xa9, 0xa7, 0xb6, 0xbc,
/*b8*/	0xbd, 0xbe, 0x5b, 0x5d, 0xaf, 0xa8, 0xb4, 0xd7,
/*c0*/	0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
/*c8*/	0x48, 0x49, 0xad, 0xf4, 0xf6, 0xf2, 0xf3, 0xf5,
/*d0*/	0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
/*d8*/	0x51, 0x52, 0xb9, 0xfb, 0xfc, 0xf9, 0xfa, 0xff,
/*e0*/	0x5c, 0xf7, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
/*e8*/	0x59, 0x5a, 0xb2, 0xd4, 0xd6, 0xd2, 0xd3, 0xd5,
/*f0*/	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
/*f8*/	0x38, 0x39, 0xb3, 0xdb, 0xdc, 0xd9, 0xda, 0x20 };
const unsigned short asc2ebc0[256] = {
/*00*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*08*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*10*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*18*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*20*/  0x40, 0x5a, 0x7f, 0x7b, 0x5b, 0x6c, 0x50, 0x7d,
/*28*/  0x4d, 0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61,
/*30*/  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
/*38*/  0xf8, 0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f,
/*40*/  0x7c, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
/*48*/  0xc8, 0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6,
/*50*/  0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
/*58*/  0xe7, 0xe8, 0xe9, 0xba, 0xe0, 0xbb, 0xb0, 0x6d,
/*60*/  0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
/*68*/  0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
/*70*/  0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
/*78*/  0xa7, 0xa8, 0xa9, 0xc0, 0x4f, 0xd0, 0xa1, 0x00,
/*80*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*88*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*90*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*98*/  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*a0*/  0x41, 0xaa, 0x4a, 0xb1, 0x9f, 0xb2, 0x6a, 0xb5,
/*a8*/  0xbd, 0xb4, 0x9a, 0x8a, 0x5f, 0xca, 0xaf, 0xbc,
/*b0*/  0x90, 0x8f, 0xea, 0xfa, 0xbe, 0xa0, 0xb6, 0xb3,
/*b8*/  0x9d, 0xda, 0x9b, 0x8b, 0xb7, 0xb8, 0xb9, 0xab,
/*c0*/  0x64, 0x65, 0x62, 0x66, 0x63, 0x67, 0x9e, 0x68,
/*c8*/  0x74, 0x71, 0x72, 0x73, 0x78, 0x75, 0x76, 0x77,
/*d0*/  0xac, 0x69, 0xed, 0xee, 0xeb, 0xef, 0xec, 0xbf,
/*d8*/  0x80, 0xfd, 0xfe, 0xfb, 0xfc, 0xad, 0xae, 0x59,
/*e0*/  0x44, 0x45, 0x42, 0x46, 0x43, 0x47, 0x9c, 0x48,
/*e8*/  0x54, 0x51, 0x52, 0x53, 0x58, 0x55, 0x56, 0x57,
/*f0*/  0x8c, 0x49, 0xcd, 0xce, 0xcb, 0xcf, 0xcc, 0xe1,
/*f8*/  0x70, 0xdd, 0xde, 0xdb, 0xdc, 0x8d, 0x8e, 0xdf };
