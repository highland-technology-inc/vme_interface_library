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
// Date: Thu Sep 02 13:49:36 2021

#ifndef _LIBV375_PROTO_H_
#define _LIBV375_PROTO_H_

#ifndef _V375_SDEF_H_
#include "v375_regs_sdef.h"
#endif

// read/write registers

extern unsigned short v375_read_vximfr(V375_REGS *);
extern unsigned short v375_read_vxitype(V375_REGS *);
extern unsigned short v375_read_vxistatus(V375_REGS *);
extern unsigned short v375_read_romid(V375_REGS *);
extern unsigned short v375_read_romrev(V375_REGS *);
extern unsigned short v375_read_master(V375_REGS *);
extern int v375_set_as_master(V375_REGS *);
extern int v375_unset_as_master(V375_REGS *);
extern unsigned short v375_read_resets(V375_REGS *);
extern int v375_write_resets(V375_REGS *, unsigned short);
extern unsigned short v375_read_strobe(V375_REGS *);
extern int v375_write_strobe(V375_REGS *, unsigned short);
extern unsigned short v375_read_vrun(V375_REGS *);
extern unsigned int v375_read_tpass(V375_REGS *);
extern unsigned int v375_read_terror(V375_REGS *);
extern unsigned short v375_read_tstop(V375_REGS *);
extern int v375_clear_wavetable_memory(V375_REGS *, int);
extern int v375_load_wavetable_to_memory(V375_REGS *, int, int, int, short *);
extern int v375_read_wavetable_from_memory(V375_REGS *, int, int, int, short *);
extern int v375_load_waveform_start_location(V375_REGS *, int, int, int);

extern int v375_read_reg(V375_REGS *, unsigned short, unsigned short *);
extern int v375_read_interlock_reg(V375_REGS *, unsigned short, unsigned int *);
extern int v375_write_interlock_reg(V375_REGS *, unsigned short, unsigned int);

// macro execution

extern int v375_execute_macro(V375_REGS *, unsigned short, unsigned short *, int, int *);

// configuration

extern int v375_set_wavetable_size(V375_REGS *, int, int);
extern int v375_get_wavetable_size_and_blocks(V375_REGS *, int, int *, int *);
extern int v375_set_lowpass_filter(V375_REGS *, int, int);
extern int v375_set_frequency(V375_REGS *, int, float, int);
extern int v375_set_clock_source(V375_REGS *, int, int);
extern int v375_set_chnl_divisor(V375_REGS *, int, int);
extern int v375_set_chnl_multiplier(V375_REGS *, int, float);
extern int v375_set_chnl_dc_offset(V375_REGS *, int, float);
extern int v375_set_chnl_phase(V375_REGS *, int, float);
extern int v375_set_chnl_summing_mask(V375_REGS *, int, unsigned short);
extern int v375_set_new_active_block(V375_REGS *, int, int);

// built-in waveform capabilities

extern int v375_build_fourier_series_vargs(V375_REGS *, int, int, int, ...);
extern int v375_build_fourier_series(V375_REGS *, int, int, int, float *);
extern int v375_build_gear_waveform_vargs(V375_REGS *, int, int, int, float, float, float, int, ...);
extern int v375_build_gear_waveform(V375_REGS *, int, int, int, float, float, float, int, float *);
extern int v375_build_pulse_train_vargs(V375_REGS *, int, int, int, float, int, ...);
extern int v375_build_pulse_train(V375_REGS *, int, int, int, float, int, float *);
extern int v375_load_constant(V375_REGS *, int, int, float);

// extern int v375_config_chnl_builtin(V375_REGS *, int, int, int, int, char *, float *, float *, float *, float *, int *, int, float *);
  
// load wavetable files

extern int v375_load_wavetable(V375_REGS *, int, char *, int *);

// all in one call

extern int v375_config_chnl_wavefile(V375_REGS *, int, char *, int, int, int, float *, float *, float *, float *, int *, int *);
#endif
