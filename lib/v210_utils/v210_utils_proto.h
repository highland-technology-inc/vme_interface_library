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
// Date: Fri Mar 04 15:39:57 2022

#ifndef _LIBV210_PROTO_H_
#define _LIBV210_PROTO_H_

#ifndef _V210_SDEF_H_
#include "v210_regs_sdef.h"
#endif

#include <stdint.h>

extern unsigned short v210_read_bdid(V210_REGS *);
extern unsigned short v210_read_csr(V210_REGS *);
extern unsigned short v210_write_csr(V210_REGS *, unsigned short);
extern unsigned short v210_read_vximfr(V210_REGS *);
extern unsigned short v210_read_vxitype(V210_REGS *);
extern char v210_read_fpgarev(V210_REGS *);
extern void v210_turn_off_error_led(V210_REGS *);
extern void v210_turn_on_error_led(V210_REGS *);
extern void v210_connect_relays(V210_REGS *);
extern void v210_disconnect_relays(V210_REGS *);
extern int v210_set_bit_in_mask(V210_REGS *, int);
extern int v210_clr_bit_in_mask(V210_REGS *, int);
extern int v210_read_bit_in_mask(V210_REGS *, int);
extern void v210_write_relay_regs(V210_REGS *);
extern void v210_read_relay_regs(V210_REGS *, int);
extern void v210_clr_all_relays(V210_REGS *);
extern void v210_set_relays_from_mask64(V210_REGS *, uint64_t);
extern void v210_read_relays_into_mask64(V210_REGS *, uint64_t *, int);
#endif
