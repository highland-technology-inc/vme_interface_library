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
// Date: Sat Aug 07 17:19:59 2021

// read unsigned int from hi/lo regs

extern unsigned int ht_read_interlocked(volatile unsigned short *);

// write unsigned int into hi/lo regs

extern unsigned int ht_write_interlocked(volatile unsigned short *, unsigned int);

// write macro register and wait for completion

extern int ht_write_macro(unsigned short *, unsigned short, unsigned short *, int, int);
