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
// Date: Sat Aug 07 17:01:17 2021

#include <stdlib.h>
#include <string.h>

// structure defs

#include <V120.h>

// prototypes

#include "parse_args_proto.h"
#include "parse_options_proto.h"
#include "read_line_proto.h"

#define MAX_ARG_V 64
#define MAX_CMD_LINE 1024
#define MAX_FN_LENGTH 256

// configure VME cards

#define VME_SET_VME_BASE_ADDR		0x0000000000000001UL
#define VME_SET_REPEATER_NUM		0x0000000000000002UL
#define VME_SET_ADDR_SPACE		0x0000000000000004UL
#define VME_SET_ONLINE_FLAG		0x0000000000000008UL

#define REPEATER_SET_A16_ADDR		0x0000000000000001UL
#define REPEATER_SET_A16_SIZE		0x0000000000000002UL
#define REPEATER_SET_A16_STARTPAGE	0x0000000000000004UL
#define REPEATER_SET_A16_ENDPAGE	0x0000000000000008UL
#define REPEATER_SET_A24_ADDR		0x0000000000000010UL
#define REPEATER_SET_A24_SIZE		0x0000000000000020UL
#define REPEATER_SET_A24_STARTPAGE	0x0000000000000040UL
#define REPEATER_SET_A24_ENDPAGE	0x0000000000000080UL
#define REPEATER_SET_A32_ADDR		0x0000000000000100UL
#define REPEATER_SET_A32_SIZE		0x0000000000000200UL
#define REPEATER_SET_A32_STARTPAGE	0x0000000000000400UL
#define REPEATER_SET_A32_ENDPAGE	0x0000000000000800UL
#define REPEATER_SET_ONLINE_FLAG	0x0000000000001000UL

static void load_repeater_board_num(char *, int *, int *);

void *get_vaddr_for_device(char *conf_file, char *board_spec)
{
  char dev_spec[32];
  FILE *fpi;
  int arg_c;
  char *arg_v[MAX_ARG_V];
  char cmdline[MAX_CMD_LINE] = {""};
  char parsed_cmdline[MAX_CMD_LINE] = {""};
  int __attribute__((unused)) num_chars;
  V120_HANDLE *v120_handle;
  void *a16_base_vaddr, *a24_base_vaddr, *a32_base_vaddr;
  static void *vme_base_vaddr;

  static unsigned int vme_base_addr;
  static int vme_addr_space;
  static int vme_online_flag;
  static unsigned long vme_chgflg;

  int repeater_num, board_num;

  static OPTION_TABLE vme_option_table[] = {
    // opt_str		arg_str		arg_value	data				type_mask	routine			chgflg_mask             
    {"-base",		"*",		0,		(void *)&vme_base_addr,		T_UINT,		load_constant,		VME_SET_VME_BASE_ADDR},
    {"-a",		"*",		0,		(void *)&vme_addr_space,	T_INT,		load_constant,		VME_SET_ADDR_SPACE},
    {"-o",		"true",		1,		(void *)&vme_online_flag,	T_INT,		load_enum,		VME_SET_ONLINE_FLAG},
    {"-o",		"false",	0,		(void *)&vme_online_flag,	T_INT,		load_enum,		VME_SET_ONLINE_FLAG},
    {"",		"",		0,		(void *)0,			0,              0,                      0}
  };

  static unsigned int a16_addr;
  static unsigned int a16_size;
  static unsigned int a16_startpage;
  static unsigned int a16_endpage;
  static unsigned int a24_addr;
  static unsigned int a24_size;
  static unsigned int a24_startpage;
  static unsigned int a24_endpage;
  static unsigned int a32_addr;
  static unsigned int a32_size;
  static unsigned int a32_startpage;
  static unsigned int a32_endpage;
  static unsigned long repeater_online_flag;
  static unsigned long repeater_chgflg;

  static OPTION_TABLE repeater_option_table[] = {
    // opt_str		arg_str		arg_value	data				type_mask	routine			chgflg_mask             
    {"-a16",		"*",		0,		(void *)&a16_addr,	        T_UINT,		load_constant,		REPEATER_SET_A16_ADDR},      
    {"-s16",		"*",		0,		(void *)&a16_size,	        T_UINT,		load_constant,		REPEATER_SET_A16_SIZE},      
    {"-S16",		"*",		0,		(void *)0,			0,		noop,			0},
    {"-sp16",		"*",		0,		(void *)&a16_startpage,	        T_UINT,		load_constant,		REPEATER_SET_A16_STARTPAGE}, 
    {"-ep16",		"*",		0,		(void *)&a16_endpage,	        T_UINT,		load_constant,		REPEATER_SET_A16_ENDPAGE},   
    {"-a24",		"*",		0,		(void *)&a24_addr,	        T_UINT,		load_constant,		REPEATER_SET_A24_ADDR},      
    {"-s24",		"*",		0,		(void *)&a24_size,	        T_UINT,		load_constant,		REPEATER_SET_A24_SIZE},      
    {"-S24",		"*",		0,		(void *)0,			0,		noop,			0},
    {"-sp24",		"*",		0,		(void *)&a24_startpage,	        T_UINT,		load_constant,		REPEATER_SET_A24_STARTPAGE}, 
    {"-ep24",		"*",		0,		(void *)&a24_endpage,	        T_UINT,		load_constant,		REPEATER_SET_A24_ENDPAGE},   
    {"-a32",		"*",		0,		(void *)&a32_addr,	        T_UINT,		load_constant,		REPEATER_SET_A32_ADDR},      
    {"-s32",		"*",		0,		(void *)&a32_size,	        T_UINT,		load_constant,		REPEATER_SET_A32_SIZE},      
    {"-S32",		"*",		0,		(void *)0,			0,		noop,			0},
    {"-sp32",		"*",		0,		(void *)&a32_startpage,	        T_UINT,		load_constant,		REPEATER_SET_A32_STARTPAGE}, 
    {"-ep32",		"*",		0,		(void *)&a32_endpage,	        T_UINT,		load_constant,		REPEATER_SET_A32_ENDPAGE},   
    {"-o",		"true",		1,		(void *)&repeater_online_flag,	T_INT,		load_enum,		REPEATER_SET_ONLINE_FLAG},   
    {"-o",		"false",	1,		(void *)&repeater_online_flag,	T_INT,		load_enum,		REPEATER_SET_ONLINE_FLAG},   
  };

  vme_base_vaddr = (void *)0;

  load_repeater_board_num(board_spec, &repeater_num, &board_num);

  // open input configuration file 

  if ((fpi = fopen(conf_file, "r")) == (FILE *)0)
    {
      printf("FATAL: cannot open input configuration file: %s\n", conf_file);
      return((void *)-1);
    }
    
  while (((num_chars = read_line(cmdline, MAX_CMD_LINE, fpi)) > 0) && (vme_base_vaddr == (void *)0))
    {
      arg_c = parse_args(cmdline, arg_v, MAX_ARG_V, parsed_cmdline, MAX_CMD_LINE);
  
      if ((strcmp(arg_v[0], "vmedevice") == 0) && (strcmp(arg_v[1], board_spec) == 0))
        {
          // parse command line options/args -- load vme_base_addr, vme_addr_space, vme_online_flag

          parse_options(arg_c - 2, &arg_v[2], vme_option_table, &vme_chgflg);

          // if (vme_online_flag == 1)
              
          sprintf(dev_spec, "v120[%d]", repeater_num);

          rewind(fpi);

          while (((num_chars = read_line(cmdline, MAX_CMD_LINE, fpi)) > 0) && (vme_base_vaddr == (void *)0))
            {
              arg_c = parse_args(cmdline, arg_v, MAX_ARG_V, parsed_cmdline, MAX_CMD_LINE);

              if ((strcmp(arg_v[0], "repeater") == 0) && (strcmp(arg_v[1], dev_spec) == 0))
                {
                  // parse command line options/args -- load aXX_addr, aXX_size, aXX_startpage, aXX_endpage, repeater_online_flag

                  parse_options(arg_c - 2, &arg_v[2], repeater_option_table, &repeater_chgflg);
                  
                  // if (repeater_online_flag == 1)

                  v120_handle = v120_open(repeater_num);                  

                  switch(vme_addr_space)
                    {
                    case 16:
                      a16_base_vaddr = v120_get_vme(v120_handle, a16_startpage, a16_endpage);
                      vme_base_vaddr = (void *)(a16_base_vaddr + vme_base_addr - a16_addr);
                      break;

                    case 24:
                      a24_base_vaddr = v120_get_vme(v120_handle, a24_startpage, a24_endpage);
                      vme_base_vaddr = (void *)(a24_base_vaddr + vme_base_addr - a24_addr);
                      break;

                    case 32:
                      a32_base_vaddr = v120_get_vme(v120_handle, a32_startpage, a32_endpage);
                      vme_base_vaddr = (void *)(a32_base_vaddr + vme_base_addr - a32_addr);
                      break;

                    default:
                      break;
                    }
                }                
            }
        }
    }

  // close inout configuration file

  if (fpi != (FILE *)0)
    fclose(fpi);

  return(vme_base_vaddr);
}


static void load_repeater_board_num(char *board_spec, int *pcrate_num, int *pboard_num)
{
  char *ps = strdup(board_spec);
  char *p, *q;
  char __attribute__((unused)) *pname;

  p = ps;
  pname = strtok_r(p, "[]", &q);
  p = q;
  *pcrate_num = atoi(strtok_r(p, "[]", &q));
  p = q;
  *pboard_num = atoi(strtok_r(p, "[]", &q));
  free(ps);
  return;
}


