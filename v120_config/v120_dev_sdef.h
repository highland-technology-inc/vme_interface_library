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
// Date: Sun Aug 08 13:07:04 2021

#ifndef _V120_DEV_SDEF_H_
#define _V120_DEV_SDEF_H_

#ifndef _VME_SPECS_SDEF_H_
#include "vme_specs_sdef.h"	// needed for MAX_CARDS_IN_CHASSIS
#endif

// V120.c (included as a library) defines the V120_HANDLE structure as:

// struct V120_HANDLE {
//   struct VME_REGION               *list_head;	// linked list of VME cards mapped (not same as mmlist, which takes care of mmap management)
//   int                             fd_ctrl;		// file descriptor of v120_c(n) device (control region)
//   int                             fd_vme;		// file descriptor of v120_v(n) device (VME pages)
//   int                             fd_irq;		// file descriptor of v120_q(n) device (IRQ )
//   V120_CONFIG                     *config;		// pointer to V120 config regions (PCIe memory)
//   V120_PD                         *pd;		// pointer to V120 page descriptor registers (PCIe memory)
//   V120_IRQ                        *irqregs;		// pointer to V120 IRQ registers (PCIe memory)
//   struct V120_MONITOR_BLOCK       *mon_blk;		// pointer to V120's VME monitor block (PCIe memory)
//   struct V120_PCIE_RECORD_BLOCK   *record_blk;	// pointer to V120's PCIe record block (PCIe memory)
//   void                            *mapptr;		// pointer to memory-mapped base of V120's control region
//   int                             errnum;		// saved value of errno for this device
//   struct vme_mmlist_t             *mmlist;		// pointer to head of linked list of memory-mapped areas
//   unsigned long                   flag;		// TODO: meaning of this
//   unsigned int                    crateno;		// Crate number of V120; set by either v120_next or call to v120_open
// };


// V120.h includes v120_uapi.h that defines V120_CONFIG, and v120 registers
// V120.h defines V120_PD as uint64_t

// V120.h defines VME_REGION
//
// typedef struct VME_REGION {
//     struct VME_REGION *next;
//     void              *base;
//     unsigned int      start_page;
//     unsigned int      end_page;
//     uint64_t          vme_addr;
//     size_t            len;
//     V120_PD           config;
//     const char        *tag;
//     void              *udata;
// } VME_REGION;
// 

#ifndef V120_H_INCLUDED
#include <V120.h>	        // needed for VME_REGION
#endif

#define MAX_DEV 4
#define MAX_TAG_LENGTH 32
#define MAX_CARDS_IN_CHASSIS 20

typedef volatile struct {
  int crate_num;
  int online;
  V120_HANDLE *crate_handle;
  V120_CONFIG *crate_regs;
  VME_SPECS_SDEF cards[MAX_CARDS_IN_CHASSIS];
  int last_card_index;
  VME_REGION a16;
  char a16_tag[MAX_TAG_LENGTH];
  VME_REGION a24;
  char a24_tag[MAX_TAG_LENGTH];
  VME_REGION a32;
  char a32_tag[MAX_TAG_LENGTH];
} V120_DEV;

#endif
