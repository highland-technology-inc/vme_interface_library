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
// Date: Thu Aug 19 16:02:00 2021

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

// usage:
//   ./v545_synchro -m min_position -M max_position -u usec_pause -1 -d -r
// defaults:
//   usec_pause = 200000 (0.2 sec)
//   excitation_freq = 400.
//   debug_flag = on
//   reset_flag = off

// This program is designed to demonstrate a synchro input into P2 for real synchro read.
// The input uses function block 0 coil 20 for excitation R1:R2, coil 21 for V(S3:S1), coil 22 for V(S2:S3), and coil 23 for V(S1:S2) on the P2 connector.

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
#include "v545_utils_proto.h"

#define SIGNED_MAX_DEG 180.0*32767.0/32768.0 		// 179.9945
#define SIGNED_MIN_DEG -180.0
#define UNSIGNED_MAX_DEG 360.0*65535.0/65536.0		// 359.9945
#define UNSIGNED_MIN_DEG 0.0

#define SYNCHRO_INPUT_FB_CHNL 0	// define function block of 0


int main(int argc, char *argv[])
{
  V545_REGS *pb;
  int i;
  char *whoami = argv[0];
  float min_position = 0;
  float max_position = 0;
  int signed_flag = 0;
  int counts = 0;
  int min_counts = 0;
  int max_counts = 0;
  float read_position;
  int usec_pause = 200000;
  int debug_flag = 1;
  int reset_flag = 0;
  float measured_secondary_voltage;
  char c;
  char usage[] = "Usage: ./v545_real_synchro -m min_position -M max_position -c min_counts -C max_counts -u usec_pause -1 -d -r -s signed_flag\n";
  int loop_flag = 1;
  char ans[8];
  
  while ((c = getopt(argc, argv, "m:M:c:C:u:dr\?")) != (char)EOF)
    switch(c)
      {
      case 'm':
        if (sscanf(optarg, "%f", &min_position) != 1)
          goto contin;
        break;

      case 'M':
        if (sscanf(optarg, "%f", &max_position) != 1)
          goto contin;
        break;

      case 'c':
        if (sscanf(optarg, "%d", &min_counts) != 1)
          goto contin;
        break;

      case 'C':
        if (sscanf(optarg, "%d", &max_counts) != 1)
          goto contin;
        break;

      case 'u':
        if (sscanf(optarg, "%d", &usec_pause) != 1)
          goto contin;
        break;

      case 'd':
        debug_flag = 1;
        break;
        
      case 'r':
        reset_flag = 1;
        break;

      case '\?':
        printf("%s", usage);
        exit(1);

      default:
      contin:
        printf("ERROR %s/%s: conversion error for '-%c %s' option/arg\n", whoami, __FUNCTION__, c, optarg);
        exit(1);
      }

  // check options

  if (min_position >= max_position)
    {
      printf("ERROR %s/%s: -m min_position must be less than -M Max_position\n", whoami, __FUNCTION__);
      exit(1);
    }

  // map to board

  pb = (V545_REGS *)get_vaddr_for_device("output.conf", "v545[0][0]");

  if (reset_flag)
    {
      if (v545_execute_macro(pb, V545_MACRO_RESET, 0, 0) == -1)
        {
          printf("ERROR %s/%s: v545_execute_macro() failure\n", whoami, __FUNCTION__);
          exit(1);
        }
    }
  
  if (debug_flag)
    {
      printf("vximfr: 0x%hx\n", v545_read_vximfr(pb));
      printf("vxitype: %hd\n", v545_read_vxitype(pb));
      printf("serial: %hd\n", v545_read_serial(pb));
      printf("rom id: %d\n", (int)v545_read_romid(pb));
      printf("rom rev: %c\n", (char)v545_read_romrev(pb));
      printf("dash: 0x%4.4hx\n", v545_read_dash(pb));
      printf("state: 0x%4.4hx\n", v545_read_state(pb));

      v545_write_uled(pb, 0xcccc);
             
      printf("uled: 0x%4.4hx\n", v545_read_uled(pb));
      printf("biss: 0x%4.4hx\n", v545_read_biss(pb));
      printf("calid: %hd\n", v545_read_calid(pb));
      printf("calibration date: %s\n", v545_get_cal_date(pb));
      printf("%s\n", v545_sprintf_checksum_flash(pb));

      printf("transformers:\n");
      printf("  ");
      for (i = 0; i < V545_CCB_CHNLS_PER_BOARD / 2; i++)
        printf("%s[%2d] ", v545_read_xfmr_type(pb, i), i);
      printf("\n");
      printf("  ");
      for (i = V545_CCB_CHNLS_PER_BOARD / 2; i < V545_CCB_CHNLS_PER_BOARD; i++)
        printf("%s[%2d] ", v545_read_xfmr_type(pb, i), i);
      printf("\n");
    }

  // set signed_flag as a f(min_position, max_position)

  if ((min_position < 0) || (max_position < 0))
    signed_flag = 1;

  //                                       fb_chnl
  //                                        |             EX_coil
  //                                        |              |  A_coil
  //                                        |              |   |  B_coil
  //                                        |              |   |   |  C_coil
  //                                        |              |   |   |   |
  //                                        V              V   V   V   V
  if (v545_config_synchro_input(pb, SYNCHRO_INPUT_FB_CHNL, 20, 21, 22, 23, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_config_synchro_input() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // set to min position and read counts

  if ((min_counts == 0) && (max_counts == 0))
    {
      printf("Set to min position of %f degrees? [y/n] \n", min_position);
      scanf("%1s", ans);

      if (v545_read_synchro_resolver_position_counts(pb, SYNCHRO_INPUT_FB_CHNL, &min_counts, signed_flag) == -1)
        {
          printf("ERROR %s/%s: v545_read_synchro_resolver_position_counts() failure\n", whoami, __FUNCTION__);
          exit(1);
        }

      printf("min_position = %f, min_counts = %d\n", min_position, min_counts);
      
      printf("Set to max position of %f degrees? [y/n] \n", max_position);
      scanf("%1s", ans);

      if (v545_read_synchro_resolver_position_counts(pb, SYNCHRO_INPUT_FB_CHNL, &max_counts, signed_flag) == -1)
        {
          printf("ERROR %s/%s: v545_read_synchro_resolver_position_counts() failure\n", whoami, __FUNCTION__);
          exit(1);
        }

      printf("max_position = %f, max_counts = %d\n", max_position, max_counts);
    }

  // Sanity check -- check to see if min or max values are too close to count crossover points (179/-180 for signed range, and 0/359 for unsigned range).

  printf("INFO %s/%s: min_position = %f, min_counts = %d, max_position = %f, max_counts = %d\n", whoami, __FUNCTION__, min_position, min_counts, max_position, max_counts);

  if (min_counts > max_counts)
    {
      printf("ERROR %s/%s: min or max points too close to count value transition points (179/-180 for signed range, and 0/359 for unsigned range)\n", whoami, __FUNCTION__);
      exit(1);
    }

  // read synchro input

  while (loop_flag)
    {
      if (v545_read_synchro_resolver_position_counts(pb, SYNCHRO_INPUT_FB_CHNL, &counts, signed_flag) == -1)
        {
          printf("ERROR %s/%s: v545_read_synchro_resolver_position_counts() failure\n", whoami, __FUNCTION__);
          exit(1);
        }

      if (v545_read_synchro_resolver(pb, SYNCHRO_INPUT_FB_CHNL, min_counts, max_counts, min_position, max_position, &read_position, &measured_secondary_voltage, signed_flag) == -1)
        {
          printf("ERROR %s/%s: v545_read_synchro_resolver() failures\n", whoami, __FUNCTION__);
          exit(1);
        }

      printf("read_position = %f, counts = %d, measured_secondary_voltage = %f\n", read_position, counts, measured_secondary_voltage);
      
      usleep(usec_pause);
    }

  // clear function block

  if (v545_clear_function_block(pb, SYNCHRO_INPUT_FB_CHNL) == -1)
    {
      printf("ERROR %s/%s: error calling v545_clear_function_block()\n", whoami, __FUNCTION__);
      exit(1);
    }

  exit(0);
}
