// -*- c++ -*-

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
// Date: Sun Aug 08 10:40:35 2021

#ifndef _PARSE_OPTIONS_PROTO_H_
#define _PARSE_OPTIONS_PROTO_H_

#ifndef _OPTION_TABLE_SDEF_H_
#include "option_table_sdef.h"
#endif

extern int parse_options(int, char *[], OPTION_TABLE *, unsigned long *);

extern int print_help_text(OPTION_TABLE *, char *, unsigned long *);
extern int set_mask(OPTION_TABLE *, char *, unsigned long *);
extern int set_chgflg(OPTION_TABLE *, char *, unsigned long *);

extern int load_enum(OPTION_TABLE *, char *, unsigned long *);
extern int load_constant(OPTION_TABLE *, char *, unsigned long *);
extern int load_string(OPTION_TABLE *, char *, unsigned long *);
extern int noop(OPTION_TABLE *, char *, unsigned long *);
extern int load_crate_num(OPTION_TABLE *, char *, unsigned long *);
extern int load_crate_board_num(OPTION_TABLE *, char *, unsigned long *);
extern int match_dev_name(char *, char *);

#endif
