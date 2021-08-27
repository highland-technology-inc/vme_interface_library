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
