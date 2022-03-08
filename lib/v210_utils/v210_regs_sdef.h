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
// Date: Fri Mar 04 13:11:07 2022

#ifndef _V210_REGS_SDEF_H_
#define _V210_REGS_SDEF_H_

#define V210_CHNLS_PER_BOARD	64

// csr bits

#define V210_CSR_ERR		0x8000		// turn off error LED
#define V210_CSR_P4TM		0x4000		// enable relays 0-31
#define V210_CSR_P3TM		0x2000		// enable relays 32-63
#define V210_CSR_ENABLE_RELAYS	V210_CSR_P4TM | V210_CSR_P3TM	// enable all relays

// V210 register map

typedef volatile struct
{							//       offset
  volatile unsigned short bdid;				// 0x000 board id -- 6912 (0x1b00)
  volatile unsigned short csr;				// 0x002 controls relay drivers and front panel LED
  volatile unsigned short vximfr;			// 0x004 manufacturer's ID -- should read 0xfeee
  volatile unsigned short vxitype;			// 0x006 module type -- 22210 (0x56c2)
  volatile unsigned short fpgarev;			// 0x008 firmware revision -- 0x0041 ("A")
  volatile unsigned short unused0;			// 0x00a 
  volatile unsigned short unused1;			// 0x00c 
  volatile unsigned short unused2;			// 0x00e
  volatile unsigned short ctl0;		   		// 0x010 controls relays 63-48
  volatile unsigned short ctl1;		   		// 0x012 controls relays 47-32
  volatile unsigned short ctl2;		   		// 0x014 controls relays 31-16
  volatile unsigned short ctl3;				// 0x016 controls relays 15-0 
  volatile unsigned short rcon0;			// 0x018 read the contacts of relays 63-48
  volatile unsigned short rcon1;			// 0x01a read the contacts of relays 47-32
  volatile unsigned short rcon2;			// 0x01c read the contacts of relays 31-16
  volatile unsigned short rcon3;			// 0x01e read the contacts of relays 15-0 
} V210_REGS;

#endif
