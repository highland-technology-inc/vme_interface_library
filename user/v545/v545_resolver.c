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
// Date: Wed Aug 18 15:58:10 2021

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

// usage:
//   ./v545_resolver -m min_position -M max_position -D delta_position -u usec_pause -f excitation_freq -1 -d -r -P target_position -V target_velocity -S sbits
// defaults:
//   min_position = -180.0
//   max_position = 179.9945
//   usec_pause = 200000 (0.2 sec)
//   excitation_freq = 400.
//   debug_flag = on
//   reset_flag = off
//   one_pass_flag = off

// This program is designed to demonstrate a resolver output (using the DDS for excitation) going out P1, looped back into P2 for resolver read.
// The output uses function block 0 with coil 0 for excitation R1:R2, coil 1 for X_coil cosine V(S4:S2), coil 2 for Y_coil sine V(S1:S3) on the P1 connector.
// The input uses function block 1 coil 12 for excitation R1:R2, coil 13 for X_coil cosine V(S4:S2), coil 14 for Y_coil sine V(S1:S3) on the P2 connector.
// A straight through DB25 connector, connects P1 to P2.
//
// Additionally, an override condition may be tested using a software trigger to drive override block 12.
// -P argument specifies the target position, and -V argument specifies the target velocity.
//
// Note: if you do not have the correct V548-6 transformers, you will need to compile lib/v545_utils.c with EXCITATION_CHECK set to 0 in the C code.


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

#define RESOLVER_OUTPUT_FB_CHNL 0
#define RESOLVER_INPUT_FB_CHNL 1
#define OVERRIDE_BLOCK_CHNL 12
#define DDS_CHNL 0

#define SIGNED_MAX_DEG 180.0*32767.0/32768.0 		// 179.9945
#define SIGNED_MIN_DEG -180.0
#define UNSIGNED_MAX_DEG 360.0*65535.0/65536.0		// 359.9945
#define UNSIGNED_MIN_DEG 0.0


int main(int argc, char *argv[])
{
  V545_REGS *pb;
  int i;
  char *whoami = argv[0];
  float min_position = -90;
  float max_position = 90;
  int signed_flag = 0;
  int counts = 0;
  int min_counts = 0;
  int max_counts = 0;
  float read_position;
  float write_position;
  int usec_pause = 200000;
  float excitation_freq = 400.0;
  int debug_flag = 1;
  int reset_flag = 0;
  float delta_position = 10.0;
  float measured_secondary_voltage;
  char c;
  char usage[] = "Usage: ./v545_resolver -m min_position -M max_position -D delta_position -u usec_pause -f excitation_freq -1 -d -r -P target_position -V target_velocity -S sbits\n";
  int one_pass_flag = 0;
  int loop_flag = 1;
  char ans[8];
  float target_position = 45.0;
  float target_velocity = 22.5;  // 22.5 deg/sec
  int sbits = 0;                 // 1 msec = smallest time interval
  struct timespec start_ts, stop_ts;
  double delta_time;

  while ((c = getopt(argc, argv, "m:M:D:u:f:1drP:V:S:\?")) != (char)EOF)
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

      case 'D':
        if (sscanf(optarg, "%f", &delta_position) != 1)
          goto contin;
        break;
        
      case 'u':
        if (sscanf(optarg, "%d", &usec_pause) != 1)
          goto contin;
        break;

      case 'f':
        if (sscanf(optarg, "%f", &excitation_freq) != 1)
          goto contin;
        break;

      case '1':
        one_pass_flag = 1;
        break;

      case 'd':
        debug_flag = 1;
        break;
        
      case 'r':
        reset_flag = 1;
        break;

      case 'P':
        if (sscanf(optarg, "%f", &target_position) != 1)
          goto contin;
        break;

      case 'V':
        if (sscanf(optarg, "%f", &target_velocity) != 1)
          goto contin;

        break;

      case 'S':
        if (sscanf(optarg, "%d", &sbits) != 1)
          goto contin;

        // check sbits are 0, 1, or 2

        switch(sbits)
          {
          case V545_TIME_INT_1MS:
          case V545_TIME_INT_10MS:
          case V545_TIME_INT_100MS:
            break;
          default:
            printf("ERROR %s/%s: SBITS can on be 0, 1, 2\n", whoami, __FUNCTION__);
            exit(1);
          }
        break;

      case '\?':
        printf("%s", usage);
        exit(1);

      default:
      contin:
        printf("ERROR %s/%s: conversion error for '-%c %s' option/arg\n", whoami, __FUNCTION__, c, optarg);
        exit(1);
      }

  // set signed flag

  if ((min_position < 0) || (max_position < 0))
    signed_flag = 1;

  // check options

  if (min_position >= max_position)
    {
      printf("ERROR %s/%s: -m min_position must be less than -M Max_position\n", whoami, __FUNCTION__);
      exit(1);
    }

  // map to board

  pb = (V545_REGS *)get_vaddr_for_device("output.conf", "v545[0][0]");

  // check to reset board

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
  
  //                                          fb_chnl
  //                                           |                dds_chnl
  //                                           |                 |         dds_freq
  //                                           |                 |          |           EX_coil
  //                                           |                 |          |            | X_coil
  //                                           |                 |          |            |  | Y_coil
  //                                           |                 |          |            |  |  | X_inv_flg
  //                                           |                 |          |            |  |  |  | Y_inv_flg
  //                                           |                 |          |            |  |  |  |  |  
  //                                           |                 |          |            |  |  |  |  |  
  //                                           |                 |          |            |  |  |  |  |  
  //                                           |                 |          |            |  |  |  |  |     min_pos
  //                                           |                 |          |            |  |  |  |  |       |           max_pos
  //                                           |                 |          |            |  |  |  |  |       |             |           init_pos
  //                                           |                 |          |            |  |  |  |  |       |             |             |
  //                                           V                 V          V            V  V  V  V  V       V             V             V
  if (v545_config_resolver_output(pb, RESOLVER_OUTPUT_FB_CHNL, DDS_CHNL, excitation_freq, 0, 1, 2, 0, 0, min_position, max_position, min_position, sbits, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_config_resolver_output() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  //                                fb_chnl
  //                                 | EX_coil
  //                                 |  |  X_coil
  //                                 |  |   |  Y_coil
  //                                 |  |   |   | 
  //                                 V  V   V   V 
  if (v545_config_resolver_input(pb, 1, 12, 13, 14, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_config_resolver_input() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // setup min counts on resolver read

  write_position = min_position;
  
  if (v545_write_synchro_resolver(pb, RESOLVER_OUTPUT_FB_CHNL, min_position, max_position, write_position, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_write_synchro_resolver() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
          
  usleep(200000);               // sleep for 200 msec

  v545_read_synchro_resolver_position_counts(pb, RESOLVER_INPUT_FB_CHNL, &min_counts, signed_flag);

  // setup max counts on resolver read

  write_position = max_position;
  
  if (v545_write_synchro_resolver(pb, RESOLVER_OUTPUT_FB_CHNL, min_position, max_position, write_position, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_write_synchro_resolver() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
          
  usleep(200000);               // sleep for 200 msec

  v545_read_synchro_resolver_position_counts(pb, RESOLVER_INPUT_FB_CHNL, &max_counts, signed_flag);

  // Sanity check -- check to see if min or max values are too close to count crossover points (179/-180 for signed range, and 0/359 for unsigned range).
  // For example -- for signed range:
  //   ./v545_resolver -m -180 -M 179 -P 90 -V 45 -d -1
  //      min_position = -180.000000, min_counts = 32764, max_position = 179.000000, max_counts = 32582
  //   fix with:
  //   ./v545_resolver -m -179 -M 179 -P 90 -V 45 -d -1
  //      min_position = -179.000000, min_counts = -32589, max_position = 179.000000, max_counts = 32582
  // For example -- for unsigned range:
  //   ./v545_resolver -m 0 -M 270 -P 90 -V 45 -d -1
  //      min_position = 0.000000, min_counts = 65535, max_position = 270.000000, max_counts = 49157
  //   fix with:
  //   ./v545_resolver -m 1 -M 270 -P 90 -V 45 -d -1
  //      min_position = 1.000000, min_counts = 179, max_position = 270.000000, max_counts = 49157

  printf("INFO %s/%s: min_position = %f, min_counts = %d, max_position = %f, max_counts = %d\n", whoami, __FUNCTION__, min_position, min_counts, max_position, max_counts);

  if (min_counts > max_counts)
    {
      printf("ERROR %s/%s: min or max points too close to count value transition points (179/-180 for signed range, and 0/359 for unsigned range)\n", whoami, __FUNCTION__);
      exit(1);
    }

  // setup software trigger for override function for override block 12 for function block 0, set at 45 degrees at 1 degree per second (with no min/max counts)

  // if (v545_config_override_switch(pb, 12, 1) == -1)
  //   {
  //     printf("ERROR %s/%s: v545_config_override_switch() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }
    
  if (v545_config_override_swtrigger(pb, 12) == -1)
    {
      printf("ERROR %s/%s: v545_config_override_swtrigger() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v545_config_override_position_velocity(pb, 12, 0, target_position, target_velocity, 0, 0, min_position, max_position, signed_flag) == -1)
    {
      printf("ERROR %s/%s: v545_set_override_position_velocity() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // sweep from min -> max -> min -> ...

  while (loop_flag)
    {
      write_position = min_position;
      while (write_position < max_position)
        {
          v545_write_synchro_resolver(pb, RESOLVER_OUTPUT_FB_CHNL, min_position, max_position, write_position, signed_flag);
          
          usleep(usec_pause);

          v545_read_synchro_resolver(pb, RESOLVER_INPUT_FB_CHNL, min_counts, max_counts, min_position, max_position, &read_position, &measured_secondary_voltage, signed_flag);
          v545_read_synchro_resolver_position_counts(pb, RESOLVER_INPUT_FB_CHNL, &counts, signed_flag);

          if (debug_flag)
            printf("write_position = %f; read_position = %f, counts = %d, delta = %f, measured_secondary_voltage = %f\n", write_position, read_position, counts, write_position - read_position, measured_secondary_voltage);

          write_position += delta_position;
        }

      write_position = max_position;
      while (write_position > min_position)
        {
          v545_write_synchro_resolver(pb, RESOLVER_OUTPUT_FB_CHNL, min_position, max_position, write_position, signed_flag);
          
          usleep(usec_pause);

          v545_read_synchro_resolver(pb, RESOLVER_INPUT_FB_CHNL, min_counts, max_counts, min_position, max_position, &read_position, &measured_secondary_voltage, signed_flag);
          v545_read_synchro_resolver_position_counts(pb, RESOLVER_INPUT_FB_CHNL, &counts, signed_flag);

          if (debug_flag)
            printf("write_position = %f; read_position = %f, counts = %d, delta = %f, measured_secondary_voltage = %f\n", write_position, read_position, counts, write_position - read_position, measured_secondary_voltage);

          write_position -= delta_position;
        }

      if (one_pass_flag)
        loop_flag = 0;
    }


  printf("run override block s/w trigger? [y/n] \n");
  scanf("%1s", ans);

  if (strcmp(ans, "y") == 0)
    {
      clock_gettime(CLOCK_MONOTONIC, (struct timespec *)&start_ts);
      if (v545_set_override_swtrigger(pb, 12) == -1)
        {
          printf("ERROR %s/%s: v545_set_override_swtrigger() failure\n", whoami, __FUNCTION__);
          exit(1);
        }

      // loop while read position is not within +/-1 degree of target position

      while ((read_position < target_position - 1.0) || (read_position > target_position + 1.0))
        {
          v545_read_synchro_resolver(pb, RESOLVER_INPUT_FB_CHNL, min_counts, max_counts, min_position, max_position, &read_position, &measured_secondary_voltage, signed_flag);
          v545_read_synchro_resolver_position_counts(pb, RESOLVER_INPUT_FB_CHNL, &counts, signed_flag);

          if (debug_flag)
            printf("write_position = %f; read_position = %f, counts = %d, delta = %f, measured_secondary_voltage = %f\n", write_position, read_position, counts, write_position - read_position, measured_secondary_voltage);

          usleep(usec_pause);
        }

      clock_gettime(CLOCK_MONOTONIC, (struct timespec *)&stop_ts);
      delta_time = ((double)stop_ts.tv_sec + (double)stop_ts.tv_nsec / 1000000000.0L) - ((double)start_ts.tv_sec + (double)start_ts.tv_nsec / 1000000000.0L);
      printf("time to slew to target position = %lf seconds\n", delta_time);
    }

  if (debug_flag)
    printf("write_position = %f; read_position = %f, delta = %f, measured_secondary_voltage = %f\n", write_position, read_position, write_position - read_position, measured_secondary_voltage);

  // clear regfile

  v545_clear_regfile(pb);

  // clear override block

  if (v545_clear_override_block(pb, OVERRIDE_BLOCK_CHNL) == -1)
    {
      printf("ERROR %s/%s: error calling v545_clear_override_block()\n", whoami, __FUNCTION__);
      exit(1);
    }

  // clear function blocks 

  if (v545_clear_function_block(pb, RESOLVER_OUTPUT_FB_CHNL) == -1)
    {
      printf("ERROR %s/%s: error calling v545_clear_function_block()\n", whoami, __FUNCTION__);
      exit(1);
    }

  if (v545_clear_function_block(pb, RESOLVER_INPUT_FB_CHNL) == -1)
    {
      printf("ERROR %s/%s: error calling v545_clear_function_block()\n", whoami, __FUNCTION__);
      exit(1);
    }

  exit(0);
}
