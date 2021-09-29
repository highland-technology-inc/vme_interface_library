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
// Date: Thu Sep 02 14:22:23 2021

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

// usage:
//   ./v375 ./v375 -c chnl -F wavefile_name -f freq -d -r -S sleeptime

#include <sys/mman.h>
#include <sys/time.h>

#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <V120.h>

// prototypes

#include "deg_conv_proto.h"
#include "get_vaddr_for_device_proto.h"
#include "ht_utils_proto.h"
#include "misc_math_utils_proto.h"
#include "v375_utils_proto.h"


int main(int argc, char *argv[])
{
  V375_REGS *pb;
  char *whoami = argv[0];
  char c;
  int debug_flag = 0;
  int reset_flag = 0;
  char usage[] = {"./v375 -c chnl -F wavefile_name -f freq -d -r -S sleeptime\n"};
  int chnl = 0;
  unsigned short chnl_mask = 1 << chnl;
  float freq = 1000.0;
  char wavefile_name[256] = { [0 ... 255] = '\0'};
  char __attribute__((unused)) ans;
  int sleeptime = 3;

  int blksize = 256;
  int blknum = 0;
  int num_teeth;

  float dc_baseline;
  float dc_top;
  float pulse_width;

  while ((c = getopt(argc, argv, "c:f:F:drS:\?")) != (char)EOF)
    switch(c)
      {
      case 'c':
        if (sscanf(optarg, "%d", &chnl) != 1)
          goto contin;
        chnl_mask = 1 << chnl;
        break;

      case 'f':
        if (sscanf(optarg, "%f", &freq) != 1)
          goto contin;
        break;

      case 'F':
        strcpy(wavefile_name, optarg);
        break;
        
      case 'd':
        debug_flag = 1;
        break;
        
      case 'r':
        reset_flag = 1;
        break;

      case 'S':
        if (sscanf(optarg, "%d", &sleeptime) != 1)
          goto contin;
        break;
        
      case '\?':
        printf("%s", usage);
        exit(1);

      default:
      contin:
        printf("ERROR %s/%s: conversion error for '-%c %s' option/arg\n", whoami, __FUNCTION__, c, optarg);
        exit(1);
      }

  // map to board

  pb = (V375_REGS *)get_vaddr_for_device("output.conf", "v375[0][0]");

  if (reset_flag)
    {
      if (v375_execute_macro(pb, V375_MACRO_REINITIALIZE, 0, 0, 0) == -1)
        {
          printf("ERROR %s/%s: v375_execute_macro() failure\n", whoami, __FUNCTION__);
          exit(1);
        }
    }
  
  if (debug_flag)
    {
      printf("vximfr: 0x%4.4hx\n", v375_read_vximfr(pb));
      printf("vxitype: %hd (0x%4.4hx)\n", v375_read_vxitype(pb), v375_read_vxitype(pb));
      printf("vxistatus: %hd (0x%4.4hx)\n", v375_read_vxistatus(pb), v375_read_vxistatus(pb));

      printf("rom id: %d (0x%4.4hx)\n", (int)v375_read_romid(pb), (int)v375_read_romid(pb));
      printf("rom rev: %c\n", (char)v375_read_romrev(pb));
      printf("master: 0x%4.4hx\n", v375_read_master(pb));
      printf("resets: 0x%4.4hx\n", v375_read_resets(pb));
      printf("strobe: 0x%4.4hx\n", v375_read_strobe(pb));
      printf("vrun: 0x%4.4hx\n", v375_read_vrun(pb));

      printf("tpass: 0x%8.8x\n", v375_read_tpass(pb));
      printf("terror: 0x%8.8x\n", v375_read_terror(pb));
      printf("tstop: 0x%4.4hx\n", v375_read_tstop(pb));
    }
  
  // ------------------------------------------------------------------------------------------------------

  // Generate a sine wave with a 1st harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |    0th       1st    
  //                                                |  harmonic  harmonic 
  //                                                |  DC offset fundmntl
  //                                                |  amp  pha  amp  pha 
  //                                                V  /------\  /------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 4, 0.0, 0.0, 0.5, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a sine wave with a 2nd harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |    0th       1st       2nd
  //                                                |  harmonic  harmonic  harmonic
  //                                                |  DC offset fundmntl
  //                                                |  amp  pha  amp  pha  amp  pha
  //                                                V  /------\  /------\  /------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 6, 0.0, 0.0, 0.5, 0.0, 0.4, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a sine wave with a 3rd harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |    0th       1st       2nd       3nd   
  //                                                |  harmonic  harmonic  harmonic  harmonic
  //                                                |  DC offset fundmntl                    
  //                                                |  amp  pha  amp  pha  amp  pha  amp  pha
  //                                                V  /------\  /------\  /------\  /------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 8, 0.0, 0.0, 0.5, 0.0, 0.4, 0.0, 0.3, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a sine wave with a 4th harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |     0th       1st       2nd       3nd       4nd   
  //                                                |   harmonic  harmonic  harmonic  harmonic  harmonic
  //                                                |   DC offset fundmntl                              
  //                                                |   amp  pha  amp  pha  amp  pha  amp  pha  amp  pha
  //                                                V   /------\  /------\  /------\  /------\  /------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 10, 0.0, 0.0, 0.5, 0.0, 0.4, 0.0, 0.3, 0.0, 0.2, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a square wave with a 5th harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |     0th       1st       2nd       3nd       4nd        5nd   
  //                                                |   harmonic  harmonic  harmonic  harmonic  harmonic   harmonic
  //                                                |   DC offset fundmntl                                        
  //                                                |   amp  pha  amp  pha  amp  pha  amp  pha  amp  pha  amp    pha
  //                                                V   /------\  /------\  /------\  /------\  /------\  /--------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 12, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.120, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a square wave with a 7th harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |     0th       1st       2nd       3nd       4nd        5nd         6nd         7nd     
  //                                                |   harmonic  harmonic  harmonic  harmonic  harmonic   harmonic    harmonic    harmonic  
  //                                                |   DC offset fundmntl                                                                   
  //                                                |   amp  pha  amp  pha  amp  pha  amp  pha  amp  pha  amp    pha  amp    pha  amp    pha 
  //                                                V   /------\  /------\  /------\  /------\  /------\  /--------\  /--------\  /---------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 16, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.120, 0.0, 0.0,   0.0, 0.0857, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  // Generate a square wave with a fifth harmonic using the V375 built-in function.

  blksize = 256;
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                             num_args
  //                                                | 
  //                                                |     0th       1st       2nd       3nd       4nd        5nd         6nd         7nd          8nd         9nd     
  //                                                |   harmonic  harmonic  harmonic  harmonic  harmonic   harmonic    harmonic    harmonic     harmonic    harmonic  
  //                                                |   DC offset fundmntl                                                                                            
  //                                                |   amp  pha  amp  pha  amp  pha  amp  pha  amp  pha  amp    pha  amp    pha  amp    pha   amp    pha  amp    pha 
  //                                                V   /------\  /------\  /------\  /------\  /------\  /--------\  /--------\  /---------\  /--------\  /---------\ .
  v375_build_fourier_series_vargs(pb, chnl, blknum, 20, 0.0, 0.0, 0.6, 0.0, 0.0, 0.0, 0.2, 0.0, 0.0, 0.0, 0.120, 0.0, 0.0,   0.0, 0.0857, 0.0, 0.0,   0.0, 0.0667, 0.0);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Generate a gear waveform using the V375 built-in function.  There are 4 teath on the gear, with the tooth #1 being a short tooth with 5v amplitude,
  // and tooth #2 having an amplitude of 7.5v.  The last two teeth have an amplitude of 10v.

  // NOTE: There appears to be a bug in the "Build Gear Waveform" macro commond on the V375 firmware.
  // This only works correctly when the blksize = 64K.  At half the blocksize, the tooth width is half as wide as it should be.

  blksize = 65536.0;            // REQUIRED
  blknum = 0;
  freq = 1000.0;

  num_teeth = 4;
  dc_baseline = 0.0;
  dc_top = 10.0;
  pulse_width = 45.0;

  v375_write_resets(pb, chnl_mask);                             // hold chnl 0 in reset condition
  v375_set_wavetable_size(pb, chnl, blksize);                   // define chnl 0 wavetable size = 256

  //                                                                                         num_args
  //                                                                                            |  
  //                                                                                            |  1st tooth 2nd tooth
  //                                                                                            |  th#  amp  th#  amp
  //                                                                                            V  /------\  /------\ .
  v375_build_gear_waveform_vargs(pb, chnl, blknum, num_teeth, dc_baseline, dc_top, pulse_width, 4, 1.0, 5.0, 2.0, 7.5);
  v375_set_clock_source(pb, chnl, chnl);                        // set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        // set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           // set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          // set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           // set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              // set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        // set chnl 0 for freq of 1000 Hz
  v375_write_resets(pb, 0x0);                                   // take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Generate three 64 point waveforms by reading a raw waveform data file.
  // The first wavespec definition sets the first 16 points to 9v, and the rest to 0v.
  // The second wavespec definition sets the first 32 points to 8v, and the rest to 0v.
  // The third wavespec definition sets the first 48 points to 7v, and the rest to 0v.
  //
  // A blank line is used to terminate wavepoint data.  
  // There can be no whitespace on either side of the hyphen.
  //
  // Filename: raw.txt
  // 
  //   # -*- conf -*-
  //   # square wave 25% duty cycle, blocksize = 64, blocknum = 0
  //   wavespec -t raw -bs 64 -bn 0
  //   0-15	9.0
  //   16-63	0.0
  //
  //   # square wave 50% duty cycle, blocksize = 64, blocknum = 1
  //   wavespec -t raw -bs 64 -bn 1
  //   0-31	8.0
  //   32-63	0.0
  //
  //   # square wave 75% duty cycle, blocksize = 64, blocknum = 2
  //   wavespec -t raw -bs 64 -bn 2
  //   0-47	7.0
  //   48-63	0.0

  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "raw.txt", &blksize);              	// load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 1
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 2
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  blknum = 0;
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 0
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 1
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 2
  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Load a 64 point sinewave, where each point has been generated through another method.  This is a demonstration
  // of loading any arbitrary waveform where each point has a defined value.  Single value point ranges can be done
  // using a hyphen to define the point range (as seen in the previous example).
  //
  // Filename: sine_raw.txt
  //
  //     # -*- conf -*-
  //     wavespec -m raw -bs 64 -bn 0
  //     # entry amplitude
  //     0	0.000000
  //     1	3.826834
  //     2	7.071068
  //     3	9.238795
  //     4	10.000000
  //     5	9.238795
  //     6	7.071068
  //     7	3.826834
  //     8	0.000000
  //     9	-3.826834
  //     10	-7.071068
  //     11	-9.238795
  //     12	-10.000000
  //     13	-9.238795
  //     14	-7.071068
  //     15	-3.826834
  //     16	-0.000000
  //     17	3.826834
  //     18	7.071068
  //     19	9.238795
  //     20	10.000000
  //     21	9.238795
  //     22	7.071068
  //     23	3.826834
  //     24	0.000000
  //     25	-3.826834
  //     26	-7.071068
  //     27	-9.238795
  //     28	-10.000000
  //     29	-9.238795
  //     30	-7.071068
  //     31	-3.826834
  //     32	-0.000000
  //     33	3.826834
  //     34	7.071068
  //     35	9.238795
  //     36	10.000000
  //     37	9.238795
  //     38	7.071068
  //     39	3.826834
  //     40	0.000000
  //     41	-3.826834
  //     42	-7.071068
  //     43	-9.238795
  //     44	-10.000000
  //     45	-9.238795
  //     46	-7.071068
  //     47	-3.826834
  //     48	-0.000000
  //     49	3.826834
  //     50	7.071068
  //     51	9.238795
  //     52	10.000000
  //     53	9.238795
  //     54	7.071068
  //     55	3.826834
  //     56	0.000000
  //     57	-3.826834
  //     58	-7.071068
  //     59	-9.238795
  //     60	-10.000000
  //     61	-9.238795
  //     62	-7.071068
  //     63	-3.826834
  
  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 100.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "sine_raw.txt", &blksize);              // load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Generate three 256 point sine waves.  Load the first into blocknumber 0, with an amplitude of 10v (20vp-p) with a phase shift of 0.
  // Load the second into blocknumber 1, with a phase shift of 45 deg.  Load the third into blocknumber 2, with a phase shift of 90 deg.
  // Play blocknumber 0, wait "sleeptime" seconds, play blocknumber 1, wait "sleeptime" seconds, then play blocknumber 3, and wait "sleeptime".
  //
  // Filename: sine.txt
  //
  // # -*- conf -*-
  // # sine wave
  // wavespec -t sine -bs 256 -bn 0 -amp 10.0 -pha 0.0
  // wavespec -t sine -bs 256 -bn 1 -amp 10.0 -pha 45.0
  // wavespec -t sine -bs 256 -bn 2 -amp 10.0 -pha 90.0

  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "sine.txt", &blksize);              	// load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 1
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 2
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Filename: fourier.txt
  //
  // # -*- conf -*-
  // # fourier waveform
  // wavespec -t fourier -bs 256 -bn 0
  // #    amplitude	phase
  // 0	0.0	0.0		# 0th harmonic (DC term offset) amplitude / phase (unused)
  // 1	5.0	0.0		# 1st harmonic (fundamental) amplitude / phase 
  //
  // # fourier wave
  // wavespec -t fourier -bs 256 -bn 1
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	5.0	0.0
  // 2	4.0	0.0
  //
  // # fourier wave
  // wavespec -t fourier -bs 256 -bn 2
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	5.0	0.0
  // 2	4.0	0.0
  // 3	3.0	0.0
  //
  // # fourier wave
  // wavespec -t fourier -bs 256 -bn 3
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	5.0	0.0
  // 2	4.0	0.0
  // 3	3.0	0.0
  // 4	2.0	0.0
  //
  // # fourier wave (rough square)
  // wavespec -t fourier -bs 256 -bn 4
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	6.0	0.0	# fundamental
  // 2	0.0	0.0
  // 3	2.0	0.0	# 3rd harmonic = 1/3 fundamental
  // 4	0.0	0.0
  // 5	1.25	0.0     # 5th harmonic = 1/5 fundamental
  //
  // # fourier wave (rough square)
  // wavespec -t fourier -bs 256 -bn 5
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	6.0	0.0	# fundamental
  // 2	0.0	0.0
  // 3	2.0	0.0	# 3rd harmonic = 1/3 fundamental
  // 4	0.0	0.0
  // 5	1.2	0.0     # 5th harmonic = 1/5 fundamental
  // 6	0.0	0.0
  // 7	0.8571	0.0	# 7th harmonic = 1/7 fundamental
  //
  // # fourier wave (rough square)
  // wavespec -t fourier -bs 256 -bn 6
  // #    amplitude	phase
  // 0	0.0	0.0
  // 1	6.0	0.0	# fundamental
  // 2	0.0	0.0
  // 3	2.0	0.0	# 3rd harmonic = 1/3 fundamental
  // 4	0.0	0.0
  // 5	1.2	0.0     # 5th harmonic = 1/5 fundamental
  // 6	0.0	0.0
  // 7	0.8571	0.0	# 7th harmonic = 1/7 fundamental
  // 8	0.0	0.0
  // 9	0.6667	0.0	# 9th harmonic = 1/9 fundamental

  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 1000.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "fourier.txt", &blksize);              	// load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 1
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 2
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 3
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 4
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 5
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 6
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  blknum = 0;
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 0
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 1
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 2
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 3
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 4
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 5
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, blknum++); // execute from block 6
  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Filename gear.txt
  //
  // # -*- conf -*-
  // wavespec -t gear -bs 1024 -bn 0 -nt 12 -pb 0.0 -ph 10.0
  // #	amplitude	short_tooth_number
  // 0	2.5		0
  // 1	5.0		4
  // 2	7.5		8

  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 100.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "gear.txt", &blksize);              	// load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------
  
  // Filename: pulse.txt
  //
  // # -*- conf -*-
  // # wavespec -t pulse_train -bs blocksize -bn blocknumber -pb pulse_base_volts
  // wavespec -t pulse_train -bs 1024 -bn 0 -pb 0.0
  // # entry  amplitude(+/-10.0)  phase(0-360)  width(0-360)
  //       0          2.5              0           5
  //       1          5.0             90          10
  //       2          7.5            180          15
  //       3         10.0            270          20

  blksize = 0;                  // loaded from v375_load_wavetable()
  blknum = 0;
  freq = 100.0;

  v375_write_resets(pb, chnl_mask);					// hold chnl 0 in reset condition
  v375_load_wavetable(pb, chnl, "pulse.txt", &blksize);              	// load wavefile and blksize
  v375_set_wavetable_size(pb, chnl, blksize);				// define chnl 0 wavetable size = 256
  v375_set_clock_source(pb, chnl, chnl);                        	// set clock source to DDS = 0
  v375_set_chnl_multiplier(pb, chnl, 1.0);		        	// set output amplitude: 10vp-p * multiplier
  v375_set_chnl_phase(pb, chnl, 0.0);                           	// set deg_phase = 0.0
  v375_set_chnl_dc_offset(pb, chnl, 0.0);                          	// set voltage offset = 0.0
  v375_set_chnl_divisor(pb, chnl, 1);                           	// set freq divisor = 1
  v375_set_lowpass_filter(pb, chnl, V375_LOW_PASS_FILTER_OFF);
  v375_set_chnl_summing_mask(pb, chnl, chnl_mask);              	// set analog output summing mask
  v375_set_frequency(pb, chnl, freq, 1);                        	// set chnl 0 for freq of 1000 Hz
  v375_load_waveform_start_location(pb, chnl, blksize, blknum++);	// start from block 0
  v375_write_resets(pb, 0x0);                                   	// take chnl 0 out of reset condition

  sleep(sleeptime);

  v375_write_resets(pb, chnl_mask);                             	// hold chnl 0 in reset condition
  v375_clear_wavetable_memory(pb, chnl);

  // ------------------------------------------------------------------------------------------------------
  //
  // All in one call using wavefile fourier.txt
  //
  
  blksize = 0;
  blknum = 0;
  freq = 1000.0;
  int dds = -1;                     // take default
  int summing_mask = -1;            // take default
  int low_pass_filter = -1;         // take default
  float multiplier = 1.0;
  float phase_shift = 0.0;
  float dc_offset = 0.0;
  int divisor = 1;
  int active_block = 0;

  if (v375_config_chnl_wavefile(pb, chnl, "fourier.txt", dds, summing_mask, low_pass_filter, &freq, &multiplier, &phase_shift, &dc_offset, &divisor, &active_block) == -1)
    {
      printf("ERROR %s/%s: v375_config_chnl_wavefile() failure\n", whoami, __FUNCTION__);
    }

  sleep(sleeptime);
  active_block = 1;
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 1
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 2
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 3
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 4
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 5
  sleep(sleeptime);
  v375_set_new_active_block(pb, chnl, active_block++); // execute from block 6
  sleep(sleeptime);

  // ------------------------------------------------------------------------------------------------------

  if (v375_execute_macro(pb, V375_MACRO_REINITIALIZE, 0, 0, 0) == -1)
    {
      printf("ERROR %s/%s: v375_execute_macro() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  exit(0);
}
