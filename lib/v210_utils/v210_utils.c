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
// Date: Fri Mar 04 13:22:46 2022

#include <sys/types.h>
#include <sys/stat.h>

#include <ctype.h>
#include <endian.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// prototypes

#include "v210_utils_proto.h"     

// structure defs

#include "v210_regs_sdef.h"

#define DEBUG  1                // printf errors while executing library

struct timespec sleeptime = {0, 10000000}; // sleep for 10 msec for relays to settle before reading

typedef union {
  uint64_t mask;
#if __BYTE_ORDER == __LITTLE_ENDIAN
  struct {
    uint16_t reg3;
    uint16_t reg2;
    uint16_t reg1;
    uint16_t reg0;
  } ctl;
#else
  struct {
    uint16_t reg0;
    uint16_t reg1;
    uint16_t reg2;
    uint16_t reg3;
  } ctl;
#endif
} RELAY_SDEF;

RELAY_SDEF relay = {.mask = 0x0L};


unsigned short v210_read_bdid(V210_REGS *pb)
{
  return(pb->bdid);
}


unsigned short v210_read_csr(V210_REGS *pb)
{
  return(pb->csr);
}


unsigned short v210_write_csr(V210_REGS *pb, unsigned short mask)
{
  pb->csr = mask;
  return(pb->csr);
}


unsigned short v210_read_vximfr(V210_REGS *pb)
{
  return(pb->vximfr);
}


unsigned short v210_read_vxitype(V210_REGS *pb)
{
  return(pb->vxitype);
}


char v210_read_fpgarev(V210_REGS *pb)
{
  return((char)(pb->fpgarev & 0xff));
}


void v210_turn_off_error_led(V210_REGS *pb)
{
  pb->csr |= V210_CSR_ERR;
  return;
}


void v210_turn_on_error_led(V210_REGS *pb)
{
  pb->csr &= ~V210_CSR_ERR;
  return;
}

// -----------------------------------------------------------------------------------

// connect relays to P3/P4 external connectors

void v210_connect_relays(V210_REGS *pb)
{
  pb->csr |= V210_CSR_ENABLE_RELAYS;
  return;
}


// disconnect relays to P3/P4 external connectors

void v210_disconnect_relays(V210_REGS *pb)
{
  pb->csr &= ~V210_CSR_ENABLE_RELAYS;
  return;
}


// set relay bit in internal 64-bit mask

int v210_set_bit_in_mask(V210_REGS *pb, int chnl)
{
  if ((chnl > V210_CHNLS_PER_BOARD) || (chnl < 0))
    return(-1);

  relay.mask |= (1L << chnl);
  return(0);
}


// clear relay bit in internal 64-bit mask

int v210_clr_bit_in_mask(V210_REGS *pb, int chnl)
{
  if ((chnl > V210_CHNLS_PER_BOARD) || (chnl < 0))
    return(-1);

  relay.mask &= ~(1L << chnl);
  return(0);
}


// read bit status in internal 64-bit mask

int v210_read_bit_in_mask(V210_REGS *pb, int chnl)
{
  if ((chnl > V210_CHNLS_PER_BOARD) || (chnl < 0))
    return(-1);

  if (relay.mask & (uint64_t)(1L << chnl))
    return(1);
  else
    return(0);
}


// write contents of internal 64-bit mask to registers

void v210_write_relay_regs(V210_REGS *pb)
{
  pb->ctl0 = relay.ctl.reg0;
  pb->ctl1 = relay.ctl.reg1;
  pb->ctl2 = relay.ctl.reg2;
  pb->ctl3 = relay.ctl.reg3;
  return;
}


// read contents of registers into internal 64-bit mask

void v210_read_relay_regs(V210_REGS *pb, int waitflag)
{
  if (waitflag == 1)
    nanosleep(&sleeptime, 0);   // wait 10 msec -- manual says 5.5 msec, but more is needed

  relay.ctl.reg0 = pb->rcon0;
  relay.ctl.reg1 = pb->rcon1;
  relay.ctl.reg2 = pb->rcon2;
  relay.ctl.reg3 = pb->rcon3;
  return;
}


// clear all relays -- write 0L to internal 64-bit mask and write to registers

void v210_clr_all_relays(V210_REGS *pb)
{
  relay.mask = 0L;
  v210_write_relay_regs(pb);
  return;
}


// set all 64-bit relays from mask

void v210_set_relays_from_mask64(V210_REGS *pb, uint64_t mask)
{
  relay.mask = mask;
  v210_write_relay_regs(pb);
  return;
}


// load state of all relays into 64-bit mask

void v210_read_relays_into_mask64(V210_REGS *pb, uint64_t *mask, int waitflag)
{
  v210_read_relay_regs(pb, waitflag);
  *mask = relay.mask;
  return;
}
