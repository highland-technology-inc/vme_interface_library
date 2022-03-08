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
// Date: Fri Mar 04 14:19:24 2022

// structure def for V120_HANDLE is contained in v120_clt-master/libV120/V120.c
// structure def for VME_REGION is contained in v120_clt-master/include/V120.h

// usage:
//   ./v210 ./v210 -c chnl -F wavefile_name -f freq -d -r -S sleeptime

#include <sys/mman.h>
#include <sys/time.h>

#include <endian.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <V120.h>

// prototypes

#include "deg_conv_proto.h"
#include "get_vaddr_for_device_proto.h"
#include "ht_utils_proto.h"
#include "misc_math_utils_proto.h"
#include "v210_utils_proto.h"

uint64_t mask;


int main(int argc, char *argv[])
{
  V210_REGS *pb;
  char *whoami = argv[0];
  char c;
  char *psetlist = (char *)0;
  char *pclrlist = (char *)0;
  int debug_flag = 0;
  char *p;
  int chnl;

  char usage[] = {"./v210 -s <relaylist|all> -c <relaylist|all> -d\n"};

  while ((c = getopt(argc, argv, "s:c:d\?")) != (char)EOF)
    switch(c)
      {
      case 's':
        psetlist = strdup(optarg);
        break;

      case 'c':
        pclrlist = strdup(optarg);
        break;

      case 'd':
        debug_flag = 1;
        break;
        
      case '\?':
        printf("%s", usage);
        exit(1);

      default:
        printf("ERROR %s/%s: input option/arg error -- optarg = %s\n", whoami, __FUNCTION__, optarg);
        exit(1);
      }

  // map to board

  pb = (V210_REGS *)get_vaddr_for_device("output.conf", "v210[0][0]");

  if (debug_flag)
    {
      printf("bdid: 0x%4.4hx\n", v210_read_bdid(pb));
      printf("csr: 0x%4.4hx\n", v210_read_csr(pb));
      printf("vximfr: 0x%4.4hx\n", v210_read_vximfr(pb));
      printf("vxitype: %hd (0x%4.4hx)\n", v210_read_vxitype(pb), v210_read_vxitype(pb));
      printf("fpgarev: %c\n", v210_read_fpgarev(pb));
    }
  
  // ------------------------------------------------------------------------------------------------------

  // turn off "error" LED
  
  v210_turn_off_error_led(pb);  

  // connect relays to P3/P4 external connectors
  
  v210_connect_relays(pb);

  // clear all relays (sets internal 64-bit mask)

  v210_clr_all_relays(pb);

  // get "set" relays as a comma delimited list (no spaces)
  // for example, set relays 0-3: -s 0,1,2,3

  if (psetlist != (char *)0)
    {
      p = strtok(psetlist, ",");
    
      while (p != (char *)0)
        {
          chnl = atoi(p);
          v210_set_bit_in_mask(pb, chnl); // set bit in internal 64-bit mask
          p = strtok(0, ",");
        }
    }
  
  // drive state of relays from internal 64-bit mask

  v210_write_relay_regs(pb);	
  
  // load all 64 relays state into 64-bit local mask

  v210_read_relays_into_mask64(pb, &mask, 1); 
  printf("mask = 0x%lx\n", mask);

  // get "clear" relays as a comma delimited list (no spaces)
  // for example, clear relays 0-3: -c 0,1,2,3

  if (pclrlist != (char *)0)
    {  
      p = strtok(pclrlist, ",");
    
      while (p != (char *)0)
        {
          chnl = atoi(p);
          v210_clr_bit_in_mask(pb, chnl); // clear bit in internal 64-bit mask
          p = strtok(0, ",");
        }
    }

  // drive state of relays from internal 64-bit mask

  v210_write_relay_regs(pb);

  // load all 64 relays state into 64-bit local mask

  v210_read_relays_into_mask64(pb, &mask, 1);
  printf("mask = 0x%lx\n", mask);

  // flag all relays to be turned on, and then load the internal 64-bit mask to drive relays

  mask = 0xffffffffffffffffL;
  v210_set_relays_from_mask64(pb, mask);
  v210_read_relays_into_mask64(pb, &mask, 1);
  printf("mask = 0x%lx\n", mask);

  // flag all relays to be turned on, and then load the internal 64-bit mask to drive relays

  mask = 0L;
  v210_set_relays_from_mask64(pb, mask);
  v210_read_relays_into_mask64(pb, &mask, 1);
  printf("mask = 0x%lx\n", mask);

  // clear all the relays

  v210_clr_all_relays(pb);

  // disconnect the relays from the P3/P4 connectors
  
  v210_disconnect_relays(pb);

  // return "error" LED to power up condition
  
  v210_turn_on_error_led(pb);

  // free space for arglists

  free(psetlist);
  free(pclrlist);

  return(0);
}
