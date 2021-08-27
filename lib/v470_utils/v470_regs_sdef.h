// -*- c++ -*- emacs c++ mode for comments

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
// Date: Sun Aug 08 11:44:03 2021

#ifndef _V470_REGS_SDEF_H_
#define _V470_REGS_SDEF_H_

#define V470_CHNLS_PER_BOARD	       16
#define V470_COUNTS_MIN_16         -32768
#define V470_COUNTS_MAX_16          32768

// RFLAGS RTD error flags

#define V470_RTD_ERROR_LM71	0x0080
#define V470_RTD_ERROR_FPGA	0x0040
#define V470_RTD_ERROR_CERR	0x0020
#define V470_RTD_ERROR_ERRT	0x0010
#define V470_RTD_ERROR_ERRD	0x0008
#define V470_RTD_ERROR_ERRC	0x0004
#define V470_RTD_ERROR_ERRB	0x0002
#define V470_RTD_ERROR_ERRA	0x0001

// CALID register values

#define V470_CALID_GOOD	0x57c6  // 22470
#define V470_CALID_BAD	0xdefc

// macros supported in V470

#define V470_MACRO_NOOP			0x0000
#define V470_MACRO_SETALL_TYPE_J	0x8401	// set all channels to Type J thermocouple at 16.7 Hz, using the onboard reference junction
#define V470_MACRO_SETALL_TYPE_K	0x8402	// set all channels to Type K thermocouple at 16.7 Hz, using the onboard reference junction
#define V470_MACRO_SETALL_TYPE_E	0x8403	// set all channels to Type E thermocouple at 16.7 Hz, using the onboard reference junction
#define V470_MACRO_SETALL_TYPE_T	0x8404	// set all channels to Type T thermocouple at 16.7 Hz, using the onboard reference junction
#define V470_MACRO_SETALL_12_5_V	0x8405	// set all channels to +/- 12.5V output
#define V470_MACRO_SETALL_80_MV		0x8406	// set all channels to +/- 80mV
#define V470_MACRO_SETALL_25_MV		0x8407	// set all channels to +/- 25VmV
#define V470_MACRO_BIST_ALL		0x8410  // BIST all
#define V470_MACRO_BIST_ONE_CHNL	0x8411  // BIST one channel
#define V470_MACRO_HARD_REBOOT		0x8420	// hard reboot -- reloads PFGSs, restarts code, disappears from bus for 4 seconds
#define V470_MACRO_SOFT_REBOOT		0x8421  // soft reboot -- remains on bus

// RTD types

#define V470_RTD_TYPE_UNUSED		0
#define V470_RTD_TYPE_100_OHM_PT	1
#define V470_RTD_TYPE_1000_OHM_PT	2

// RN + RS codes for channel control registers

// RN codes

#define V470_RN_CODE_SHIFT		0
#define V470_RN_CODE_MASK		0x001f
#define V470_RN_CODE_OFF		0
#define V470_RN_CODE_25_MV		1	// +/-25mv
#define V470_RN_CODE_50_MV		2       // +/-50mv
#define V470_RN_CODE_80_MV		3       // +/-80mv
#define V470_RN_CODE_125_MV		4       // +/-125mv
#define V470_RN_CODE_250_MV		5       // +/-250mv
#define V470_RN_CODE_500_MV		6       // +/-500mv
#define V470_RN_CODE_1_25_V		7       // +/-1.25v
#define V470_RN_CODE_2_5_V		8       // +/-2.5v
#define V470_RN_CODE_5_V		9       // +/-5.0v
#define V470_RN_CODE_12_5_V		10	// +/-12.5v

#define V470_RN_CODE_TYPE_J		16 	// -210 to 1200 degC
#define V470_RN_CODE_TYPE_K		17 	// -270 to 1372 degC
#define V470_RN_CODE_TYPE_E		18 	// -270 to 1000 degC
#define V470_RN_CODE_TYPE_T		19 	// -270 to 400 degC
#define V470_RN_CODE_TYPE_R		20      // -50 to 1768 degC
#define V470_RN_CODE_TYPE_S		21      // -50 to 1768 degC
#define V470_RN_CODE_TYPE_B		22      // 0 to 1820 degC
#define V470_RN_CODE_TYPE_N		23      // -270 to 1300 degC

// RTD RS code

#define V470_RS_CODE_SHIFT		8
#define V470_RS_CODE_MASK		0x0700
#define V470_RS_CODE_RTD_A		0
#define V470_RS_CODE_RTD_B		1
#define V470_RS_CODE_RTD_C		2
#define V470_RS_CODE_RTD_D		3
#define V470_RS_CODE_RTD_INTERNAL	4
#define V470_RS_CODE_RTD_FAKE1		5
#define V470_RS_CODE_RTD_FAKE2		6
#define V470_RS_CODE_RTD_NONE		7

// channel type

#define V470_CHNL_TYPE_OFF		0
#define V470_CHNL_TYPE_DA		1
#define V470_CHNL_TYPE_TC		2

// BISS register

#define V470_REG_BISS_BAV		0x0001	// BIST option is available
#define V470_REG_BISS_BSY		0x0080  // BIST busy
#define V470_REG_BISS_TSF		0x0800  // error with local reference junction temperature sensor
#define V470_REG_BISS_RTF		0x1000  // error with RTD acquisition subsystem
#define V470_REG_BISS_ISO		0x2000  // electrical leakage detected from channel common to module ground
#define V470_REG_BISS_BCS		0x4000  // soft error detected
#define V470_REG_BISS_BCH		0x8000	// hard error detected

#define V470_BIST_NONE			0x0
#define V470_BIST_SNGL_CHNL		0x1
#define V470_BIST_FULL			0x2
#define V470_BIST_SNGL_CHNL_ACTIVE	0x4
#define V470_BIST_FULL_ACTIVE		0x8

// error summary word for BIST

#define V470_MAX_ERRSUM		   21

#define V470_REG_ERRSUM_CHNL_MASK	0x000f
#define V470_REG_ERRSUM_TYPE_MASK	0x00f0
#define V470_REG_ERRSUM_25MV	   	     1
#define V470_REG_ERRSUM_50MV   		     2
#define V470_REG_ERRSUM_80MV   		     3
#define V470_REG_ERRSUM_125MV		     4
#define V470_REG_ERRSUM_250MV		     5
#define V470_REG_ERRSUM_500MV		     6
#define V470_REG_ERRSUM_1_25V		     7
#define V470_REG_ERRSUM_2_5V   		     8
#define V470_REG_ERRSUM_5V     		     9
#define V470_REG_ERRSUM_12_5V		    10
#define V470_REG_ERRSUM_TSF		0x0800
#define V470_REG_ERRSUM_RTF		0x1000
#define V470_REG_ERRSUM_ISO		0x2000
#define V470_REG_ERRSUM_BCS		0x4000
#define V470_REG_ERRSUM_BCH		0x8000

typedef volatile struct
{                                  	// offset
  volatile unsigned short vximfr;	// 0x00
  volatile unsigned short vxitype;	// 0x02
  volatile unsigned short unused0;	// 0x04
  volatile unsigned short serial;	// 0x06
  volatile unsigned short romid;	// 0x08
  volatile unsigned short romrev;       // 0x0a
  volatile unsigned short mcount;	// 0x0c
  volatile unsigned short unused1; 	// 0x0e
  volatile unsigned short cflags;	// 0x10
  volatile unsigned short rflags;	// 0x12
  volatile unsigned short unused2; 	// 0x14
  volatile unsigned short relays;	// 0x16
  volatile unsigned short uled;		// 0x18
  volatile unsigned short mode;		// 0x1a
  volatile unsigned short calid;	// 0x1c
  volatile unsigned short biss; 	// 0x1e
  volatile unsigned short macro;        // 0x20
  volatile unsigned short param0;	// 0x22
  volatile unsigned short param1;       // 0x24
  volatile unsigned short param2;       // 0x26
  volatile unsigned short unused3;      // 0x28
  volatile unsigned short unused4;      // 0x2a
  volatile unsigned short unused5;      // 0x2c
  volatile unsigned short unused6;      // 0x2e
  volatile unsigned short ycal;         // 0x30
  volatile unsigned short dcal;         // 0x32
  volatile unsigned short unused7;      // 0x34
  volatile unsigned short unused8;      // 0x36
  volatile unsigned short unused9;      // 0x38
  volatile unsigned short unused10;     // 0x3a
  volatile unsigned short unused11;     // 0x3c
  volatile unsigned short unused12;     // 0x3e
  volatile unsigned short rtda;		// 0x40
  volatile short tempa;			// 0x42
  volatile unsigned short rtdb;		// 0x44
  volatile short tempb;                 // 0x46
  volatile unsigned short rtdc;         // 0x48
  volatile short tempc;                 // 0x4a
  volatile unsigned short rtdd;         // 0x4c
  volatile short tempd;                 // 0x4e
  volatile short tempr;                 // 0x50
  volatile unsigned short unused13;     // 0x52
  volatile unsigned short unused14;     // 0x54
  volatile unsigned short unused15;     // 0x56
  volatile unsigned short rahi;         // 0x58
  volatile unsigned short ralo;         // 0x5a
  volatile unsigned short rbhi;         // 0x5c
  volatile unsigned short rblo;         // 0x5e
  volatile unsigned short rchi;         // 0x60
  volatile unsigned short rclo;         // 0x62
  volatile unsigned short rdhi;         // 0x64
  volatile unsigned short rdlo;         // 0x66
  volatile unsigned short trhi;         // 0x68
  volatile unsigned short trlo;         // 0x6a
  volatile unsigned short bivh;         // 0x6c
  volatile unsigned short bivl;         // 0x6e
  volatile unsigned short unused16;     // 0x70
  volatile unsigned short unused17;     // 0x72
  volatile unsigned short unused18;     // 0x74
  volatile unsigned short unused19;     // 0x76
  volatile unsigned short fake1;        // 0x78
  volatile unsigned short fake2;        // 0x7a
  volatile unsigned short unused20;     // 0x7c
  volatile unsigned short unused21;     // 0x7e
  volatile struct                       // 0x80
  {
    volatile short val;
    volatile unsigned short ctl;
    volatile unsigned short dvl;
    volatile unsigned short unused;
  } ccb[V470_CHNLS_PER_BOARD];
  volatile unsigned short buffer[128];  // registers used for BIST
} V470_REGS;

#endif
