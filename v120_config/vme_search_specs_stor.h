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
// Date: Sun Aug 08 13:07:46 2021

#ifndef _VME_SEARCH_SPECS_STOR_H_
#define _VME_SEARCH_SPECS_STOR_H_

#ifndef _VME_SEARCH_SPECS_SDEF_H_
#include "vme_search_specs_sdef.h"
#endif

VME_SEARCH_SPECS_SDEF vme_search_specs[] = {
  //										vximfr   vximfr    vxitype  vxitype  board    num
  //                                                                                     offset             offset  spacing  cards
  //                      1         2         3         4         5         6     
  //            0123456789012345678901234567890123456789012345678901234567890123
  { "v210",    "VME Relay Module",						0xfeee,    0x4,    0x56c2,    0x6,    0x20,    0},
  { "v220",    "VME 4-20mA Analog Control I/O Module",				0xfeee,    0x0,    0x56cc,    0x2,   0x200,    0},
  { "v230",    "VME Analog Input Module",					0xfeee,    0x0,    0x56d6,    0x2,   0x200,    0},
  { "v250",    "VME Digital I/O Module",					0xfeee,    0x0,    0x56ea,    0x2,   0x200,    0},
  { "v340",    "VME Waveform Generator Module",					0xfeee,    0x0,    0x5744,    0x2,   0x100,    0},
  { "v344",    "VME Waveform Generator",					0xfeee,    0x0,    0x5648,    0x2,   0x200,    0},
  { "v346",    "VME Waveform Generator",					0xfeee,    0x0,    0x564a,    0x2,   0x200,    0},
  { "v350",    "VME Analog/Digital Function Generator",				0xfeee,    0x0,    0x574e,    0x2,   0x100,    0},
  { "v360",    "VME Tachometer Module",						0xfeee,    0x0,    0x5758,    0x2,    0x40,    0},
  { "v365",    "VME Tachometer / Overspeed Module",				0xfeee,    0x0,    0x575d,    0x2,    0x40,    0},
  { "v370",    "VME Arbitrary Waveform Generator",				0xfeee,    0x0,    0x5762,    0x2,   0x200,    0},
  { "v375",    "VME Arbitrary Waveform Generator",				0xfeee,    0x0,    0x5767,    0x2,   0x200,    0},
  { "v380",    "VME Strain Gauge / Weighing Module",				0xfeee,    0x0,    0x576c,    0x2,   0x200,    0},
  { "v385",    "VME Strain Gauge / Load Cell Module",				0xfeee,    0x0,    0x5771,    0x2,   0x100,    0},
  { "v410",    "VME RTD / Resistance Input Module",				0xfeee,    0x0,    0x578a,    0x2,   0x200,    0},
  { "v420",    "VME Isolated Resistance Simulator",				0xfeee,    0x0,    0x5794,    0x2,   0x100,    0},
  { "v450",    "VME Analog Input and Thermocouple Measurement Module",		0xfeee,    0x0,    0x57b2,    0x2,   0x200,    0},
  { "v460",    "VME Analog Scanner Module",					0xfeee,    0x0,    0x57bc,    0x2,   0x100,    0},
  { "v470",    "VME Analog Output and Thermocouple Simulator Module",		0xfeee,    0x0,    0x57c6,    0x2,   0x200,    0},
  { "v490",    "VME Multi-range Digitizer",					0xfeee,    0x0,    0x57da,    0x2,   0x200,    0},
  { "v545",    "VME Synchro/Resolver/LVDT/RVDT Simulation/Acquisition Module",	0xfeee,    0x0,    0x5811,    0x2,   0x800,    0},
};

#endif
