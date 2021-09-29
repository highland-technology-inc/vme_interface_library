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
// Date: Thu Sep 02 10:03:08 2021

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// prototypes

#include "ht_utils_proto.h"		// prototypes for ht_read_interlocked(), ht_write_interlocked(), and ht_write_macro()
#include "misc_math_utils_proto.h"
#include "misc_string_utils_proto.h"
#include "parse_args_proto.h"
#include "parse_options_proto.h"
#include "read_line_proto.h"
#include "v375_utils_proto.h"     

// structure defs

#include "v375_regs_sdef.h"

#define DEBUG  1                // printf errors while executing library


unsigned short v375_read_vximfr(V375_REGS *pb)
{
  return(pb->vximfr);
}


unsigned short v375_read_vxitype(V375_REGS *pb)
{
  return(pb->vxitype);
}


unsigned short v375_read_vxistatus(V375_REGS *pb)
{
  return(pb->vxistatus);
}


unsigned short v375_read_romid(V375_REGS *pb)
{
  return(pb->romid);
}


unsigned short v375_read_romrev(V375_REGS *pb)
{
  return(pb->romrev);
}


unsigned short v375_read_master(V375_REGS *pb)
{
  return(pb->master);
}


int v375_set_as_master(V375_REGS *pb)
{
  pb->master = 0x1;
  return(0);
}


int v375_unset_as_master(V375_REGS *pb)
{
  pb->master = 0x0;
  return(0);
}


unsigned short v375_read_resets(V375_REGS *pb)
{
  return(pb->resets);
}


int v375_write_resets(V375_REGS *pb, unsigned short chnl_mask)
{
  if ((chnl_mask & 0xfff0) != 0)
    return(-1);
  pb->resets = chnl_mask;
  return(0);
}


unsigned short v375_read_strobe(V375_REGS *pb)
{
  return(pb->strobe);
}


int v375_write_strobe(V375_REGS *pb, unsigned short chnl_mask)
{
  if ((chnl_mask & 0xfff0) != 0)
    return(-1);
  pb->strobe = chnl_mask;
  return(0);
}


unsigned short v375_read_vrun(V375_REGS *pb)
{
  return(pb->vrun);
}


unsigned int v375_read_tpass(V375_REGS *pb)
{
  return(ht_read_interlocked(&pb->tpass.hi));
}


unsigned int v375_read_terror(V375_REGS *pb)
{
  return(ht_read_interlocked(&pb->terror.hi));
}


unsigned short v375_read_tstop(V375_REGS *pb)
{
  return(pb->tstop);
}


int v375_clear_wavetable_memory(V375_REGS *pb, int chnl)
{
  int i;

  pb->chnl[chnl].ptr = 0;
  for (i = 0; i < V375_WAVETABLE_SIZE; i++)
    pb->chnl[chnl].wav = 0;
  return(0);
}


int v375_load_wavetable_to_memory(V375_REGS *pb, int chnl, int blksize, int blknum, short *pdata)
{
  int i;
  
  pb->chnl[chnl].ptr = blksize * blknum;
  for (i = 0; i < blksize; i++)
    pb->chnl[chnl].wav = *pdata++;
  return(0);
}


int v375_read_wavetable_from_memory(V375_REGS *pb, int chnl, int blksize, int blknum, short *pdata)
{
  int i;
  
  pb->chnl[chnl].ptr = blksize * blknum;
  for (i = 0; i < blksize; i++)
    *pdata++ = pb->chnl[chnl].wav;
  return(0);
}


int v375_load_waveform_start_location(V375_REGS *pb, int chnl, int blksize, int blknum)
{
  pb->chnl[chnl].bas = (unsigned short)(blksize * blknum);
  return(0);
}


// ------------------------------------------------------------------------------------------------

int v375_read_reg(V375_REGS *pb, unsigned short offset, unsigned short *reg_value)
{
  *reg_value = *(unsigned short *)((void *)pb + (offset & 0xfffe));
  return(0);
}


int v375_read_interlock_reg(V375_REGS *pb, unsigned short offset, unsigned int *reg_value)
{
  *reg_value = ht_read_interlocked((unsigned short *)((void *)pb + (offset & 0xfffc)));
  return(0);
}


int v375_write_interlock_reg(V375_REGS *pb, unsigned short offset, unsigned int reg_value)
{
  return(ht_write_interlocked((unsigned short *)((void *)pb + (offset & 0xfffc)), reg_value));
}

// ------------------------------------------------------------------------------------------------


// execute macros

int v375_execute_macro(V375_REGS *pb, unsigned short macro, unsigned short *params, int num_params, int *arg)
{
  int tpass, terror;
  int *parg = arg;
  
  switch(macro)
    {
    case V375_MACRO_BUILD_FOURIER_SERIES:
      ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0); // args loaded in v375_build_fourier_series[_vargs]()
      return(0);
      
    case V375_MACRO_ADD_FOURIER_SERIES: // not presently implemented
      return(0);
      
    case V375_MACRO_BUILD_GEAR_WAVEFORM:
      ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0); // args loaded in v375_build_gear_waveform[_vargs]()
      return(0);

    case V375_MACRO_BUILD_PULSE_TRAIN:
      ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0); // args loaded in v375_build_pulse_train[_vargs]()
      return(0);
      
    case V375_MACRO_OVERLAY_PULSE_TRAIN: // not presently implemented
      return(0);

    case V375_MACRO_LOAD_CONSTANT:
      ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0); // args loaded in v375_load_constant()
      return(0);
      
    case V375_MACRO_MEASURE_FREQ:
      return(0);
      
    case V375_MACRO_SWEEP_FREQ:
      return(0);
      
    case V375_MACRO_REINITIALIZE:
      pb->cp[0].us = 0x1029;
      pb->macro = macro;
      sleep(1);
      return(0);
      
      // params[0] = first word [0-249], params[1] = last word [1-250]

    case V375_MACRO_TEST_VME_DUAL_PORT_MEM: 
      if (num_params != 2)
        return(-1);
      ht_write_interlocked(&pb->tpass.hi, 0);
      ht_write_interlocked(&pb->terror.hi, 0);
      pb->tstop = 0x137;
      ht_write_macro((unsigned short *)&pb->macro, macro, params, num_params, 0);
      sleep(5);                 // test for 5 seconds
      pb->tstop = 0x0;          // stop test
      tpass = (int)ht_read_interlocked(&pb->tpass.hi);
      terror = (int)ht_read_interlocked(&pb->terror.hi);

      if (arg != (int *)0)
        {
          *parg++ = tpass;
          *parg++ = terror;
        }
      return(0);
      
      // params[0] = chnl [0-3], params[1] = first word [0-65534], params[2] = last word [1-65535]

    case V375_MACRO_TEST_WAVEFORM_MEM:      
      if (num_params != 3)
        return(-1);
      ht_write_interlocked(&pb->tpass.hi, 0);
      ht_write_interlocked(&pb->terror.hi, 0);
      pb->tstop = 0x137;
      ht_write_macro((unsigned short *)&pb->macro, macro, params, num_params, 0);
      sleep(5);                 // test for 5 seconds
      pb->tstop = 0x0;          // stop test
      tpass = (int)ht_read_interlocked(&pb->tpass.hi);
      terror = (int)ht_read_interlocked(&pb->terror.hi);

      if (arg != (int *)0)
        {
          *parg++ = tpass;
          *parg++ = terror;
        }
      return(0);
      
    case V375_MACRO_TEST_WATCHDOG_TIMER: // basically a watchdog reset
      pb->cp[0].us = 0x6666;
      ht_write_macro((unsigned short *)&pb->macro, macro, 0, 0, 0);
      sleep(5);
      return(0);
      
    case V375_MACRO_HIBERNATE:  // not presently implemented
      return(0);
      
    case V375_MACRO_TEST_CPU_STATIC_RAM:
      if (num_params != 0)
        return(-1);
      ht_write_interlocked(&pb->tpass.hi, 0);
      ht_write_interlocked(&pb->terror.hi, 0);
      pb->tstop = 0x137;
      ht_write_macro((unsigned short *)&pb->macro, macro, params, num_params, 0);
      sleep(5);                 // test for 5 seconds
      pb->tstop = 0x0;          // stop test
      tpass = (int)ht_read_interlocked(&pb->tpass.hi);
      terror = (int)ht_read_interlocked(&pb->terror.hi);

      if (arg != (int *)0)
        {
          *parg++ = tpass;
          *parg++ = terror;
        }
      return(0);

    default:
      return(-1);
    }
}


int v375_set_wavetable_size(V375_REGS *pb, int chnl, int size)
{
  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  switch(size)
    {
    case 64:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_64;
      break;
    case 128:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_128;
      break;
    case 256:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_256;
      break;
    case 512:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_512;
      break;
    case 1024:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_1K;
      break;
    case 2048:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_2K;
      break;
    case 4096:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_4K;
      break;
    case 8192:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_8K;
      break;
    case 16384:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_16K;
      break;
    case 32768:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_32K;
      break;
    case 65536:
      pb->chnl[chnl].siz = V375_WAVEFORM_SIZE_64K;
      break;
    default:
      return(-1);
    }

  return(0);
}


int v375_get_wavetable_size_and_blocks(V375_REGS *pb, int chnl, int *blksize, int *numblks)
{
  unsigned short __attribute__((unused)) size;

  size = pb->chnl[chnl].siz;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  switch(pb->chnl[chnl].siz)
    {
    case V375_WAVEFORM_SIZE_64:
      if (blksize != (int *)0)
        *blksize = 64;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_64;
      break;
    case V375_WAVEFORM_SIZE_128:
      if (blksize != (int *)0)
        *blksize = 128;
      if (numblks != (int *)0)
        *numblks =V375_WAVEFORM_NUMBLKS_128;
      break;
    case V375_WAVEFORM_SIZE_256:
      if (blksize != (int *)0)
        *blksize = 256;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_256;
      break;
    case V375_WAVEFORM_SIZE_512:
      if (blksize != (int *)0)
        *blksize = 512;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_512;
      break;
    case V375_WAVEFORM_SIZE_1K:
      if (blksize != (int *)0)
        *blksize = 1024;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_1K;
      break;
    case V375_WAVEFORM_SIZE_2K:
      if (blksize != (int *)0)
        *blksize = 2048;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_2K;
      break;
    case V375_WAVEFORM_SIZE_4K:
      if (blksize != (int *)0)
        *blksize = 4096;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_4K;
      break;
    case V375_WAVEFORM_SIZE_8K:
      if (blksize != (int *)0)
        *blksize = 8192;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_8K;
      break;
    case V375_WAVEFORM_SIZE_16K:
      if (blksize != (int *)0)
        *blksize = 16384;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_16K;
      break;
    case V375_WAVEFORM_SIZE_32K:
      if (blksize != (int *)0)
        *blksize = 32768;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_32K;
      break;
    case V375_WAVEFORM_SIZE_64K:
      if (blksize != (int *)0)
        *blksize = 65536;
      if (numblks != (int *)0)
        *numblks = V375_WAVEFORM_NUMBLKS_64K;
      break;
    default:
      return(-1);
    }

  return(0);
}


int v375_set_lowpass_filter(V375_REGS *pb, int chnl, int filter)
{
  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  switch(filter)
    {
    case V375_LOW_PASS_FILTER_3K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_3K;
      break;
    case V375_LOW_PASS_FILTER_6K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_6K;
      break;
    case V375_LOW_PASS_FILTER_15K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_15K;
      break;
    case V375_LOW_PASS_FILTER_30K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_30K;
      break;
    case V375_LOW_PASS_FILTER_60K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_60K;
      break;
    case V375_LOW_PASS_FILTER_150K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_150K;
      break;
    case V375_LOW_PASS_FILTER_300K:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_300K;
      break;
    case V375_LOW_PASS_FILTER_OFF:
      pb->chnl[chnl].flt = V375_LOW_PASS_FILTER_OFF;
      break;
    default:
      return(-1);
    }
  return(0);
}


// requires call to v375_set_wavetable_size_and_blocks() first

int v375_set_frequency(V375_REGS *pb, int chnl, float freq, int strobe_flag)
{
  unsigned int freq_counts;
  unsigned short chnl_mask = 1 << chnl;
  int blksize;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  freq_counts = (unsigned int)((double)freq * MAX_UINT * (double)blksize / V375_REFERENCE_SOURCE);

  ht_write_interlocked(&pb->freq[chnl].hi, freq_counts);

  if (strobe_flag)
    v375_write_strobe(pb, chnl_mask);

  return(0);
}


int v375_set_clock_source(V375_REGS *pb, int chnl, int clock_source)
{
  int master_flag;
  
  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  // if master_flag == 1, put all four cynthesizer frequencies and reset pulses onto the external daisy-chain bus

  master_flag = v375_read_master(pb) & 0x1; 

  if ((master_flag == 0) && ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD)))
    return(-1);

  if ((master_flag == 1) && ((chnl < 4) || (chnl >= 4 + V375_CHNLS_PER_BOARD)))
    return(-1);

  pb->chnl[chnl].src = (unsigned short)clock_source;
  return(0);
}


// must put chnl in reset prior to setting the divisor
// divisor ranges between 1-256

int v375_set_chnl_divisor(V375_REGS *pb, int chnl, int divisor)
{
  unsigned short chnl_mask = 1 << chnl;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);
  
  if ((v375_read_resets(pb) & chnl_mask) != 1)
    return(-1);

  if ((divisor < 0) || (divisor > 256))
    return(-1);

  pb->chnl[chnl].div = (unsigned short)(divisor - 1);
  return(0);
}


// multiplier ranges between -1.0 to 1.0

int v375_set_chnl_multiplier(V375_REGS *pb, int chnl, float multiplier)
{
  double dmultiplier = (double)multiplier;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  dmultiplier = ratio(dmultiplier, -1.0L, 1.0L, -32768.0L, 32768.0L);
  dmultiplier = clamp(dmultiplier, -32768.0, 32767.0);
  pb->amp[chnl] = (short)dmultiplier;
  return(0);
}


// voltage offset

int v375_set_chnl_dc_offset(V375_REGS *pb, int chnl, float offset)
{
  double doffset = (double)offset;
  
  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  doffset = ratio(doffset, -10.0L, 10.0L, -32768.0L, 32768.0L);
  doffset = clamp(doffset, -32768.0L, 32767.0L);
  pb->offset[chnl] = (short)doffset;
  return(0);
}


// phase shift

int v375_set_chnl_phase(V375_REGS *pb, int chnl, float deg_phase)
{
  int blksize = 0;
  unsigned short deg_phase_counts = 0;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (deg_phase < 0.0)
    return(-1);
  
  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  deg_phase_counts = deg_phase / (360.0 / (float)blksize);
  pb->chnl[chnl].pha = deg_phase_counts;
  return(0);
}


// define analog output summing mask

int v375_set_chnl_summing_mask(V375_REGS *pb, int chnl, unsigned short summing_mask)
{
  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if ((summing_mask & 0xfff0) != 0)
    return(-1);
  
  pb->chnl[chnl].sum = summing_mask;
  return(0);
}


// define the active block for chnl

int v375_set_new_active_block(V375_REGS *pb, int chnl, int active_block)
{
  int blksize;
  int numblks;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, &numblks) == -1)
    return(-1);

  if (active_block >= numblks)
    return(-1);

  // do a smooth transition at TDC

  pb->chnl[chnl].jba = (unsigned short)(active_block * blksize);
  pb->chnl[chnl].jph = 0;                  // transition at TDC
  pb->chnl[chnl].trg = blksize - 1;
  pb->chnl[chnl].jmp = 0x2;
  return(0);
}


// requires call to v375_set_wavetable_size_and_blocks() first

// the ... is amplitude / phase angle data pairs -------------------------------------,
//                                                                                    V
int v375_build_fourier_series_vargs(V375_REGS *pb, int chnl, int blknum, int numargs, ...)
{
  int i, j;
  va_list arglist;
  int blksize;
  struct {
    double amplitude;
    double phase;
  } harmonic[V375_MAX_BUILTIN_HARMONIC] = {[0 ... V375_MAX_BUILTIN_HARMONIC - 1] = {0.0L, 0.0L}};
  double phase;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)(numargs / 2); // data in pairs -- amp and pha

  if (numargs > V375_MAX_BUILTIN_HARMONIC * 2)
    numargs = V375_MAX_BUILTIN_HARMONIC * 2;

  va_start(arglist, numargs);

  for (i = 0; i < numargs / 2; i++) // i = data pair index
    {
      harmonic[i].amplitude = clamp(ratio(va_arg(arglist, double), -1.0L, 1.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pb->cp[j++].s = (short)harmonic[i].amplitude;
      
      phase = va_arg(arglist, double);
      if (phase < 0.0L)     // treat as signed
        {
          harmonic[i].phase = clamp(ratio(phase, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
          pb->cp[j++].s = (short)harmonic[i].phase;
        }
      else                  // treat as unsigned
        {
          harmonic[i].phase = clamp(ratio(phase, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
          pb->cp[j++].us = (unsigned short)harmonic[i].phase;
        }
    }  

  va_end(arglist);

  v375_execute_macro(pb, V375_MACRO_BUILD_FOURIER_SERIES, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_build_fourier_series(V375_REGS *pb, int chnl, int blknum, int numargs, float *arglist)
{
  int i, j;
  int blksize;
  struct {
    double amplitude;
    double phase;
  } harmonic[V375_MAX_BUILTIN_HARMONIC] = {[0 ... V375_MAX_BUILTIN_HARMONIC - 1] = {0.0L, 0.0L}};
  double phase;
  float *pargs = arglist;

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)(numargs / 2); // data in pairs -- amp and pha

  if (numargs > V375_MAX_BUILTIN_HARMONIC * 2)
    numargs = V375_MAX_BUILTIN_HARMONIC * 2;

  for (i = 0; i < numargs / 2; i++) // i = data pair index
    {
      harmonic[i].amplitude = clamp(ratio((double)*pargs++, -1.0L, 1.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pb->cp[j++].s = (short)harmonic[i].amplitude;
      
      phase = (double)*pargs++;
      if (phase < 0.0L)     // treat as signed
        {
          harmonic[i].phase = clamp(ratio(phase, -180.0L, 180.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
          pb->cp[j++].s = (short)harmonic[i].phase;
        }
      else                  // treat as unsigned
        {
          harmonic[i].phase = clamp(ratio(phase, 0.0L, 360.0L, 0.0L, 65536.0L), 0.0L, 65535.0L);
          pb->cp[j++].us = (unsigned short)harmonic[i].phase;
        }
    }  

  v375_execute_macro(pb, V375_MACRO_BUILD_FOURIER_SERIES, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_build_gear_waveform_vargs(V375_REGS *pb, int chnl, int blknum, int num_teeth, float dc_baseline, float dc_top, float pulse_width, int numargs, ...)
{
  int i, j;
  va_list arglist;
  int blksize;
  struct {
    int index;                  // tooth number
    float level;
  } pulse[V375_MAX_BUILTIN_SHORT_TEETH + 1] = {[0 ... V375_MAX_BUILTIN_SHORT_TEETH] = {0, 0.0}};

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  if (inrange((double)num_teeth, 1.0L, 512.0L) == -1)
    return(-1);
  
  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize);	// starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)num_teeth;  
  pb->cp[j++].s = (short)clamp(ratio((double)dc_baseline, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pb->cp[j++].us = 0;					// unused
  pb->cp[j++].us = (unsigned short)((pulse_width / 360.0) * (float)blksize);
  pb->cp[j++].s = (short)clamp(ratio((double)dc_top, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);

  va_start(arglist, numargs);

  for (i = 0; i < numargs / 2; i++) // i = data pair index
    {
      pulse[i].index = (int)(va_arg(arglist, double)); // tooth number
      pb->cp[j++].us = (unsigned short)pulse[i].index;
      
      pulse[i].level = (float)va_arg(arglist, double);
      pb->cp[j++].s = (short)clamp(ratio((double)pulse[i].level, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
    }  

  va_end(arglist);

  pb->cp[j++].us = 0;
  pb->cp[j++].us = 0;

  v375_execute_macro(pb, V375_MACRO_BUILD_GEAR_WAVEFORM, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_build_gear_waveform(V375_REGS *pb, int chnl, int blknum, int num_teeth, float dc_baseline, float dc_top, float pulse_width, int numargs, float *arglist)
{
  int i, j;
  int blksize;
  struct {
    int index;                  // tooth number
    float level;
  } pulse[V375_MAX_BUILTIN_SHORT_TEETH + 1] = {[0 ... V375_MAX_BUILTIN_SHORT_TEETH] = {0, 0.0}};
  float *pargs = arglist;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  if (inrange((double)num_teeth, 1.0L, 512.0L) == -1)
    return(-1);
  
  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory

  pb->cp[j++].us = (unsigned short)num_teeth;  
  pb->cp[j++].s = (short)clamp(ratio((double)dc_baseline, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pb->cp[j++].us = 0;           // unused
  pb->cp[j++].us = (unsigned short)((pulse_width / 360.0) * (float)blksize);
  pb->cp[j++].s = (short)clamp(ratio((double)dc_top, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);

  for (i = 0; i < numargs / 2; i++) // i = data pair index
    {
      pulse[i].index = (int)*pargs++; // tooth number
      pb->cp[j++].us = (unsigned short)pulse[i].index;
      
      pulse[i].level = *pargs++;
      pb->cp[j++].s = (short)clamp(ratio((double)pulse[i].level, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
    }  

  pb->cp[j++].us = 0;
  pb->cp[j++].us = 0;

  v375_execute_macro(pb, V375_MACRO_BUILD_GEAR_WAVEFORM, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_build_pulse_train_vargs(V375_REGS *pb, int chnl, int blknum, int num_pulses, float dc_baseline, int numargs, ...)
{
  int i, j;
  va_list arglist;
  int blksize;
  struct {
    float level;
    float position;
    float width;
  } pulse[V375_MAX_BUILTIN_PULSES] = {[0 ... V375_MAX_BUILTIN_PULSES - 1] = {0.0, 0.0, 0.0}};

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)num_pulses;
  pb->cp[j++].s = (short)clamp(ratio((double)dc_baseline, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pb->cp[j++].us = 0;           // unused

  va_start(arglist, numargs);
  
  for (i = 0; i < num_pulses; i++)
    {
      pulse[i].level = (float)(va_arg(arglist, double));
      pb->cp[j++].s = (short)clamp(ratio((double)pulse[i].level, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pulse[i].position = (float)(va_arg(arglist, double));
      pb->cp[j++].us = (unsigned short)((pulse[i].position / 360.0) * (float)blksize);
      pulse[i].width = (float)(va_arg(arglist, double));
      pb->cp[j++].us = (unsigned short)((pulse[i].width / 360.0) * (float)blksize);
    }

  va_end(arglist);
  
  v375_execute_macro(pb, V375_MACRO_BUILD_PULSE_TRAIN, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_build_pulse_train(V375_REGS *pb, int chnl, int blknum, int num_pulses, float dc_baseline, int numargs, float *arglist)
{
  int i, j;
  int blksize;
  struct {
    float level;
    float position;
    float width;
  } pulse[V375_MAX_BUILTIN_PULSES] = {[0 ... V375_MAX_BUILTIN_PULSES - 1] = {0.0, 0.0, 0.0}};
  float *pargs = arglist;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)num_pulses;
  pb->cp[j++].s = (short)clamp(ratio((double)dc_baseline, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
  pb->cp[j++].us = 0;           // unused

  for (i = 0; i < num_pulses; i++)
    {
      pulse[i].level = *pargs++;
      pb->cp[j++].s = (short)clamp(ratio((double)pulse[i].level, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
      pulse[i].position = *pargs++;
      pb->cp[j++].us = (unsigned short)((pulse[i].position / 360.0) * (float)blksize);
      pulse[i].width = *pargs++;
      pb->cp[j++].us = (unsigned short)((pulse[i].width / 360.0) * (float)blksize);
    }

  v375_execute_macro(pb, V375_MACRO_BUILD_PULSE_TRAIN, 0, 0, 0); // args already loaded in this routine
  return(0);
}


int v375_load_constant(V375_REGS *pb, int chnl, int blknum, float dc)
{
  int j;
  int blksize;

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if (v375_get_wavetable_size_and_blocks(pb, chnl, &blksize, 0) == -1)
    return(-1);

  j = 0;
  pb->cp[j++].us = (unsigned short)chnl;
  pb->cp[j++].us = (unsigned short)(blknum * blksize); // starting location in channel wavetable memory
  pb->cp[j++].us = (unsigned short)(blknum * blksize + blksize - 1); // ending location in channel wavetable memory
  pb->cp[j++].s = (short)clamp(ratio((double)dc, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);

  v375_execute_macro(pb, V375_MACRO_LOAD_CONSTANT, 0, 0, 0); // args already loaded in this routine
  return(0);
}


#define V375_MAX_INPUT_LINE 256
#define V375_MAX_ARG_V 32

// chgflg assignments

#define V375_WAVETABLE_TYPE		0x0000000000000001L
#define V375_WAVETABLE_BLKSIZE		0x0000000000000002L
#define V375_WAVETABLE_BLKNUM		0x0000000000000004L
#define V375_WAVETABLE_AMPLITUDE	0x0000000000000008L
#define V375_WAVETABLE_PHASE		0x0000000000000010L
#define V375_WAVETABLE_NUM_TEETH	0x0000000000000020L
#define V375_WAVETABLE_PULSE_BASE	0x0000000000000040L
#define V375_WAVETABLE_PULSE_HEIGHT	0x0000000000000080L
#define V375_WAVETABLE_PULSE_WIDTH	0x0000000000000100L

int v375_load_wavetable(V375_REGS *pb, int chnl, char *wavefile, int *pblksize)
{
  int i, j, k;
  FILE *fpi;
  int num_chars;
  char inputline[V375_MAX_INPUT_LINE] = {""};
  char parsed_inputline[V375_MAX_INPUT_LINE] = {""};
  int arg_c;
  char *arg_v[V375_MAX_ARG_V];
  short wavedata[V375_CHNLS_PER_BOARD][V375_WAVETABLE_SIZE];

  // option table parameters

  static int wavespec_type = 0;
  static int blksize = 0;
  static int blknum = 0;
  static float amplitude = 0.0;
  static float phase = 0.0;
  static int num_teeth = 0;
  static float pulse_base = 0;
  static float pulse_height = 0;
  static float pulse_width = 0;
  static unsigned long chgflg;

  // additional parameters used in raw waveform processing from wavetable file

  int start_index, end_index;

  // additional parameters used in sine waveform processing from wavetable file

  float sine_amplitude;
  int counts;

  // additional parameters used in fourier waveform processing from wavetable file

  int harmonic;

  struct {
    float amplitude;
    float phase;
  } fourier[V375_MAX_WAVESPEC_HARMONIC] = {[ 0 ... V375_MAX_WAVESPEC_HARMONIC - 1] = {0.0, 0.0}};


  // additional parameters used in gear waveform processing from wavetable file

  double __attribute__((unused)) fraction;
  double integral_d;
  int integral_i;
  int tooth_num;
  int tooth_half;
  int tooth_match;
  int entry;

  struct {
    float amplitude;
    int tooth_num;
  } short_tooth[V375_MAX_WAVESPEC_SHORT_TEETH] = {[ 0 ... V375_MAX_WAVESPEC_SHORT_TEETH - 1] = {0.0, 0.0}};
  

  // additional parameters used in pulse_train waveform processing from wavetable file
  
  float degrees;
  float width;

  struct {
    float amplitude;
    float phase;
    float width;
  } pulse[V375_MAX_WAVESPEC_PULSES] = {[ 0 ... V375_MAX_WAVESPEC_PULSES - 1] = {0.0, 0.0}};


  static OPTION_TABLE option_table[] = {
    // opt_str		arg_str		arg_value			data			type_mask	routine		chgflg_mask
    {"-t",		"raw",		V375_WAVESPEC_TYPE_RAW,	        (void *)&wavespec_type,	T_INT,		load_enum,	V375_WAVETABLE_TYPE},	
    {"-t",		"sine",		V375_WAVESPEC_TYPE_SINE,	(void *)&wavespec_type,	T_INT,		load_enum,	V375_WAVETABLE_TYPE},	
    {"-t",		"fourier",	V375_WAVESPEC_TYPE_FOURIER,     (void *)&wavespec_type,	T_INT,		load_enum,	V375_WAVETABLE_TYPE},	
    {"-t",		"gear",		V375_WAVESPEC_TYPE_GEAR,        (void *)&wavespec_type,	T_INT,		load_enum,	V375_WAVETABLE_TYPE},	
    {"-t",		"pulse_train",	V375_WAVESPEC_TYPE_PULSETRAIN,	(void *)&wavespec_type,	T_INT,		load_enum,	V375_WAVETABLE_TYPE},
    {"-bs",		"64",		64,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"128",		128,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"256",		256,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"512",		512,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"1024",		1024,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"2048",		2048,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"4096",		4096,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"8192",		8192,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"16384",	16384,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"32768",	32768,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"65536",	65536,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"1K",		1024,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"2K",		2048,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"4K",		4096,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"8K",		8192,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"16K",		16384,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"32K",		32768,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bs",		"64K",		65536,				(void *)&blksize,	T_INT,		load_enum,	V375_WAVETABLE_BLKSIZE},
    {"-bn",		"*",		0,				(void *)&blknum,	T_INT,		load_constant,	V375_WAVETABLE_BLKNUM},
    {"-amp",		"*",		0,				(void *)&amplitude,	T_FLOAT,	load_constant,	V375_WAVETABLE_AMPLITUDE},
    {"-pha",		"*",		0,				(void *)&phase,		T_FLOAT,	load_constant,	V375_WAVETABLE_PHASE},
    {"-nt",		"*",		0,				(void *)&num_teeth,	T_INT,		load_constant,	V375_WAVETABLE_NUM_TEETH},
    {"-pb",		"*",		0,				(void *)&pulse_base,	T_FLOAT,	load_constant,	V375_WAVETABLE_PULSE_BASE},
    {"-ph",		"*",		0,				(void *)&pulse_height,	T_FLOAT,	load_constant,	V375_WAVETABLE_PULSE_HEIGHT},
    {"-pw",		"*",		0,				(void *)&pulse_width,	T_FLOAT,	load_constant,	V375_WAVETABLE_PULSE_WIDTH},
    {"",		"",		0,				(void *)0,		0,		0,		0}
  };

  if ((chnl < 0) || (chnl >= V375_CHNLS_PER_BOARD))
    return(-1);

  if ((fpi = fopen(wavefile, "r")) == (FILE *)0)
    return(-1);

  while ((num_chars = read_line(inputline, V375_MAX_INPUT_LINE, fpi)) > 0)
    {
    next_wavespec:
      arg_c = parse_args(inputline, arg_v, V375_MAX_ARG_V, parsed_inputline, V375_MAX_INPUT_LINE);

      if (strlen(inputline) == 0)
        continue;

      if (strncmp(arg_v[0], "#", 1) == 0)
        continue;

      if (strcmp(arg_v[0], "wavespec") != 0)
        continue;

      parse_options(arg_c - 1, &arg_v[1], option_table, &chgflg);

      if (blksize != 0)
        *pblksize = blksize;

      switch(wavespec_type)
        {
        case V375_WAVESPEC_TYPE_RAW:
          while ((num_chars = read_line(inputline, V375_MAX_INPUT_LINE, fpi)) > 0)
            {
              remove_leading_whitespace(inputline);

              if (strlen(inputline) == 0) // indicates end of data listed in wavetable file
                break;

              if (!isdigit(inputline[0])) // usually indicates new wavespec command
                goto next_wavespec;

              if (sscanf(inputline, "%d-%d %f", &start_index, &end_index, &amplitude) == 3)
                {
                  for (i = (blknum * blksize) + start_index; i <= (blknum * blksize) + end_index; i++)
                    wavedata[chnl][i] = (short)(clamp(ratio(amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
                }
              else if (sscanf(inputline, "%d %f", &i, &amplitude) == 2)
                wavedata[chnl][i] = (short)(clamp(ratio(amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
              else
                break;
            }

          v375_load_wavetable_to_memory(pb, chnl, blksize, blknum, &wavedata[chnl][blknum * blksize]);
          break;

        case V375_WAVESPEC_TYPE_SINE:
          for (i = blknum * blksize, j = 0; i < (blknum + 1) * blksize; i++, j++)
            {
              sine_amplitude = (float)sin((M_PI * 2.0L) * ((double)j / (double)blksize) + (M_PI * 2.0L) * ((double)phase / 360.0L)) * amplitude;
              counts = (int)clamp(ratio(sine_amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
              wavedata[chnl][i] = (short)counts;
            }

          v375_load_wavetable_to_memory(pb, chnl, blksize, blknum, &wavedata[chnl][blknum * blksize]);
          break;

        case V375_WAVESPEC_TYPE_FOURIER:
          memset((void *)fourier, 0, sizeof(fourier));
          while ((num_chars = read_line(inputline, V375_MAX_INPUT_LINE, fpi)) > 0)
            {
              remove_leading_whitespace(inputline);

              if (strlen(inputline) == 0) // indicates end of data listed in wavetable file
                break;

              if (!isdigit(inputline[0])) // usually indicates new wavespec command
                goto next_wavespec;

              if (sscanf(inputline, "%d %f %f", &harmonic, &amplitude, &phase) != 3)
                break;

              fourier[harmonic].amplitude = amplitude;
              fourier[harmonic].phase = phase;
            }

          for (i = blknum * blksize, j = 0; i < (blknum + 1) * blksize; i++, j++)
            {
              amplitude = fourier[0].amplitude; // DC offset

              for (k = 1; k <= harmonic; k++)
                amplitude += (float)sin((M_PI * 2.0L * (double)k) * ((double)j / (double)blksize) + (M_PI * 2.0L * (double)k) * ((double)fourier[k].phase / 360.0L)) * fourier[k].amplitude;

              counts = (int)clamp(ratio(amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L);
              wavedata[chnl][i] = (short)counts;
            }
          
          v375_load_wavetable_to_memory(pb, chnl, blksize, blknum, &wavedata[chnl][blknum * blksize]);
          break;

        case V375_WAVESPEC_TYPE_GEAR:
          memset((void *)short_tooth, 0, sizeof(short_tooth));
          while ((num_chars = read_line(inputline, V375_MAX_INPUT_LINE, fpi)) > 0)
            {
              remove_leading_whitespace(inputline);

              if (strlen(inputline) == 0) // indicates end of data listed in wavetable file
                break;

              if (!isdigit(inputline[0])) // usually indicates new wavespec command
                goto next_wavespec;

              if (sscanf(inputline, "%d %f %d", &entry, &amplitude, &tooth_num) != 3)
                break;

              short_tooth[entry].amplitude = amplitude;
              short_tooth[entry].tooth_num = tooth_num;
            }

          for (i = 0; i < blksize; i++)
            {
              // deg_position = (float)i / (float)blksize * 360.0;
              
              tooth_num = (int)((float)i / (float)blksize * num_teeth);
              tooth_half = (int)((float)i / (float)blksize * num_teeth * 2);
              fraction = modf((double)tooth_half, &integral_d);
              integral_i = (int)integral_d;

              // odd modulo means low part of the tooth half, even modulo means high part of tooth half

              if ((integral_i & 0x1) == 0x1)
                wavedata[chnl][i] = (short)(clamp(ratio((double)pulse_base, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
              else              // high tooth half
                {               
                  tooth_match = 0;
                  for (j = 0; j < V375_MAX_WAVESPEC_SHORT_TEETH; j++)
                    {
                      if (tooth_num == short_tooth[j].tooth_num)
                        {
                          tooth_match = 1;
                          break;
                        }
                    }

                  if (tooth_match == 1)
                    wavedata[chnl][i] = (short)(clamp(ratio((double)short_tooth[j].amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
                  else
                    wavedata[chnl][i] = (short)(clamp(ratio((double)pulse_height, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
                }
            }

          v375_load_wavetable_to_memory(pb, chnl, blksize, blknum, &wavedata[chnl][blknum * blksize]);
          break;

        case V375_WAVESPEC_TYPE_PULSETRAIN:
          memset((void *)pulse, 0, sizeof(pulse));
          while ((num_chars = read_line(inputline, V375_MAX_INPUT_LINE, fpi)) > 0)
            {
              remove_leading_whitespace(inputline);

              if (strlen(inputline) == 0) // indicates end of data listed in wavetable file
                break;

              if (!isdigit(inputline[0])) // usually indicates new wavespec command
                goto next_wavespec;

              if (sscanf(inputline, "%d %f %f %f", &entry, &amplitude, &phase, &width) != 4)
                break;

              pulse[entry].amplitude = amplitude;
              pulse[entry].phase = phase;
              pulse[entry].width = width;
            }

          for (i = 0; i < blksize; i++)
            {
              degrees = (double)i / (double)blksize * 360.0; // degress = f(current point in block)
              amplitude = pulse_base; // amplitude = default pulse_base

              for (j = 0; j < V375_MAX_WAVESPEC_PULSES; j++)
                {
                  if (pulse[j].width == 0.0) // if at end of data set with no match
                    break;

                  // if in defined pulse range
                  
                  if ((degrees >= pulse[j].phase) && (degrees < pulse[j].phase + pulse[j].width))
                    {
                      amplitude = pulse[j].amplitude;
                      break;
                    }
                }

              wavedata[chnl][i] = (short)(clamp(ratio((double)amplitude, -10.0L, 10.0L, -32768.0L, 32768.0L), -32768.0L, 32767.0L));
            }

          v375_load_wavetable_to_memory(pb, chnl, blksize, blknum, &wavedata[chnl][blknum * blksize]);
          break;

        default:
          return(-1);
        }
    }

  return(0);
}


// All in one call to configure and start an arbitrary waveform channel define in "wavefilename".
//
// Arguments;
//   if dds == -1, then use default
//   if summing_mask = -1, then use default
//   is low_pass_filter == -1, then use default (6: 300K)

int v375_config_chnl_wavefile(V375_REGS *pb, int chnl, char *wavefilename, int dds, int summing_mask, int low_pass_filter, float *freq_param, float *multiplier_param, float *phase_shift_param, float *dc_offset_param, int *divisor_param, int *active_block_param)
{
  unsigned short chnl_mask = 1 << chnl;
  int blksize;
  unsigned short resets_mask;
  
  resets_mask = v375_read_resets(pb);
  resets_mask |= chnl_mask;
  v375_write_resets(pb, resets_mask);

  if (v375_load_wavetable(pb, chnl, wavefilename, &blksize) == -1)
    return(-1);

  if (v375_set_wavetable_size(pb, chnl, blksize) == -1)
    return(-1);

  // set DDS clock source 

  if (dds == -1)
    {
      if (v375_set_clock_source(pb, chnl, chnl) == -1)
        return(-1);
    }
  else
    {
      if (v375_set_clock_source(pb, chnl, dds) == -1)
        return(-1);
    }

  // set analog output summing mask

  if (summing_mask == -1)
    {
      if (v375_set_chnl_summing_mask(pb, chnl, chnl_mask) == -1)
        return(-1);
    }
  else
    {
      if (v375_set_chnl_summing_mask(pb, chnl, summing_mask) == -1)
        return(-1);
    }
      
  // set low pass filter

  if (low_pass_filter == -1)
    {
      if (v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_300K) == -1)
        return(-1);
    }
  else
    {
      if (v375_set_lowpass_filter(pb, chnl, low_pass_filter) == -1)
        return(-1);
    }
    
  // set initial frequency

  if (v375_set_frequency(pb, chnl, *freq_param, 1) == -1)
    return(-1);

  // set multiplier
  
  if (v375_set_chnl_multiplier(pb, chnl, *multiplier_param) == -1)
    return(-1);

  // set phase shift
  
  if (v375_set_chnl_phase(pb, chnl, *phase_shift_param) == -1)
    return(-1);
  
  // set channel dc offset
  
  if (v375_set_chnl_dc_offset(pb, chnl, *dc_offset_param) == -1)
    return(-1);
  
  // set channel divisor

  if (v375_set_chnl_divisor(pb, chnl, *divisor_param) == -1)
    return(-1);

  // set initial block number

  if (v375_load_waveform_start_location(pb, chnl, blksize, *active_block_param) == -1)
    return(-1);
  
  // take channel out of reset

  resets_mask = v375_read_resets(pb);
  resets_mask &= ~chnl_mask;
  v375_write_resets(pb, resets_mask);

  return(0);
}


// int v375_config_chnl_builtin_vargs(V375_REGS *pb, int chnl, char *waveform_type, int blksize, int dds, int summing_mask, int low_pass_filter, float *freq_param, float *multiplier_param, float *phase_shift_param, float *dc_offset_param, int *active_block_param, int numargs, ...)
// {

//   return(0);
// }


// int v375_config_chnl_builtin(V375_REGS *pb, int chnl, char *waveform_type, int blksize, int dds, int summing_mask, int low_pass_filter, float *freq_param, float *multiplier_param, float *phase_shift_param, float *dc_offset_param, int *active_block_param, int numargs, float *arglist)
// {

//   return(0);
// }



