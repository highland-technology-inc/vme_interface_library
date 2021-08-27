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
// Date: Sun Aug 08 14:31:38 2021

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

#include <sys/mman.h>
#include <sys/time.h>

#include <errno.h>
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
#include "v470_utils_proto.h"


int main(int argc, char *argv[])
{
  int i, j;
  V470_REGS *pb;
  char *whoami = argv[0];
  int type;
  float tempr, temp1, temp2;
  float ohms;
  float volts[V470_CHNLS_PER_BOARD / 2] = {-12.0, -8.0, -4.0, -2.0, 2.0, 4.0, 8.0, 12.0};
  float temps[V470_CHNLS_PER_BOARD / 2] = {-200.0, -100.0, 0.0, 100.0, 200.0, 300.0, 400.0, 500.0};
  // unsigned short params[3] = {0, 0, 0};
  
  pb = (V470_REGS *)get_vaddr_for_device("output.conf", "v470[0][0]");

  // if (v470_execute_macro(pb, V470_MACRO_HARD_REBOOT, 0, 0) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_execute_macro() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  printf("vximfr: 0x%hx\n", v470_read_vximfr(pb));
  printf("vxitype: %hd\n", v470_read_vxitype(pb));
  printf("serial: %hd\n", v470_read_serial(pb));
  printf("rom id: %d\n", (int)v470_read_romid(pb));
  printf("rom rev: %c\n", (char)v470_read_romrev(pb));
  printf("cflags: 0x%4.4hx\n", v470_read_cflags(pb));
  printf("rflags: 0x%4.4hx\n", v470_read_rflags(pb));
  printf("relays: 0x%4.4hx\n", v470_read_relays(pb));

  v470_write_uled(pb, 0xaaaa);
             
  printf("uled: 0x%4.4hx\n", v470_read_uled(pb));
  printf("mode: 0x%4.4hx\n", v470_read_mode(pb));
  printf("calid: %hd\n", v470_read_calid(pb));
  printf("calibration date: %s\n", v470_get_cal_date(pb));
  printf("biss: 0x%4.4hd\n", v470_read_biss(pb));

  printf("\n");

  if (v470_write_fake_rtd_temp(pb, '1', 'F', 70.0) == -1)
    {
      printf("ERROR %s/%s: v470_write_fake_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v470_write_fake_rtd_temp(pb, '2', 'F', 100.0) == -1)
    {
      printf("ERROR %s/%s: v470_write_fake_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v470_read_rtd_temp(pb, '1', 'F', &temp1) == -1)
    {
      printf("ERROR %s/%s: v470_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp fake1 = %f degF\n", temp1);
    
  if (v470_read_rtd_temp(pb, '2', 'F', &temp2) == -1)
    {
      printf("ERROR %s/%s: v470_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp fake2 = %f degF\n", temp2);
    
  if (v470_read_rtd_temp(pb, 'r', 'F', &tempr) == -1)
    {
      printf("ERROR %s/%s: v470_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp onboard ref = %f degF\n", tempr);
    
  if (v470_read_rtd_ohms(pb, 't', &ohms) == -1)
    {
      printf("ERROR %s/%s: v470_read_rtd_ohms() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("test resister = %f ohms\n", ohms);

// #define V470_RN_CODE_OFF		0
// #define V470_RN_CODE_25_MV		1
// #define V470_RN_CODE_50_MV		2
// #define V470_RN_CODE_80_MV		3
// #define V470_RN_CODE_125_MV		4
// #define V470_RN_CODE_250_MV		5
// #define V470_RN_CODE_500_MV		6
// #define V470_RN_CODE_1_25_V		7
// #define V470_RN_CODE_2_5_V		8
// #define V470_RN_CODE_5_V		9
// #define V470_RN_CODE_12_5_V		10
// #define V470_RN_CODE_25_V		11
// #define V470_RN_CODE_50_V		12
// #define V470_RN_CODE_125_V		13
// #define V470_RN_CODE_250_V		14

  for (i = 0; i < V470_CHNLS_PER_BOARD / 2; i++)
    {
      v470_config_analog_chnl(pb, i, V470_RN_CODE_12_5_V);
    }

  for (i = 0; i < V470_CHNLS_PER_BOARD / 2; i++)
    {
      v470_write_analog_chnl(pb, i, volts[i]);
      printf("chnl: %d, volts: %f\n", i, volts[i]);
    }

// #define V470_RN_CODE_TYPE_J		16 	// -210 to 1200 degC
// #define V470_RN_CODE_TYPE_K		17 	// -270 to 1372 degC
// #define V470_RN_CODE_TYPE_E		18 	// -270 to 1000 degC
// #define V470_RN_CODE_TYPE_T		19 	// -270 to 400 degC
// #define V470_RN_CODE_TYPE_R		20      // -50 to 1768 degC
// #define V470_RN_CODE_TYPE_S		21      // -50 to 1768 degC
// #define V470_RN_CODE_TYPE_B		22      // 0 to 1820 degC
// #define V470_RN_CODE_TYPE_N		23      // -270 to 1300 degC

// #define V470_RS_CODE_SHIFT		8
// #define V470_RS_CODE_MASK		0x0700
// #define V470_RS_CODE_RTD_A		0
// #define V470_RS_CODE_RTD_B		1
// #define V470_RS_CODE_RTD_C		2
// #define V470_RS_CODE_RTD_D		3
// #define V470_RS_CODE_RTD_INTERNAL	4
// #define V470_RS_CODE_RTD_FAKE1	5
// #define V470_RS_CODE_RTD_FAKE2	6
// #define V470_RS_CODE_RTD_NONE	7

  for (i = V470_CHNLS_PER_BOARD / 2; i < V470_CHNLS_PER_BOARD; i++)
    {
      v470_config_tc_chnl(pb, i, V470_RS_CODE_RTD_C, V470_RN_CODE_TYPE_K);
    }

  for (i = V470_CHNLS_PER_BOARD / 2, j = 0; i < V470_CHNLS_PER_BOARD; i++, j++)
    {
      v470_write_tc_chnl(pb, i, 'C', temps[j]);
      printf("chnl: %d, temp: %f\n", i, temps[j]);
    }


  // if (v470_config_rtd_type(pb, 'a', V470_RTD_TYPE_100_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  // if (v470_config_rtd_type(pb, 'b', V470_RTD_TYPE_100_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  if (v470_config_rtd_type(pb, 'c', V470_RTD_TYPE_100_OHM_PT) == -1)
    {
      printf("ERROR %s/%s: v470_config_rtd_type() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // if (v470_config_rtd_type(pb, 'd', V470_RTD_TYPE_1000_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  // if (v470_read_rtd_type(pb, 'a', &type) == 0)
  //   printf("INFO %s/%s: v470_read_rte_type 'a' = %d\n", whoami, __FUNCTION__, type);

  // if (v470_read_rtd_type(pb, 'b', &type) == 0)
  //   printf("INFO %s/%s: v470_read_rte_type 'b' = %d\n", whoami, __FUNCTION__, type);

  if (v470_read_rtd_type(pb, 'c', &type) == 0)
    printf("INFO %s/%s: v470_read_rte_type 'c' = %d\n", whoami, __FUNCTION__, type);

  // if (v470_read_rtd_type(pb, 'd', &type) == 0)
  //   printf("INFO %s/%s: v470_read_rte_type 'd' = %d\n", whoami, __FUNCTION__, type);

  // params[0] = 1;
  
  // if (v470_execute_macro(pb, V470_MACRO_BIST_ONE_CHNL, params, 0) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_execute_macro(V470_MACRO_BIST_ONE_CHNL) failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }
  // else
  //   printf("INFO %s/%s: v470_execute_macro(V450_MACRO_BIST_ONE_CHNL=1) completed\n",whoami, __FUNCTION__);
  
           
  // if (v470_execute_macro(pb, V470_MACRO_BIST_ALL, 0, 0) == -1)
  //   {
  //     printf("ERROR %s/%s: v470_execute_macro(V470_MACRO_BIST_ALL) failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }
  // else
  //   printf("INFO %s/%s: v470_execute_macro(V450_MACRO_BIST_ALL) completed\n",whoami, __FUNCTION__);

  printf("cflags: 0x%4.4hx\n", v470_read_cflags(pb));
  printf("rflags: 0x%4.4hx\n", v470_read_rflags(pb));

  exit(0);
}
