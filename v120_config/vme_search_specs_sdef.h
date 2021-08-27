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
// Date: Sun Aug 08 13:07:24 2021

#ifndef _VME_SEARCH_SPECS_SDEF_H_
#define _VME_SEARCH_SPECS_SDEF_H_

#ifndef _VME_SPECS_SDEF_H_
#include "vme_specs_sdef.h"     // needed for DEVNAME_LENGTH and DEVDESC_LENGTH
#endif

typedef volatile struct
{
  char devname[DEVNAME_LENGTH];
  char devdesc[DEVDESC_LENGTH];
  unsigned short vximfr;
  unsigned short vximfr_offset;
  unsigned short vxitype;
  unsigned short vxitype_offset;
  unsigned short device_spacing;
  unsigned short num_cards_of_same_type;
} VME_SEARCH_SPECS_SDEF;

#endif
