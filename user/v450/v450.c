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
// Date: Sun Aug 08 14:30:59 2021

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
#include "v450_utils_proto.h"


int main(int argc, char *argv[])
{
  int i;
  V450_REGS *pb;
  char *whoami = argv[0];
  int type;
  float tempr, temp1, temp2;
  float ohms;
  float volts[V450_CHNLS_PER_BOARD];
  int counts[V450_CHNLS_PER_BOARD];
  float temps[V450_CHNLS_PER_BOARD];
  unsigned short analog_filters[V450_CHNLS_PER_BOARD];
  unsigned short digital_filters[V450_CHNLS_PER_BOARD] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 11, 12, 13, 14, 15, 16, 17};
  unsigned short read_digital_filters[V450_CHNLS_PER_BOARD] = {V450_CHNLS_PER_BOARD * 0};
  
  pb = (V450_REGS *)get_vaddr_for_device("output.conf", "v450[0][0]");

  // if (v450_execute_macro(pb, V450_MACRO_SOFT_REBOOT, 0, 0) == -1)
  //   {
  //     printf("ERROR %s/%s: v450_execute_macro() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  // sleep(5);

  printf("vximfr: 0x%hx\n", v450_read_vximfr(pb));
  printf("vxitype: %hd\n", v450_read_vxitype(pb));
  printf("serial: %hd\n", v450_read_serial(pb));
  printf("rom id: %d\n", (int)v450_read_romid(pb));
  printf("rom rev: %c\n", (char)v450_read_romrev(pb));
  printf("dfilt: 0x%4.4hx\n", v450_read_dfilt(pb));
  printf("cflags: 0x%4.4hx\n", v450_read_cflags(pb));
  printf("rflags: 0x%4.4hx\n", v450_read_rflags(pb));
  printf("relays: 0x%4.4hx\n", v450_read_relays(pb));

  v450_write_uled(pb, 0x51cd);
             
  printf("uled: 0x%4.4hx\n", v450_read_uled(pb));
  printf("mode: 0x%4.4hx\n", v450_read_mode(pb));
  printf("calid: %hd\n", v450_read_calid(pb));
  printf("calibration date: %s\n", v450_get_cal_date(pb));
  printf("biss: 0x%4.4hd\n", v450_read_biss(pb));

  printf("\n");

  if (v450_write_fake_rtd_temp(pb, '1', 'F', 70.0) == -1)
    {
      printf("ERROR %s/%s: v450_write_fake_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v450_write_fake_rtd_temp(pb, '2', 'F', 100.0) == -1)
    {
      printf("ERROR %s/%s: v450_write_fake_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v450_read_rtd_temp(pb, '1', 'F', &temp1) == -1)
    {
      printf("ERROR %s/%s: v450_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp fake1 = %f degF\n", temp1);
    
  if (v450_read_rtd_temp(pb, '2', 'F', &temp2) == -1)
    {
      printf("ERROR %s/%s: v450_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp fake2 = %f degF\n", temp2);
    
  if (v450_read_rtd_temp(pb, 'r', 'F', &tempr) == -1)
    {
      printf("ERROR %s/%s: v450_read_rtd_temp() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("temp onboard ref = %f degF\n", tempr);
    
  if (v450_read_rtd_ohms(pb, 't', &ohms) == -1)
    {
      printf("ERROR %s/%s: v450_read_rtd_ohms() failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    printf("test resister = %f ohms\n", ohms);

  if (v450_execute_macro(pb, V450_MACRO_GET_AFILT, 0xffff, &analog_filters[0]) == -1)
    {
      printf("ERROR %s/%s: v450_execute_macro(V450_MACRO_GET_AFILT) failure\n", whoami, __FUNCTION__);
      exit(1);
    }
  else
    {
      for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
        printf("afilt[%d] = %d\n", i, analog_filters[i]);
    }

// #define V450_DF_NONE			0	// digital filter disabled
// #define V450_DF_1_HZ_BW		1       // 1 Hz 16-pole Butterworth
// #define V450_DF_2_HZ_BW		2	// 2 Hz 16-pole Butterworth
// #define V450_DF_5_HZ_BW		3	// 5 Hz 16-pole Butterworth
// #define V450_DF_10_HZ_BW		4       // 10 Hz 16-pole Butterworth
// #define V450_DF_20_HZ_BW		5	// 20 Hz 16-pole Butterworth
// #define V450_DF_50_HZ_BW		6	// 50 Hz 16-pole Butterworth
// #define V450_DF_100_HZ_BW		7	// 100 Hz 16-pole Butterworth
// #define V450_DF_1_HZ_BS		11	// 1 Hz 16-pole Bessel
// #define V450_DF_2_HZ_BS		12	// 2 Hz 16-pole Bessel
// #define V450_DF_5_HZ_BS		13	// 5 Hz 16-pole Bessel
// #define V450_DF_10_HZ_BS		14	// 10 Hz 16-pole Bessel
// #define V450_DF_20_HZ_BS		15	// 20 Hz 16-pole Bessel
// #define V450_DF_50_HZ_BS		16	// 50 Hz 16-pole Bessel
// #define V450_DF_100_HZ_BS		17	// 100 Hz 16-pole Bessel

  // turn off all digital filtering

  for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
    digital_filters[i] = V450_DF_NONE;

  if (v450_execute_macro(pb, V450_MACRO_SET_DFILT, 0, &digital_filters[0]) == -1)
    {
      printf("ERROR %s/%s: v450_execute_macro(V450_MACRO_SET_DFILT) failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  if (v450_execute_macro(pb, V450_MACRO_GET_DFILT, 0, &read_digital_filters[0]) == -1)
    {
      printf("ERROR %s/%s: v450_execute_macro(V450_MACRO_GET_DFILT) failure\n", whoami, __FUNCTION__);
      exit(1);
    }
    
  for (i = 0; i < V450_CHNLS_PER_BOARD; i++)
    {
      printf("digital_filters[%d] = %d\n", i, digital_filters[i]);
      printf("read_digital_filters[%d] = %d\n", i, read_digital_filters[i]);
    }

// #define V450_RF_CODE_16_7_SPS	0	// 16.7 samples per second
// #define V450_RF_CODE_4_17_SPS	1	// 4.17 samples per second
// #define V450_RF_CODE_8_33_SPS	2	// 8.33 samples per second
// #define V450_RF_CODE_33_3_SPS	3       // 33.3 samples per second
// #define V450_RF_CODE_62_5_SPS	4	// 62.5 samples per second
// #define V450_RF_CODE_125_SPS		5	// 125 samples per second
// #define V450_RF_CODE_250_SPS		6       // 250 samples per second
// #define V450_RF_CODE_500_SPS		7	// 500 samples per second

// #define V450_RN_CODE_OFF		0
// #define V450_RN_CODE_25_MV		1
// #define V450_RN_CODE_50_MV		2
// #define V450_RN_CODE_80_MV		3
// #define V450_RN_CODE_125_MV		4
// #define V450_RN_CODE_250_MV		5
// #define V450_RN_CODE_500_MV		6
// #define V450_RN_CODE_1_25_V		7
// #define V450_RN_CODE_2_5_V		8
// #define V450_RN_CODE_5_V		9
// #define V450_RN_CODE_12_5_V		10
// #define V450_RN_CODE_25_V		11
// #define V450_RN_CODE_50_V		12
// #define V450_RN_CODE_125_V		13
// #define V450_RN_CODE_250_V		14

// #define V450_OCD_ON			1	// enables open circuit detection -- for T/C's or voltages <= 500 mv
// #define V450_OCD_OFF			0

  for (i = 0; i < V450_CHNLS_PER_BOARD / 2; i++)
    {
      // RF code = 0 (16.7 samples/sec), RN code = 10 (+/-12.5v), Open Thermocouple Detection = 0
      v450_config_analog_chnl(pb, i, V450_RF_CODE_16_7_SPS, V450_RN_CODE_12_5_V, V450_OCD_OFF);
    }

  for (i = 0; i < V450_CHNLS_PER_BOARD / 2; i++)
    {
      v450_read_analog_chnl(pb, i, &volts[i], &counts[i]);
      printf("chnl: %d, volts: %f, counts = %d\n", i, volts[i], counts[i]);
    }

// #define V450_RN_CODE_TYPE_J		16 	// -210 to 1200 degC
// #define V450_RN_CODE_TYPE_K		17 	// -270 to 1372 degC
// #define V450_RN_CODE_TYPE_E		18 	// -270 to 1000 degC
// #define V450_RN_CODE_TYPE_T		19 	// -270 to 400 degC
// #define V450_RN_CODE_TYPE_R		20      // -50 to 1768 degC
// #define V450_RN_CODE_TYPE_S		21      // -50 to 1768 degC
// #define V450_RN_CODE_TYPE_B		22      // 0 to 1820 degC
// #define V450_RN_CODE_TYPE_N		23      // -270 to 1300 degC

// #define V450_RS_CODE_SHIFT		8
// #define V450_RS_CODE_MASK		0x0700
// #define V450_RS_CODE_RTD_A		0
// #define V450_RS_CODE_RTD_B		1
// #define V450_RS_CODE_RTD_C		2
// #define V450_RS_CODE_RTD_D		3
// #define V450_RS_CODE_RTD_I		4 	// internal
// #define V450_RS_CODE_RTD_F1		5       // fake 1
// #define V450_RS_CODE_RTD_F2		6       // fake 2
// #define V450_RS_CODE_RTD_NONE	7       // ice point equivalent

  for (i = V450_CHNLS_PER_BOARD / 2; i < V450_CHNLS_PER_BOARD; i++)
    {
      v450_config_tc_chnl(pb, i, V450_RF_CODE_16_7_SPS, V450_RS_CODE_RTD_C, V450_RN_CODE_TYPE_K, V450_OCD_ON, 0, 0);
    }

  for (i = V450_CHNLS_PER_BOARD / 2; i < V450_CHNLS_PER_BOARD; i++)
    {
      v450_read_tc_chnl(pb, i, 'C', &temps[i]);
      printf("chnl: %d, temp: %f\n", i, temps[i]);
    }

  // if (v450_config_rtd_type(pb, 'a', V450_RTD_TYPE_100_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v450_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  // if (v450_config_rtd_type(pb, 'b', V450_RTD_TYPE_100_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v450_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  if (v450_config_rtd_type(pb, 'c', V450_RTD_TYPE_100_OHM_PT) == -1)
    {
      printf("ERROR %s/%s: v450_config_rtd_type() failure\n", whoami, __FUNCTION__);
      exit(1);
    }

  // if (v450_config_rtd_type(pb, 'd', V450_RTD_TYPE_1000_OHM_PT) == -1)
  //   {
  //     printf("ERROR %s/%s: v450_config_rtd_type() failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  // if (v450_read_rtd_type(pb, 'a', &type) == 0)
  //   printf("INFO %s/%s: v450_read_rte_type 'a' = %d\n", whoami, __FUNCTION__, type);

  // if (v450_read_rtd_type(pb, 'b', &type) == 0)
  //   printf("INFO %s/%s: v450_read_rte_type 'b' = %d\n", whoami, __FUNCTION__, type);

  if (v450_read_rtd_type(pb, 'c', &type) == 0)
    printf("INFO %s/%s: v450_read_rte_type 'c' = %d\n", whoami, __FUNCTION__, type);

  // if (v450_read_rtd_type(pb, 'd', &type) == 0)
  //   printf("INFO %s/%s: v450_read_rte_type 'd' = %d\n", whoami, __FUNCTION__, type);

  // if (v450_execute_macro(pb, V450_MACRO_FULL_TEST, 0, 0) == -1)
  //   {
  //     printf("ERROR %s/%s: v450_execute_macro(V450_MACRO_FULL_TEST) failure\n", whoami, __FUNCTION__);
  //     exit(1);
  //   }

  exit(0);
}
