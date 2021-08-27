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
// Date: Sun Aug 08 13:08:02 2021

#ifndef _VME_SPECS_SDEF_H_
#define _VME_SPECS_SDEF_H_

#define DEVNAME_LENGTH 32
#define DEVDESC_LENGTH 64

typedef volatile struct
{
  char devname[DEVNAME_LENGTH];
  char devdesc[DEVDESC_LENGTH];
  unsigned short vxitype;
  unsigned short devnum;
  unsigned int vmeaddr;
  void *pbase_vaddr;
  int online;
} VME_SPECS_SDEF;

#endif
