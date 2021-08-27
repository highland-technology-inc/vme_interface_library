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
// Date: Thu Aug 19 15:58:01 2021

#ifndef _V545_REGS_SDEF_H_
#define _V545_REGS_SDEF_H_

#define V545_FB_CHNLS_PER_BOARD		12
#define V545_OB_CHNLS_PER_BOARD          4
#define V545_DDS_CHNLS_PER_BOARD         8
#define V545_CCB_CHNLS_PER_BOARD        24

#define V545_CCB_CHNL_INPUT		0x0000
#define V545_CCB_CHNL_OUTPUT		0x0001
#define V545_CCB_CHNL_DELAY		0x0002
#define V545_CCB_EXCITATION_MASK	0x7f00
#define V545_CCB_EXCITATION_SHIFT	     8
#define V545_CCB_LEN			     7

#define V545_FB_BYTE_LEN		0x40

// define function block code

#define V545_FUNC_CODE_NONE			0
#define V545_FUNC_CODE_LVDT_RVDT_INPUT		3
#define V545_FUNC_CODE_LVDT_RVDT_OUTPUT		4
#define V545_FUNC_CODE_SYNCHRO_RESOLVER_INPUT	5
#define V545_FUNC_CODE_SYNCHRO_RESOLVER_OUTPUT	6

// define interface type

#define V545_ITYPE_NONE				0
#define V545_ITYPE_SYNCHRO_INPUT		1
#define V545_ITYPE_RESOLVER_INPUT		2
#define V545_ITYPE_SYNCHRO_OUTPUT		3
#define V545_ITYPE_RESOLVER_OUTPUT		4
#define V545_ITYPE_LVDT_RVDT_RATIOMETRIC_INPUT	5
#define V545_ITYPE_LVDT_RVDT_OPEN_WIRING_INPUT	6
#define V545_ITYPE_LVDT_RVDT_RATIOMETRIC_OUTPUT	7
#define V545_ITYPE_LVDT_RVDT_OPEN_WIRING_OUTPUT	8

// define macro commands

#define V545_MACRO_NOOP				0x8400
#define V545_MACRO_RESET			0x8407
#define V545_MACRO_FLASH_UNLOCK			0x8408
#define V545_MACRO_FLASH_ERASE			0x8409
#define V545_MACRO_FLASH_WRITE			0x840a
#define V545_MACRO_FLASH_CHECKSUM		0x840b
#define V545_MACRO_RESET_PSD			0x840c
#define V545_MACRO_READ_FLASH			0x8410
#define V545_MACRO_READ_POWER_SUPPLY		0x8411
#define V545_MACRO_RESET_DDS			0x8414

// register file word size

#define V545_REGFILE_WORD_SIZE	64

// LVDT/RVDT simulation function block

#define V545_LVDT_RVDT_RATIOMETRIC		0x0
#define V545_LVDT_RVDT_OPEN_WIRING		0x1

#define V545_LVDT_RVDT_NO_2X			0x0
#define V545_LVDT_RVDT_2X			0x1

// time step -- S-bits in FLG register

#define V545_TIME_INT_1MS			0x0
#define V545_TIME_INT_10MS			0x1
#define V545_TIME_INT_100MS			0x2

// #define V545_LVDT_RVDT_OPR_IDLE			0
// #define V545_LVDT_RVDT_OPR_INIT			1
#define V545_LVDT_RVDT_OPR_MOVE			2

// synchro/resolver settings

#define V545_SYNCHRO_RESOLVER_SET_SYNCHRO	0
#define V545_SYNCHRO_RESOLVER_SET_RESOLVER	1

#define V545_SYNCHRO_RESOLVER_OPR_MOVE_SIGNED		2
#define V545_SYNCHRO_RESOLVER_OPR_MOVE_SHORTEST_PATH	3
#define V545_SYNCHRO_RESOLVER_OPR_MOVE_SPIN_SIGNED	4
#define V545_SYNCHRO_RESOLVER_OPR_MOVE_HARD_STOP	5

// override block offset

#define V545_OVERRIDE_FUNC_CODE_UNUSED		0x0000
#define V545_OVERRIDE_FUNC_CODE_WATCHDOG	0x0011
#define V545_OVERRIDE_FUNC_CODE_SWITCH		0x0012

#define V545_OVERRIDE_OPR_IDLE		0x0000
#define V545_OVERRIDE_OPR_INIT		0x0001

// dds registers and control block

typedef volatile struct
{
  volatile unsigned short dfr;		// (r/w) +0x0 dds source freq
  volatile unsigned short dph;		// (r/w) +0x2 phase
  volatile unsigned short dam;		// (r/w) +0x4 amplitude
  volatile unsigned short unused;	//       +0x6
} V545_DDS_REGS;

// coil registers and control block

typedef volatile struct
{
  union {
    unsigned short reg;
    volatile struct {
      unsigned short ssr: 1;      // output = 1; input = 0
      unsigned short dl: 1;       // delay = 1; no delay = 0
      unsigned short unused0: 1;
      unsigned short fbits: 3;
      unsigned short x2: 1;
      unsigned short unused1: 1;
      unsigned short mbits: 6;    // 0-23 = ADC data from I/O channels; 24-31 = DDS frequency synthesizers 0-7
      unsigned short unused2: 1;
    } bits;
  } ctl;			// (r/w) +0x0 coil control
  union {
    unsigned short reg;
    volatile struct {
      unsigned short xfmr_type: 4; // transformer type; 0 = none, 1 = V548-1 (LVDT's), 2 = V546-1 (special), 6 = V548-6 (synchro/resolver)
      unsigned short unused0: 11;
      unsigned short cl: 1;  	// ADC has clipped at least once in the last second
    } bits;
  } sts;			// (r/o) +0x2 coil status
  volatile short amp;           // (r/w) +0x4 coil amplitude
  volatile short dly;           // (r/w) +0x6 coil delay
  volatile short rms;           // (r/o) +0x8 coil rms voltage
  volatile short psd;           // (r/o) +0xa coil phase sensitive detector
  volatile unsigned short frq;  // (r/o) +0xc coil signal frequency
  volatile unsigned short unused; //     +0xe
} V545_CCB_REGS;

// synchro/resolver acquisition function block registers

typedef volatile struct
{
  volatile unsigned short fun;  	// (r/w) +0x00 function code = 0x05
  volatile unsigned short opr;  	// (r/w) +0x02 operation
  union {
    unsigned short reg;
    volatile struct {
      unsigned short type: 4;
      unsigned short cutoff_freq_code: 3;
      unsigned short unused: 9;
    } bits;
  } flg;				// (r/w) +0x04 setup time flags
  union {
    unsigned short reg;
    volatile struct {
      unsigned short init_done: 1;	// 1 = initialization done
      unsigned short unused0: 7;
      unsigned short ovr_set: 1;  	// 1 = override set
      unsigned short unused1: 3;
      unsigned short config_error: 1;	// 1 = configuration error
      unsigned short excitation_error: 1; // 1 = excitation error
      unsigned short signal_error: 1;	// 1 = signal error
      unsigned short unused2: 1;
    } bits;
  } sts;				// (r/o) +0x06 real-time status
  union {
    unsigned short reg;
    volatile struct {
      unsigned short regfile_index: 7;	// index to pair of words in the Rnn register file that define the override position and velocity
      unsigned short unused0: 5;
      unsigned short ovrblk12: 1;
      unsigned short ovrblk13: 1;
      unsigned short ovrblk14: 1;
      unsigned short ovrblk15: 1;
    } bits;
  } ovr;				// (r/o) +0x08 override control
  volatile unsigned short unused0[3];	//       +0x0a-0x0e
  union {
    unsigned short reg;
    volatile struct {
      unsigned short A_coil: 5;
      unsigned short unused0: 3;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } synchro;
    volatile struct {
      unsigned short unused0: 8;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } resolver;
  } cp01;			 	// (r/w) +0x10 coil pointers 0, 1
  union {
    unsigned short reg;
    volatile struct {
      unsigned short C_coil: 5;   	// C_coil 0-23
      unsigned short unused0: 3;
      unsigned short B_coil: 5;		// B_coil 0-23
      unsigned short unused1: 3;
    } synchro;
    volatile struct {
      unsigned short X_coil: 5;   	// X_coil 0-23
      unsigned short unused0: 3;
      unsigned short Y_coil: 5;		// Y_coil 0-23
      unsigned short unused1: 3;
    } resolver;
  } cp23;				// (r/w) +0x12 coil pointers 2, 3
  volatile unsigned short cp45; 	// (r/w) +0x14 coil pointers 4, 5
  volatile unsigned short cp67; 	// (r/w) +0x16 coil pointers 6, 7
  volatile unsigned short unused1;      //       +0x18
  volatile unsigned short srp;  	// (r/w) +0x1a phase shift
  volatile unsigned short unused2[3];   //       +0x1c-0x20
  volatile union {                      // (r/o) +0x22 actual position
    short s;
    unsigned short us;
  } ap;
  volatile unsigned short unused3; 	//       +0x24
  volatile union {			// (r/o) +0x26 filtered position
    short s;
    unsigned short us;
  } fp;
  volatile short fv;            	// (r/o) +0x28 filtered velocity
  volatile unsigned short unused4[3];   //       +0x2a-0x2e
  volatile unsigned short msv;  	// (r/o) +0x30 measured secondary voltage
  volatile unsigned short unused5[7];   //       +0x32-0x3e
} V545_SYNCHRO_RESOLVER_INPUT_REGS;

// synchro/resolver simulation function block registers

typedef volatile struct
{
  volatile unsigned short fun;  	// (r/w) +0x00 function code = 0x06
  volatile unsigned short opr;  	// (r/w) +0x02 operation
  union {
    unsigned short reg;
    volatile struct {
      unsigned short type: 4;		// 0 = three coil synchro; 1 = two coil resolver
      unsigned short unused0: 8;
      unsigned short sbits: 2;		// smallest time interval for slew: 0=1msec, 1=10msec, 2=100msec
      unsigned short unused1: 2;
    } bits;				// (r/w) +0x04 setup time flags
  } flg;
  union {
    unsigned short reg;
    volatile struct {
      unsigned short init_done: 1;	// 1 = initialization done
      unsigned short unused0: 7;
      unsigned short ovr_set: 1;  	// 1 = override set
      unsigned short unused1: 3;
      unsigned short config_error: 1;	// 1 = configuration error
      unsigned short excitation_error: 1;	// 1 = excitation error
      unsigned short unused2: 2;
    } bits;
  } sts;				// (r/o) +0x06 real-time status
  union {
    unsigned short reg;
    volatile struct {
      unsigned short regfile_index: 7;	// index to pair of words in the Rnn register file that define the override position and velocity
      unsigned short unused0: 5;
      unsigned short ovrblk12: 1;
      unsigned short ovrblk13: 1;
      unsigned short ovrblk14: 1;
      unsigned short ovrblk15: 1;
    } bits;
  } ovr;				// (r/o) +0x08 override control
  volatile unsigned short unused0[3];	//       +0x0a-0x0e
  union {
    unsigned short reg;
    volatile struct {
      unsigned short A_coil: 5;
      unsigned short unused0: 2;
      unsigned short A_inv_flag: 1;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } synchro;
    volatile struct {
      unsigned short unused0: 8;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } resolver;
  } cp01;			 	// (r/w) +0x10 coil pointers 0, 1
  union {
    unsigned short reg;
    volatile struct {
      unsigned short C_coil: 5;   	// C_coil 0-23
      unsigned short unused0: 2;
      unsigned short C_inv_flag: 1;	// invert C coil polarity
      unsigned short B_coil: 5;		// B_coil 0-23
      unsigned short unused1: 2;
      unsigned short B_inv_flag: 1;	// invert B coil polarity
    } synchro;
    volatile struct {
      unsigned short X_coil: 5;
      unsigned short unused0: 2;
      unsigned short X_inv_flag: 1;
      unsigned short Y_coil: 5;
      unsigned short unused1: 2;
      unsigned short Y_inv_flag: 1;
    } resolver;
  } cp23;				// (r/w) +0x12 coil pointers 2, 3
  volatile unsigned short cp45; 	// (r/w) +0x14 coil pointers 4, 5
  volatile unsigned short cp67; 	// (r/w) +0x16 coil pointers 6, 7
  volatile unsigned short srk;  	// (r/w) +0x18 K amplitude scaler
  volatile short srp;           	// (r/w) +0x1a phase shift
  volatile union {			// (r/w) +0x1c hard stop 1
    short s;
    unsigned short us;
  } hs1;
  volatile union {			// (r/w) +0x1e hard stop 2
    short s;
    unsigned short us;
  } hs2;
  volatile unsigned short unused1; 	//       +0x20
  volatile union {			// (r/o) +0x22 actual position
    short s;
    unsigned short us;
  } ap;
  volatile unsigned short unused2; 	//       +0x24
  volatile union {			// (r/w) +0x26 target position
    short s;
    unsigned short us;
  } tp;
  volatile short tv;            	// (r/w) +0x28 target velocity
  union {
    volatile struct {
      volatile unsigned short brk_a; 	// (r/w) +0x2a A coil error simulation register
      volatile unsigned short brk_b; 	// (r/w) +0x2c B coil error simulation register
      volatile unsigned short brk_c; 	// (r/w) +0x2e C coil error simulation register
    } synchro;
    volatile struct {
      volatile unsigned short unused3; 	// (r/w) +0x2a 
      volatile unsigned short brk_y; 	// (r/w) +0x2c Y coil error simulation register
      volatile unsigned short brk_x; 	// (r/w) +0x2e X coil error simulation register
    } resolver;
  } coil_error;				//       +0x2a-0x2e
  volatile unsigned short unused4[8];   //       +0x30-0x3e
} V545_SYNCHRO_RESOLVER_OUTPUT_REGS;

// lvdt/rvdt acquisition function block registers

typedef volatile struct
{
  volatile unsigned short fun;  	// (r/w) +0x00 function code = 0x03
  volatile unsigned short opr;  	// (r/w) +0x02 operation
  union {
    unsigned short reg;
    volatile struct {
      unsigned short type: 4;		// 0 = ratiometric LVDT/RVDT, 1 = open-wiring LVDT/RVDT
      unsigned short filter_speed: 3;	// filtering speed 0 = 0 cutoff freq (Hz), 1 = 1, 2 = 2, 3 = 5, 4 = 10, 5 = 20, 6 = 50, 7 = 100
      unsigned short unused: 9;
    } bits;
  } flg;				// (r/w) +0x04 setup time flags
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short init_done: 1;	// 1 = initialization done
      unsigned short unused0: 7;
      unsigned short ovr_set: 1;		// 1 = override set
      unsigned short unused1: 3;
      unsigned short config_error: 1;	// 1 = configuration error
      unsigned short excitation_error: 1;	// 1 = excitation error
      unsigned short signal_error: 1;     // 1 = signal error < 100 mV
      unsigned short unused2: 1;
    } bits;
  } sts;				// (r/o) +0x06 real-time status
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short regfile_index: 7;	// index to pair of words in the Rnn register file that define the override position and velocity
      unsigned short unused0: 5;
      unsigned short ovrblk12: 1;
      unsigned short ovrblk13: 1;
      unsigned short ovrblk14: 1;
      unsigned short ovrblk15: 1;
    } bits;
  } ovr;				// (r/o) +0x08 override control
  volatile unsigned short unused0[3];	//       +0x0a-0x0e
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short unused0: 8;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } bits;
  } cp01;			 	// (r/w) +0x10 coil pointers 0, 1
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short B_coil: 5;   	// B_coil 0-23
      unsigned short unused0: 3;
      unsigned short A_coil: 5;		// A_coil 0-23
      unsigned short unused1: 3;
    } bits;
  } cp23;				// (r/w) +0x12 coil pointers 2, 3
  volatile unsigned short cp45; 	// (r/w) +0x14 coil pointers 4, 5
  volatile unsigned short cp67; 	// (r/w) +0x16 coil pointers 6, 7
  volatile unsigned short unused1;      //       +0x18 
  volatile short srp;           	// (r/w) +0x1a phase shift
  volatile unsigned short unused2[3];   //       +0x1c-0x20
  volatile short ad;            	// (r/o) +0x22 actual displacement (counts) -- always signed
  volatile unsigned short unused3;      //       +0x24
  volatile short fd;            	// (r/o) +0x26 filtered displacement
  volatile unsigned short unused4[4];   //       +0x28-0x2e
  volatile unsigned short msv;  	// (r/o) +0x30 measured secondary velocity
  volatile unsigned short unused5[7];   //       +0x32-0x3e
} V545_LVDT_RVDT_INPUT_REGS;

// lvdt/rvdt simulation function block registers

typedef volatile struct
{
  volatile unsigned short fun;  	// (r/w) +0x00 function code = 0x4 LVDT/RVDT simulation
  volatile unsigned short opr;  	// (r/w) +0x02 operation IDLE = 0, INIT = 1, MOVE = 2
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short type: 4;     	// 0 = ratiometric LVDT/RVDT, 1 = open-wiring LVDT/RVDT
      unsigned short unused0: 4;
      unsigned short x2: 1;       	// 1 = multiply output 2x
      unsigned short unused1: 3;
      unsigned short sbits: 2;    	// smallest time interval for slew: 0=1msec, 1=10msec, 2=100msec
      unsigned short unused2: 2;
    } bits;
  } flg;				// (r/w) +0x04 setup time flags
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short init_done: 1;	// 1 = initialization done
      unsigned short unused0: 7;
      unsigned short ovr_set: 1;  	// 1 = override set
      unsigned short unused1: 3;
      unsigned short config_error: 1;	// 1 = configuration error
      unsigned short excitation_error: 1;	// 1 = excitation error
      unsigned short unused2: 2;
    } bits;
  } sts;
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short regfile_index: 7;	// index to pair of words in the Rnn register file that define the override position and velocity
      unsigned short unused0: 5;
      unsigned short ovrblk12: 1;
      unsigned short ovrblk13: 1;
      unsigned short ovrblk14: 1;
      unsigned short ovrblk15: 1;
    } bits;
  } ovr;				// (r/o) +0x08 override control
  volatile unsigned short unused0[3];	//       +0x0a-0x0e
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short unused0: 8;
      unsigned short EX_coil: 5;
      unsigned short unused1: 3;
    } bits;
  } cp01;			 	// (r/w) +0x10 coil pointers 0, 1
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short B_coil: 5;   	// B_coil 0-23
      unsigned short unused0: 2;
      unsigned short B_inv_flag: 1;	// invert B coil polarity
      unsigned short A_coil: 5;		// A_coil 0-23
      unsigned short unused1: 2;
      unsigned short A_inv_flag: 1;	// invert A coil polarity
    } bits;
  } cp23;				// (r/w) +0x12 coil pointers 2, 3
  volatile unsigned short cp45; 	// (r/w) +0x14 coil pointers 4, 5
  volatile unsigned short cp67; 	// (r/w) +0x16 coil pointers 6, 7
  volatile unsigned short srk;  	// (r/w) +0x18 K amplitude scaler
  volatile short srp;           	// (r/w) +0x1a phase shift
  volatile unsigned short unused1[3];   //       +0x1c-0x20
  volatile short ad; 			// (r/o) +0x22 actual displacement (counts) -- always signed
  volatile unsigned short unused2;      //       +0x24
  volatile short td;            	// (r/w) +0x26 target displacement
  volatile unsigned short tv;		// (r/w) +0x28 target velocity
  volatile unsigned short unused3;      //       +0x2a
  volatile struct {
    unsigned short pt01percent: 15;	// 1 = 0.01 percent, 10000 = 100.0 percent
    unsigned short active: 1;
  } A_coil_brk; 			// (r/w) +0x2c LVDT Va coil
  volatile struct {
    unsigned short pt01percent: 15;	// 1 = 0.01 percent, 10000 = 100.0 percent
    unsigned short active: 1;
  } B_coil_brk;				// (r/w) +0x2e LVDT Vb coil
  volatile unsigned short unused4[8];   //       +0x30-0x3e
} V545_LVDT_RVDT_OUTPUT_REGS;

// V545 override block registers

typedef volatile struct
{
  volatile unsigned short fun;		// (r/w) +0x00 function
  volatile unsigned short opr;		// (r/w) +0x02 operation
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short swin_mask: 4;    	// 0x1 = J3 pin 2, 0x2 = J3 pin 3, 0x4 = J3 pin 4, 0x8 = J3 pin 5
      unsigned short unused0: 4;
      unsigned short len: 1;
      unsigned short inv: 1;
      unsigned short tg: 1;
      unsigned short unused1: 5;
    } bits;
  } flg;				// (r/w) +0x04 flag
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short enabled: 1;
      unsigned short unused0: 6;
      unsigned short tripped: 1;
      unsigned short latched: 1;
      unsigned short unused1: 3;
      unsigned short config_error: 1;		// 1 = configuration error
      unsigned short excitation_error: 1;	// 1 = excitation error
      unsigned short unused2: 2;
    } bits;
  } sts;				// (r/o) +0x06 status
  volatile unsigned short unused0[2];	//       +0x08-0x0a
  volatile unsigned short dog;		// (r/w) +0x0c watchdog
  volatile unsigned short trg;		// (r/w) +0x0e force override via s/w
  volatile unsigned short unused1[8];	//       +0x10-0x1e
  union {
    unsigned short reg;    
    volatile struct {
      unsigned short regfile_index: 7;	// 0-63
      unsigned short unused0: 5;
      unsigned short override_blknum: 4;// override blkd number 12-15
    } bits;
  } ovr[12];				// (r/w) +0x20-0x36 override words
  volatile unsigned short unused2[4];	//       +0x38-0x3e
} V545_OVERRIDE_BLOCK_REGS;

// generic function block access

typedef volatile struct
{
  volatile unsigned short fun;  	// (r/w) +0x00 function code = 0x03
  volatile unsigned short opr;  	// (r/w) +0x02 operation
  volatile unsigned short flg;  	// (r/w) +0x04 setup time flags
  volatile unsigned short sts;  	// (r/o) +0x06 real-time status
  volatile unsigned short unused0[28];  //       +0x08-0x3e
} V545_FUNC_BLOCK_REGS;

// V545 register map

typedef volatile struct
{							//       offset
  volatile unsigned short vximfr;			// (r/o) 0x000
  volatile unsigned short vxitype;			// (r/o) 0x002
  volatile unsigned short unused0;			// (r/o) 0x004
  volatile unsigned short serial;			// (r/o) 0x006
  volatile unsigned short romid;			// (r/o) 0x008
  volatile unsigned short romrev;			// (r/o) 0x00a
  volatile unsigned short mcount;			// (r/o) 0x00c
  volatile unsigned short dash;				// (r/o) 0x00e
  volatile unsigned short unused1[2];   		//       0x010-0x012
  volatile unsigned short state;			// (r/o) 0x014
  volatile unsigned short uled;				// (r/w) 0x016
  volatile unsigned short biss;				// (r/o) 0x018
  volatile unsigned short calid;			// (r/o) 0x01a
  volatile unsigned short ycal;				// (r/o) 0x01c
  volatile unsigned short dcal;				// (r/o) 0x01e
  volatile unsigned short swin;				// (r/o) 0x020
  volatile unsigned short swout;			// (r/w) 0x022
  volatile unsigned short unused2[6];			//       0x024-0x02e
  volatile unsigned short macro;			// (r/w) 0x030
  volatile unsigned short param0;			// (r/w) 0x032
  volatile unsigned short param1;			// (r/w) 0x034
  volatile unsigned short param2;			// (r/w) 0x036
  volatile unsigned short unused3[36];			//       0x038-0x07e
  volatile union {					// (r/w) 0x080-0x0fe
    volatile unsigned short raw_regs[V545_REGFILE_WORD_SIZE];
    volatile struct {
      union {
        short s;
        unsigned short us;
      } pos;
      union {
        short s;
        unsigned short us;
      } vel;
      // volatile short pos;
      // volatile short vel;
    } entry [V545_REGFILE_WORD_SIZE / 2];
  } regfile;
  volatile V545_DDS_REGS dds[V545_DDS_CHNLS_PER_BOARD];	//       0x100
  volatile unsigned short unused4[96];			// 
  volatile V545_CCB_REGS ccb[V545_CCB_CHNLS_PER_BOARD];	//       0x200-0x37e
  volatile unsigned short unused5[64];			//       0x380-0x3fe
  volatile union
  {
    unsigned short raw_regs[V545_FB_BYTE_LEN / sizeof(unsigned short)];
    V545_FUNC_BLOCK_REGS fbregs;			// 0x400-0x6fe
    V545_SYNCHRO_RESOLVER_INPUT_REGS sri;		// 0x400-0x6fe
    V545_SYNCHRO_RESOLVER_OUTPUT_REGS sro;		// 0x400-0x6fe
    V545_LVDT_RVDT_INPUT_REGS lri;			// 0x400-0x6fe
    V545_LVDT_RVDT_OUTPUT_REGS lro;			// 0x400-0x6fe
    V545_OVERRIDE_BLOCK_REGS ob;
  } fb[V545_FB_CHNLS_PER_BOARD + V545_OB_CHNLS_PER_BOARD];
} V545_REGS;

#endif
