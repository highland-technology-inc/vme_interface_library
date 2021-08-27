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
// Date: Sun Aug 08 10:43:57 2021

#include <unistd.h>
#include <values.h>

#ifndef V120_H_INCLUDED
#include "V120.h"
#endif

#define VME_ACC_TIMER_MASK 0xffff0000
#define VME_ACC_TIMER_RSHIFT 16
#define VME_ACC_RESULT_MASK 0x1f
#define VME_ACC_AF	0x00000010
#define VME_ACC_BTO	0x00000008
#define VME_ACC_RETRY	0x00000004
#define VME_ACC_BERR	0x00000002
#define VME_ACC_DTACK	0x00000001


int probe_vme_vaddr(V120_CONFIG *crate_regs, void *vaddr, unsigned int data_width, void *write_back)
{
  union
  {
    unsigned char uc;
    unsigned short us;
    unsigned int ui;
  } __attribute__((unused)) data;

  unsigned int vmeacc;

  switch(data_width)
    {
    case CHARBITS:
      data.uc = *(unsigned char *)vaddr;
      break;
    case SHORTBITS:
      data.us = *(unsigned short *)vaddr;
      break;
    case INTBITS:
      data.ui = *(unsigned int *)vaddr;
      break;
    }
  
  usleep(60);

  vmeacc = crate_regs->vme_acc;

  if ((vmeacc & VME_ACC_RESULT_MASK) == VME_ACC_DTACK)
    {
      if (write_back != (void *)0)
        {
          switch(data_width)
            {
            case CHARBITS:
              *(unsigned char *)write_back = data.uc;
              break;
            case SHORTBITS:
              *(unsigned short *)write_back = data.us;
              break;
            case INTBITS:
              *(unsigned int *)write_back = data.ui;
              break;
            }
        }
      return(1);
    }
  else
    return(0);
}
