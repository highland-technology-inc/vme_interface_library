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
// Date: Sun Aug 08 11:37:07 2021

#ifndef _V450_REGS_SDEF_H_
#define _V450_REGS_SDEF_H_

#define V450_CHNLS_PER_BOARD	16
#define V450_COUNTS_MIN_24     -8388608
#define V450_COUNTS_MAX_24	8388608
#define V450_COUNTS_MIN_16     -32768
#define V450_COUNTS_MAX_16	32768

// RTD types

#define V450_RTD_TYPE_UNUSED		0
#define V450_RTD_TYPE_100_OHM_PT	1
#define V450_RTD_TYPE_1000_OHM_PT	2

// channel control register settings

#define V450_RN_CODE_SHIFT		0
#define V450_RN_CODE_MASK		0x001f
#define V450_RN_CODE_OFF		0
#define V450_RN_CODE_25_MV		1	// +/-25mv
#define V450_RN_CODE_50_MV		2       // +/-50mv
#define V450_RN_CODE_80_MV		3       // +/-80mv
#define V450_RN_CODE_125_MV		4       // +/-125mv
#define V450_RN_CODE_250_MV		5       // +/-250mv
#define V450_RN_CODE_500_MV		6       // +/-500mv
#define V450_RN_CODE_1_25_V		7       // +/-1.25v
#define V450_RN_CODE_2_5_V		8       // +/-2.5v
#define V450_RN_CODE_5_V		9       // +/-5.0v
#define V450_RN_CODE_12_5_V		10	// +/-12.5v
#define V450_RN_CODE_25_V		11	// +/-25.0v
#define V450_RN_CODE_50_V		12	// +/-50.0v
#define V450_RN_CODE_125_V		13	// +/-125.0v
#define V450_RN_CODE_250_V		14	// +/-250.0v
#define V450_RN_CODE_TYPE_J		16 	// -210 to 1200 degC
#define V450_RN_CODE_TYPE_K		17 	// -270 to 1372 degC
#define V450_RN_CODE_TYPE_E		18 	// -270 to 1000 degC
#define V450_RN_CODE_TYPE_T		19 	// -270 to 400 degC
#define V450_RN_CODE_TYPE_R		20      // -50 to 1768 degC
#define V450_RN_CODE_TYPE_S		21      // -50 to 1768 degC
#define V450_RN_CODE_TYPE_B		22      // 0 to 1820 degC
#define V450_RN_CODE_TYPE_N		23      // -270 to 1300 degC

#define V450_OCD_SHIFT			7
#define V450_OCD_MASK			0x0080
#define V450_OCD_OFF			0
#define V450_OCD_ON			1	// enables open circuit detection -- for T/C's or voltages <= 500 mv

#define V450_RS_CODE_SHIFT		8
#define V450_RS_CODE_MASK		0x0700
#define V450_RS_CODE_RTD_A		0
#define V450_RS_CODE_RTD_B		1
#define V450_RS_CODE_RTD_C		2
#define V450_RS_CODE_RTD_D		3
#define V450_RS_CODE_RTD_I		4 	// internal
#define V450_RS_CODE_RTD_F1		5       // fake 1
#define V450_RS_CODE_RTD_F2		6       // fake 2
#define V450_RS_CODE_RTD_NONE		7       // ice point equivalent

#define V450_RF_CODE_SHIFT		12
#define V450_RF_MASK			0x7000
#define V450_RF_CODE_16_7_SPS		0	// 16.7 samples per second
#define V450_RF_CODE_4_17_SPS		1	// 4.17 samples per second
#define V450_RF_CODE_8_33_SPS		2	// 8.33 samples per second
#define V450_RF_CODE_33_3_SPS		3       // 33.3 samples per second
#define V450_RF_CODE_62_5_SPS		4	// 62.5 samples per second
#define V450_RF_CODE_125_SPS		5	// 125 samples per second
#define V450_RF_CODE_250_SPS		6       // 250 samples per second
#define V450_RF_CODE_500_SPS		7	// 500 samples per second

// digital filter types

#define V450_DF_NONE			0	// digital filter disabled
#define V450_DF_1_HZ_BW			1       // 1 Hz 16-pole Butterworth
#define V450_DF_2_HZ_BW			2	// 2 Hz 16-pole Butterworth
#define V450_DF_5_HZ_BW			3	// 5 Hz 16-pole Butterworth
#define V450_DF_10_HZ_BW		4       // 10 Hz 16-pole Butterworth
#define V450_DF_20_HZ_BW		5	// 20 Hz 16-pole Butterworth
#define V450_DF_50_HZ_BW		6	// 50 Hz 16-pole Butterworth
#define V450_DF_100_HZ_BW		7	// 100 Hz 16-pole Butterworth
#define V450_DF_1_HZ_BS			11	// 1 Hz 16-pole Bessel
#define V450_DF_2_HZ_BS			12	// 2 Hz 16-pole Bessel
#define V450_DF_5_HZ_BS			13	// 5 Hz 16-pole Bessel
#define V450_DF_10_HZ_BS		14	// 10 Hz 16-pole Bessel
#define V450_DF_20_HZ_BS		15	// 20 Hz 16-pole Bessel
#define V450_DF_50_HZ_BS		16	// 50 Hz 16-pole Bessel
#define V450_DF_100_HZ_BS		17	// 100 Hz 16-pole Bessel

// macros supported in V450-1E

#define V450_MACRO_NOOP			0x0000
#define V450_MACRO_SETALL_TYPE_J	0x8401	// set all channels to Type J thermocouple at 16.7 Hz, using the onboard reference junction
#define V450_MACRO_SETALL_TYPE_K	0x8402	// set all channels to Type K thermocouple at 16.7 Hz, using the onboard reference junction
#define V450_MACRO_SETALL_TYPE_E	0x8403	// set all channels to Type E thermocouple at 16.7 Hz, using the onboard reference junction
#define V450_MACRO_SETALL_TYPE_T	0x8404	// set all channels to Type T thermocouple at 16.7 Hz, using the onboard reference junction
#define V450_MACRO_SETALL_12_5_V	0x8405	// set all channels to +/- 12.5V at 16.7 Hz
#define V450_MACRO_SETALL_80_MV		0x8406	// set all channels to +/- 80mV at 16.7 Hz
#define V450_MACRO_SETALL_25_MV		0x8407	// set all channels to +/- 25mV at 16.7 Hz
#define V450_MACRO_SET_DFILT		0x8408	// set digital filtering
#define V450_MACRO_GET_DFILT		0x8409  // get digital filtering setup
#define V450_MACRO_GET_AFILT		0x840b	// get analog filtering types
#define V450_MACRO_SYNC			0x840c	// sync all channels -- PARAM0 bitmask specifies channels to sync
#define V450_MACRO_MEA_TC_LOOP_RES	0x8418	// measure thermocouple loop resistance -- PARAM0 bitmask specifies channels to measure
#define V450_MACRO_HARD_REBOOT		0x8420	// hard reboot -- reloads PFGSs, restarts code, disappears from bus for 4 seconds
#define V450_MACRO_SOFT_REBOOT		0x8421  // soft reboot -- remains on bus

// macros supported in V450-2

#define V450_MACRO_FULL_TEST		0x8410	// full test sequence -- tests all channels, as well as RTD anc temperature sensor subsystems
#define V450_MACRO_SINGLE_CHNL_TEST	0x8411  // single channel test sequence -- number 0-15 in PARAM0
#define V450_MACRO_MULT_CHNL_TEST	0x8412  // multiple channel test sequence --bitmask in PARAM0

// V450 registers

typedef volatile struct 
{					// offset
  volatile unsigned short vximfr; 	// 0x00
  volatile unsigned short vxitype;      // 0x02
  volatile unsigned short unused0;      // 0x04
  volatile unsigned short serial;       // 0x06
  volatile unsigned short romid;        // 0x08
  volatile unsigned short romrev;       // 0x0a
  volatile unsigned short mcount;       // 0x0c
  volatile unsigned short dfilt;        // 0x0e
  volatile unsigned short cflags;       // 0x10
  volatile unsigned short rflags;       // 0x12
  volatile unsigned short afilt;        // 0x14
  volatile unsigned short relays;       // 0x16
  volatile unsigned short uled;         // 0x18
  volatile unsigned short mode;         // 0x1a
  volatile unsigned short calid;        // 0x1c
  volatile unsigned short biss;         // 0x1e
  volatile unsigned short macro;        // 0x20
  volatile unsigned short param0;       // 0x22
  volatile unsigned short param1;       // 0x24
  volatile unsigned short param2;       // 0x26
  volatile unsigned short ycal;         // 0x28
  volatile unsigned short dcal;         // 0x2a
  volatile unsigned short fake1;        // 0x2c
  volatile unsigned short fake2;        // 0x2e
  volatile unsigned short rtda;         // 0x30
  volatile signed short tempa;          // 0x32
  volatile unsigned short rtdb;         // 0x34
  volatile signed short tempb;          // 0x36
  volatile unsigned short rtdc;         // 0x38
  volatile signed short tempc;          // 0x3a
  volatile unsigned short rtdd;         // 0x3c
  volatile signed short tempd;          // 0x3e
  volatile signed short tempr;          // 0x40
  volatile unsigned short unused1;      // 0x42
  volatile unsigned short rahi;         // 0x44
  volatile unsigned short ralo;         // 0x46
  volatile unsigned short rbhi;         // 0x48
  volatile unsigned short rblo;         // 0x4a
  volatile unsigned short rchi;         // 0x4c
  volatile unsigned short rclo;         // 0x4e
  volatile unsigned short rdhi;         // 0x50
  volatile unsigned short rdlo;         // 0x52
  volatile unsigned short trhi;         // 0x54
  volatile unsigned short trlo;         // 0x56
  volatile unsigned short lbhi;         // 0x58
  volatile unsigned short lblo;         // 0x5a
  volatile struct
  {					// 0x5c - 0x9a
    volatile signed short dh;
    volatile signed short dl;
  } counts[16];       			// counts for volts
  volatile struct
  {					// 0x9c - 0xfa
    volatile unsigned short ctl;
    volatile unsigned short upc;
    volatile unsigned short res;
  } ccb[16];
  volatile unsigned short unused3;      // 0xfc
  volatile unsigned short bistdac;      // 0xfe
  volatile union
  {
    volatile struct
    {
      volatile unsigned short reg[V450_CHNLS_PER_BOARD]; // 0x100 - 0x11e -- used for analog and digital filtering
      volatile unsigned short unused2[0xe0]; // 
    } filt;
    volatile struct
    {
      volatile unsigned short blk[21][6]; // 0x100
      volatile unsigned short stage;      // 0x1fc
      volatile unsigned short numerrors;  // 0x1fe
    } bist;
  } ext;      
} V450_REGS;

#endif
