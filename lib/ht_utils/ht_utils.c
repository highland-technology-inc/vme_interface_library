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
// Date: Sat Aug 07 17:20:20 2021

#include <endian.h> // required for __BYTE_ORDER and __LITTLE_ENDIAN
#include <unistd.h>

// From V450 manual, section 5.8 Register Map:
// 32-bit data items are atomically interlocked and must be read in MS:LS word order.
// The MS word of 32-bit data may be read anytime.

// pms = pointer to most significant 16 bits
// pls = pointer to least significant 16 bits

unsigned int ht_read_interlocked(volatile unsigned short *phi)
{
  static union
  {
    unsigned int ui;
    unsigned short us[2];
  } data;

  volatile unsigned short *plo = phi + 1;
  
  // must read in hi/lo order
  
#if __BYTE_ORDER == __LITTLE_ENDIAN 
  data.us[1] = *phi;            
  data.us[0] = *plo;
#else
  data.us[0] = *phi;
  data.us[1] = *plo;
#endif

  return(data.ui);
}


unsigned int ht_write_interlocked(volatile unsigned short *phi, unsigned int value)
{
  static union
  {
    unsigned int ui;
    unsigned short us[2];
  } data;

  volatile unsigned short *plo = phi + 1;
  
  data.ui = value;

  // must write in hi/lo order
  
#if __BYTE_ORDER == __LITTLE_ENDIAN 
  *phi = data.us[1];
  *plo = data.us[0];
#else
  *phi = data.us[0];
  *plo = data.us[1];
#endif

  return(0);
}

 
// Steps to execute a macro
// 1) Verify that the MS bit (bit 15) of the MACRO register is clear, indicating that the card is ready to accept a command.
// 2) If required, load param_count param variables into the PARAM0..PARAMx registers.
// 3) Write a 16-bit macro code to MACRO.
// 4) Wait until the MS bit again clears.  If MACRO is non-zero, an error has occurred.

int ht_write_macro(unsigned short *pmacro, unsigned short macro, unsigned short *param, int param_count, int reset_flag)
{
  int i;
  unsigned short *param_array = param;
  
  if ((*pmacro & 0x8000) != 0)
    return(-1);
  
  if (param_count > 2)
    return(-1);

  if ((param_count > 0) && (param == (unsigned short *)0))
    return(-1);

  if (param_count > 0)
    for (i = 0; i <= 2; i++)
      *(pmacro + 1 + i) = *param_array++;

  *pmacro = macro;

  // for a reset, hard, or soft reboot; return immediately and let caller sleep for appropriate time for device to be seen again on the bus
  
  if (reset_flag == 1)
    return(0);
  
  while ((*pmacro & 0x8000) != 0)
    usleep(10000);              // sleep 10 msec

  usleep(50000);                // need to sleep for 50 msec

  if (*pmacro != 0)
    return(-1);
  else
    return(0);
}


