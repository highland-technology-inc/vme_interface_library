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
// Date: Thu Aug 19 16:07:54 2021

#ifndef _LIBV545_PROTO_H_
#define _LIBV545_PROTO_H_

#ifndef _V545_SDEF_H_
#include "v545_regs_sdef.h"
#endif

// read/write registers
extern unsigned short v545_read_vximfr(V545_REGS *);
extern unsigned short v545_read_vxitype(V545_REGS *);
extern unsigned short v545_read_serial(V545_REGS *);
extern unsigned short v545_read_romid(V545_REGS *);
extern unsigned short v545_read_romrev(V545_REGS *);
extern unsigned short v545_read_dash(V545_REGS *);
extern unsigned short v545_read_state(V545_REGS *);
extern unsigned short v545_read_uled(V545_REGS *);
extern unsigned short v545_write_uled(V545_REGS *, unsigned short);
extern unsigned short v545_read_biss(V545_REGS *);
extern unsigned short v545_read_calid(V545_REGS *);
extern unsigned short v545_read_ycal(V545_REGS *);
extern unsigned short v545_read_dcal(V545_REGS *);
extern unsigned short v545_read_swin(V545_REGS *);
extern unsigned short v545_write_swout(V545_REGS *, unsigned short);
extern int v545_read_reg(V545_REGS *, unsigned short, unsigned short *);
// extern int v545_read_interlock_reg(V545_REGS *, unsigned short, unsigned int *);
extern char *v545_get_cal_date(V545_REGS *);
extern int v545_read_dds_block(V545_REGS *, int, unsigned short *);
extern int v545_read_ccb_block(V545_REGS *, int, unsigned short *);
extern char *v545_read_xfmr_type(V545_REGS *, int);

// flash operations
extern char *v545_sprintf_checksum_flash(V545_REGS *);
extern int v545_load_flash(V545_REGS *, char *, int *);

// macro operation
extern int v545_execute_macro(V545_REGS *, unsigned short, unsigned short *, unsigned short *);

// configuration
extern int v545_config_synchro_input(V545_REGS *, int, int, int, int, int, int);
extern int v545_config_resolver_input(V545_REGS *, int, int, int, int, int);
extern int v545_config_synchro_output(V545_REGS *, int, int, int, int, int, int, int, int, int, int, float, float, float, int, int);
extern int v545_config_resolver_output(V545_REGS *, int, int, int, int, int, int, int, int, float, float, float, int, int);
extern int v545_config_lvdt_rvdt_input(V545_REGS *, int, int, int, int, int);
extern int v545_config_lvdt_rvdt_output(V545_REGS *, int, int, int, int, int, int, int, int, int, float, float, float, int);

// read/write devices
extern int v545_read_synchro_resolver(V545_REGS *, int, int, int, float, float, float *, float *, int);
extern int v545_write_synchro_resolver(V545_REGS *, int, float, float, float, int);
extern int v545_read_lvdt_rvdt(V545_REGS *, int, int, int, float, float, float *, float *);
extern int v545_write_lvdt_rvdt(V545_REGS *, int, float, float, float);

// read count value for position
extern int v545_read_synchro_resolver_position_counts(V545_REGS *, int, int *, int);
extern int v545_read_lvdt_rvdt_position_counts(V545_REGS *, int, int *);

// override block operations
extern int v545_config_override_position_velocity(V545_REGS *, int, int, float, float, int, int, float, float, int);
extern int v545_config_override_switch(V545_REGS *, int, int);
extern int v545_config_override_watchdog(V545_REGS *, int, float);
extern int v545_config_override_swtrigger(V545_REGS *, int);
extern int v545_set_override_swtrigger(V545_REGS *, int);

// cleanup
extern int v545_clear_function_block(V545_REGS *, int);
extern int v545_clear_function_blocks(V545_REGS *);
extern int v545_clear_active_override(V545_REGS *, int);
extern int v545_clear_override_block(V545_REGS *, int);
extern int v545_clear_override_blocks(V545_REGS *);
extern int v545_clear_regfile(V545_REGS *);

// debug
// extern int v545_get_debug_info(V545_REGS *);

// --------------------- NOTE: THE FOLLOWING ROUTINES ARE ONLY CALLED INTERNALLY ---------------------

extern int v545_write_function_code(V545_REGS *, int, unsigned short);
extern int v545_config_dds(V545_REGS *, int, int);
extern int v545_config_ccb(V545_REGS *, int, int);
extern int v545_is_a_valid_function_block_chnl(int);
extern int v545_is_a_valid_override_block_chnl(int);
extern int v545_override_get_regfile_index(V545_REGS *, int *);
extern int v545_get_fb_type(V545_REGS *, int);


#endif
