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
// Date: Thu Sep 02 10:05:01 2021

#ifndef _V375_REGS_SDEF_H_
#define _V375_REGS_SDEF_H_

#define V375_CHNLS_PER_BOARD	4
#define V375_REFERENCE_SOURCE	40000000.0L	// reference clock on board
#define V375_WAVETABLE_SIZE	65536           // wavetable memory size
#define MAX_UINT		4294967296.0L	// 2**32

// waveform blocksize

#define V375_WAVEFORM_SIZE_64		0	// 2^(6+n)
#define V375_WAVEFORM_SIZE_128		1
#define V375_WAVEFORM_SIZE_256		2
#define V375_WAVEFORM_SIZE_512		3
#define V375_WAVEFORM_SIZE_1K		4
#define V375_WAVEFORM_SIZE_2K		5
#define V375_WAVEFORM_SIZE_4K		6
#define V375_WAVEFORM_SIZE_8K		7
#define V375_WAVEFORM_SIZE_16K		8
#define V375_WAVEFORM_SIZE_32K		9
#define V375_WAVEFORM_SIZE_64K	       10

// waveform number of blocks as a f(blocksize)

#define V375_WAVEFORM_NUMBLKS_64     1024	// 2^16 / blk_size
#define V375_WAVEFORM_NUMBLKS_128     512
#define V375_WAVEFORM_NUMBLKS_256     256
#define V375_WAVEFORM_NUMBLKS_512     128
#define V375_WAVEFORM_NUMBLKS_1K       64
#define V375_WAVEFORM_NUMBLKS_2K       32
#define V375_WAVEFORM_NUMBLKS_4K       16
#define V375_WAVEFORM_NUMBLKS_8K	8
#define V375_WAVEFORM_NUMBLKS_16K	4
#define V375_WAVEFORM_NUMBLKS_32K	2
#define V375_WAVEFORM_NUMBLKS_64K       1

// low pass filter

#define V375_LOW_PASS_FILTER_3K		0
#define V375_LOW_PASS_FILTER_6K		1
#define V375_LOW_PASS_FILTER_15K	2
#define V375_LOW_PASS_FILTER_30K	3
#define V375_LOW_PASS_FILTER_60K	4
#define V375_LOW_PASS_FILTER_150K	5
#define V375_LOW_PASS_FILTER_300K	6
#define V375_LOW_PASS_FILTER_OFF	7

// max builtin parameters

#define V375_MAX_BUILTIN_HARMONIC 51
#define V375_MAX_BUILTIN_SHORT_TEETH 16
#define V375_MAX_BUILTIN_PULSES 35

// max wavespec parameters

#define V375_MAX_WAVESPEC_RAW 65536
#define V375_MAX_WAVESPEC_HARMONIC 20
#define V375_MAX_WAVESPEC_SHORT_TEETH 20
#define V375_MAX_WAVESPEC_PULSES 20

// wavespec types

#define V375_WAVESPEC_NONE		0
#define V375_WAVESPEC_TYPE_RAW		1
#define V375_WAVESPEC_TYPE_SINE		2
#define V375_WAVESPEC_TYPE_FOURIER	3
#define V375_WAVESPEC_TYPE_GEAR		4
#define V375_WAVESPEC_TYPE_PULSETRAIN	5

// DDS max

// #define V375_MAX_DDS_FREQ		15000000
// #define V375_MAX_DDS_COUNTS_AT_15MHZ	0x60000000	//  15 MHz / 40 MHz = max_freq_request / 2^32

// macros

#define V375_MACRO_BUILD_FOURIER_SERIES		0x01
#define V375_MACRO_ADD_FOURIER_SERIES		0x02
#define V375_MACRO_BUILD_GEAR_WAVEFORM		0x03
#define V375_MACRO_BUILD_PULSE_TRAIN		0x05

#define V375_MACRO_OVERLAY_PULSE_TRAIN		0x06
#define V375_MACRO_LOAD_CONSTANT		0x09
#define V375_MACRO_MEASURE_FREQ			0x0d
#define V375_MACRO_SWEEP_FREQ			0x0f

#define V375_MACRO_REINITIALIZE			0x20
#define V375_MACRO_TEST_VME_DUAL_PORT_MEM	0x22
#define V375_MACRO_TEST_WAVEFORM_MEM		0x23
#define V375_MACRO_TEST_WATCHDOG_TIMER		0x24

#define V375_MACRO_HIBERNATE			0x25
#define V375_MACRO_TEST_CPU_STATIC_RAM		0x26

// V375 register map

typedef volatile struct
{							//       offset
  volatile unsigned short vximfr;			// 0x000 manufacturer's ID -- should read 0xfeee
  volatile unsigned short vxitype;			// 0x002 module type -- 22375 (0x5767)
  volatile unsigned short vxistatus;			// 0x004 
  volatile unsigned short unused0;			// 0x006
  volatile unsigned short romid;			// 0x008 firmware version -- 22376 (0x5768)
  volatile unsigned short romrev;			// 0x00a firmware revision - 'E' (0x45)
  volatile unsigned short mcount;			// 0x00c cpu activity counter
  volatile unsigned short unused1;			// 0x00e
  volatile unsigned short master;	   		// 0x010 synthesizer master/slave
  volatile unsigned short resets;	   		// 0x012 
  volatile unsigned short strobe;	   		// 0x014 trigger new frequency
  volatile unsigned short unused2;			// 0x016
  volatile unsigned short unused3;			// 0x018
  volatile unsigned short unused4;			// 0x01a
  volatile unsigned short unused5;			// 0x01c
  volatile unsigned short vrun;				// 0x01e 
  volatile unsigned short macro;			// 0x020
  union {
    short s;
    unsigned short us;
  } cp[111];						// 0x022-0x0fe
  volatile struct {                                     // 0x100-0x13e
    volatile unsigned short hi;				// 
    volatile unsigned short lo;				// hi:lo = freq * 2^32 * blksize / 40,000,000
    volatile unsigned short unused6[6];			// 
  } freq[4];
  volatile short amp[4]; 				// 0x140-0x146 -32768 = -10.0v; +32767 = +9.9997v
  volatile short offset[4];				// 0x148-0x14e -32768 = -10.0v; +32767 = +9.9997v
  volatile unsigned short unused7[8];
  volatile struct {                                     // 0x160-0x1de
    volatile unsigned short src;			// +0x00 synthesizer/reset source 0-3 local, 4-7 slave from bus
    volatile unsigned short div;                        // +0x02 divisor -- used for gear simulation
    volatile unsigned short ptr;                        // +0x04 waveform memory pointer -- to 64k word waveform memory
    volatile unsigned short wav;                        // +0x06 waveform memory data
    volatile unsigned short siz;                        // +0x08 waveform block size -- 2^(6+n)
    volatile unsigned short bas;                        // +0x0a waveform start location
    volatile unsigned short pha;                        // +0x0c phase rotation -- 360 / blksize
    volatile unsigned short flt;                        // +0c0e filter control
    volatile unsigned short jmp;                        // +0x10 jump control
    volatile unsigned short trg;                        // +0x12 jump target
    volatile unsigned short jph;                        // +0x14 jump phase
    volatile unsigned short jba;                        // +0x16 jump base
    volatile unsigned short snp;                        // +0x18 phase snapshot
    volatile unsigned short bcc;                        // +0x1a burst control
    volatile unsigned short bcx;                        // +0x1c burst count
    volatile unsigned short sum;                        // +0x1e analog summing control
  } chnl[4];
  volatile unsigned short unused8[11]; 			// 0x1e0-0x1f4
  volatile struct {					// 0x1f6 diagnostic pass counter (longword)
    volatile unsigned short hi;
    volatile unsigned short lo;
  } tpass;
  volatile struct {					// 0x1fa diagnostic error count (longword)
    volatile unsigned short hi;
    volatile unsigned short lo;
  } terror;
  volatile unsigned short tstop;                        // 0x1fe diagnostic control word
} V375_REGS;

#endif
