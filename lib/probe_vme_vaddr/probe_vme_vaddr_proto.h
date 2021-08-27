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
// Date: Sun Aug 08 10:44:36 2021

#ifndef _PROBE_VME_ADDR_PROTO_H_
#define _PROBE_VME_ADDR_PROTO_H_

#ifndef V120_H_INCLUDED
#include "V120.h"
#endif

extern int probe_vme_vaddr(V120_CONFIG *, void *, unsigned int, void *);

#endif
