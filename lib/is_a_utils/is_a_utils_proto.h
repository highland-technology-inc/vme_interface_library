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
// Date: Sat Aug 07 17:21:20 2021

#ifndef _IS_A_UTILS_PROTO_H_
#define _IS_A_UTILS_PROTO_H_

extern int is_a_format_spec(char *);
extern int isodigit(char);
extern int is_a_decimal_int(char *);
extern int is_a_decimal_uint(char *);
extern int is_a_octal_int(char *);
extern int is_a_octal_uint(char *);
extern int is_a_hex_int(char *);
extern int is_a_hex_uint(char *);
extern int is_a_float(char *);
extern int is_a_double(char *);
extern int is_a_decimal_kilo_uint(char *);
extern int is_a_decimal_mega_uint(char *);

#endif
