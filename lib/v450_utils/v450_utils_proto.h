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
// Date: Sun Aug 08 11:38:20 2021

#ifndef _LIBV450_PROTO_H_
#define _LIBV450_PROTO_H_

#ifndef _V450_SDEF_H_
#include "v450_regs_sdef.h"
#endif

extern int v450_config_rtd_type(V450_REGS *, char, int);
extern int v450_read_rtd_type(V450_REGS *, char, int *);
extern int v450_read_rtd_temp(V450_REGS *, char, char, float *);
extern int v450_read_rtd_ohms(V450_REGS *, char, float *);
extern int v450_write_fake_rtd_temp(V450_REGS *, char, char, float);
extern int v450_read_reg(V450_REGS *, unsigned short, unsigned short *);
extern int v450_read_interlock_reg(V450_REGS *, unsigned short, unsigned int *);
extern int v450_config_analog_chnl(V450_REGS *, unsigned int, unsigned int, unsigned int, unsigned int);
extern int v450_config_tc_chnl(V450_REGS *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
extern int v450_read_loop_resistance(V450_REGS *, int, float *);
extern int v450_read_analog_chnl(V450_REGS *, int, float *, int *);
extern int v450_read_tc_chnl(V450_REGS *, int, char, float *);
extern int v450_execute_macro(V450_REGS *, unsigned short, unsigned short, unsigned short *);
extern int v450_chnl_to_rn_code(V450_REGS *, int, unsigned short *);
extern int v450_rn_code_to_range(int, float *, float *);

extern unsigned short v450_read_vximfr(V450_REGS *);
extern unsigned short v450_read_vxitype(V450_REGS *);
extern unsigned short v450_read_serial(V450_REGS *);
extern unsigned short v450_read_romid(V450_REGS *);
extern unsigned short v450_read_romrev(V450_REGS *);
extern unsigned short v450_read_dfilt(V450_REGS *); // digital filter active flag
extern unsigned short v450_read_cflags(V450_REGS *);
extern unsigned short v450_read_rflags(V450_REGS *);
// extern unsigned short v450_read_afilt(V450_REGS *);
extern unsigned short v450_read_relays(V450_REGS *);
extern unsigned short v450_write_relays(V450_REGS *, unsigned short);
extern unsigned short v450_read_uled(V450_REGS *);
extern unsigned short v450_write_uled(V450_REGS *, unsigned short);
extern unsigned short v450_read_mode(V450_REGS *);
extern unsigned short v450_write_mode(V450_REGS *, unsigned short);
extern unsigned short v450_read_calid(V450_REGS *);
extern unsigned short v450_read_biss(V450_REGS *);
extern unsigned short v450_read_ycal(V450_REGS *);
extern unsigned short v450_read_dcal(V450_REGS *);
extern unsigned short v450_read_bistdac(V450_REGS *);
extern unsigned short v450_write_bistdac(V450_REGS *, unsigned short);
extern unsigned short v450_read_bistblk(V450_REGS *, int, unsigned short *); // second arg points to a user array of 6 unsigned short
extern unsigned short v450_read_biststage(V450_REGS *);
extern unsigned short v450_read_bistnumerrors(V450_REGS *);

extern char *v450_get_cal_date(V450_REGS *);
#endif
