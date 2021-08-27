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
// Date: Sun Aug 08 11:46:11 2021

#ifndef _LIBV470_PROTO_H_
#define _LIBV470_PROTO_H_

#ifndef _V470_SDEF_H_
#include "v470_regs_sdef.h"
#endif

extern int v470_config_rtd_type(V470_REGS *, char, int);
extern int v470_read_rtd_type(V470_REGS *, char, int *);
extern int v470_read_rtd_temp(V470_REGS *, char, char, float *);
extern int v470_read_rtd_ohms(V470_REGS *, char, float *);
extern int v470_write_fake_rtd_temp(V470_REGS *, char, char, float);
extern int v470_read_reg(V470_REGS *, unsigned short, unsigned short *);
extern int v470_read_interlock_reg(V470_REGS *, unsigned short, unsigned int *);
extern int v470_config_analog_chnl(V470_REGS *, unsigned int, unsigned int);
extern int v470_config_tc_chnl(V470_REGS *, unsigned int, unsigned int, unsigned int);
extern int v470_write_analog_chnl(V470_REGS *, int, float);
extern int v470_write_tc_chnl(V470_REGS *, int, char, float);

extern int v470_execute_macro(V470_REGS *, unsigned short, unsigned short *, unsigned short *);
extern int v470_chnl_to_rn_code(V470_REGS *, int, unsigned short *);
extern int v470_rn_code_to_range(int, float *, float *);

extern unsigned short v470_read_vximfr(V470_REGS *);
extern unsigned short v470_read_vxitype(V470_REGS *);
extern unsigned short v470_read_serial(V470_REGS *);
extern unsigned short v470_read_romid(V470_REGS *);
extern unsigned short v470_read_romrev(V470_REGS *);
extern unsigned short v470_read_cflags(V470_REGS *);
extern unsigned short v470_read_rflags(V470_REGS *);
extern unsigned short v470_read_relays(V470_REGS *);
extern unsigned short v470_write_relays(V470_REGS *, unsigned short);
extern unsigned short v470_read_uled(V470_REGS *);
extern unsigned short v470_write_uled(V470_REGS *, unsigned short);
extern unsigned short v470_read_mode(V470_REGS *);
extern unsigned short v470_write_mode(V470_REGS *, unsigned short);
extern unsigned short v470_read_calid(V470_REGS *);
extern unsigned short v470_read_biss(V470_REGS *);
extern unsigned short v470_read_ycal(V470_REGS *);
extern unsigned short v470_read_dcal(V470_REGS *);
extern unsigned short v470_read_bistblk(V470_REGS *, int, unsigned short *); // second arg points to a user array of 6 unsigned short

extern char *v470_get_cal_date(V470_REGS *);
#endif
