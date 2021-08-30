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
// Date: Sun Aug 29 11:20:58 2021

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

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

#include "get_vaddr_for_device_proto.h"
#include "ht_utils_proto.h"
#include "v545_utils_proto.h"

void *update(void *);

pthread_t thread;
int thread_active = 0;
int percent_complete = 0;

int main(int argc, char *argv[])
{
  V545_REGS *pb;
  char *whoami = argv[0];
  int debug_flag = 1;
  int reset_flag = 0;
  int info_only_flag = 0;
  int verify_flag = 0;
  char c;
  char usage[] = "Usage: ./v545_reflash -F flash_filename -d -r\n";
  char flash_filename[256];
  int i;

  while ((c = getopt(argc, argv, "F:driv\?")) != (char)EOF)
    switch(c)
      {
      case 'F':
        if (sscanf(optarg, "%s", flash_filename) != 1)
          goto contin;
        break;

      case 'd':
        debug_flag = 1;
        break;
        
      case 'r':
        reset_flag = 1;
        break;

      case 'i':
        info_only_flag = 1;
        break;

      case 'v':
        verify_flag = 1;
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

  if ((pb = (V545_REGS *)get_vaddr_for_device("output.conf", "v545[0][0]")) == (V545_REGS *)0)
    {
      printf("ERROR %s/%s: get_vaddr_for_device() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // is reset flag set?

  if (reset_flag)
    {
      if (v545_execute_macro(pb, V545_MACRO_RESET, 0, 0) == -1)
        {
          printf("ERROR %s/%s: v545_execute_macro() failure\n", whoami, __FUNCTION__);
          exit(1);
        }
    }
  
  // is debug flag set?

  if ((debug_flag) || (info_only_flag))
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
  
  if (info_only_flag != 0)
    exit(0);

  v545_clear_function_blocks(pb);
  v545_clear_override_blocks(pb);

  // create thread

  if (pthread_create(&thread, NULL, update, (void *)0) != 0)
    {
      printf("ERROR %s/%s: cannot create thread for update()\n", whoami, __FUNCTION__);
      exit(1);
    }

  thread_active = 1;

  // load flash

  if (v545_load_flash(pb, flash_filename, &percent_complete) == -1)
    {
      printf("ERROR %s/%s: v545_load_flash() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  printf("INFO %s/%s: %s\n", whoami, __FUNCTION__,  v545_sprintf_checksum_flash(pb));
  printf("INFO %s/%s: romrev = %c\n", whoami, __FUNCTION__,  v545_read_romrev(pb));

  // verify flash

  if (verify_flag)
    {
      if (v545_verify_flash(pb, flash_filename, &percent_complete) == -1)
        {
          printf("ERROR %s/%s: v545_verify_flash() failure\n", whoami, __FUNCTION__);
          exit(1);
        }
      else
        printf("INFO %s/%s: flash verified\n", whoami, __FUNCTION__);
    }
  
  thread_active = 0;

  pthread_cancel(thread);
  pthread_join(thread, NULL);

  exit(0);
}


// update thread

void *update(void *arg)
{
  printf("\n");

  while (thread_active == 1)
    {
      printf("percent_complete = %d\r", percent_complete);
      fflush(stdout);
      sleep(1);
    }

  printf("\n");
  return((void *)0);
}
