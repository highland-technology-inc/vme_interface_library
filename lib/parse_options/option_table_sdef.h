// -*- c++ -*-		comments in c++ mode for emacs

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Author: Dean W. Anneser
// Company: RTLinux Solutions LLC for Highland Technology, Inc.
// Date: Sun Aug 08 10:40:09 2021

#ifndef _OPTION_TABLE_SDEF_H_
#define _OPTION_TABLE_SDEF_H_

#define MAX_ARG_STR		32
#define MAX_ARG_MATCH_STR	32

#define	T_NULL		0x00000000
#define T_CHAR		0x00000001
#define T_BYTE		T_CHAR
#define T_SHORT		0x00000002
#define T_INT		0x00000004
#define T_LONG		0x00000008
#define T_FLOAT		0x00000010
#define T_DOUBLE	0x00000020
#define T_UCHAR		0x00000100
#define T_UBYTE		T_UCHAR
#define T_USHORT	0x00000200
#define T_UINT		0x00000400
#define T_ULONG		0x00000800

#define T_STRING	0x00001000

#define T_ALL_MASK	(T_CHAR | T_SHORT | T_INT | T_LONG | T_FLOAT | T_DOUBLE | T_UCHAR | T_USHORT | T_UINT | T_ULONG)

typedef struct {
  char opt_str[MAX_ARG_STR];
  char arg_str[MAX_ARG_MATCH_STR];
  int arg_value;
  void *data;
  unsigned int type_mask;
  int (*routine)();
  unsigned long chgflg_mask;
} OPTION_TABLE;

#endif
