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
// Date: Sun Aug 08 13:05:55 2021

#include <stdlib.h>
#include <string.h>
#include <values.h>

#include <V120.h>

// structure defs

#include "v120_dev_sdef.h"
#include "vme_search_specs_stor.h"
#include "vme_specs_sdef.h"

// prototypes

#include "parse_args_proto.h"
#include "parse_options_proto.h"
#include "probe_vme_vaddr_proto.h"
#include "read_line_proto.h"

// used in call for v120_scan_for_vme_cards

#define MIN_ADDR_STEP 0x20
#define MAX_CARDS_IN_CHASSIS 20

#define OFFLINE_SPACE	0x0
#define A16_SPACE	0x1
#define A24_SPACE	0x2
#define A32_SPACE	0x4
#define ALL_SPACE	A16_SPACE | A24_SPACE | A32_SPACE
#define A16A24_SPACE	A16_SPACE | A24_SPACE

#define MAX_ARG_V 32
#define MAX_CMD_LINE 1024
#define MAX_FN_LENGTH 256

// #define DEBUG

static char *whoami;

static char input_config_fn[MAX_FN_LENGTH];
static char output_config_fn[MAX_FN_LENGTH];

V120_DEV v120_dev[MAX_DEV];

// local routines

void print_v120_dev(V120_DEV *);
int v120_scan_for_vme_cards(V120_DEV *, int, FILE *);
int config_repeater(int, char *[], FILE *, FILE *);
int v120_setup_memory_regions(V120_DEV *);
int config_vmedevice(int, char *[], FILE *, FILE *);
int find_vme_addr_space(V120_DEV *, unsigned int, int *);
void *probe_vme_addr_in_mapped_range(V120_DEV *, unsigned int, int);
VME_SPECS_SDEF *load_card_info(V120_DEV *, void *, char *, unsigned int, unsigned int);
int check_for_vmedevice_in_config(char *);
char *print_online(int);
int get_addr_space(int);
char *print_vme_speed(int);

// local versions of parameters for option_table

static int scan_flag = 0;
static unsigned long chgflg;

static char *help_text[] = {
  "v120_conf options:",
  "-help repeater           # print v120 repeater input file format",
  "-help vmedevice          # print vmedevice input file format",
  "-help                    # print this text",
  "-i input_config_fn       # specify the input v120/vme configuration file",
  "-o output_config_fn      # probe of devices listed in input_config_fn and optionally autoscan VME devices",
  "-q                       # set quiet flag",
  "-debug                   # set debug flag",
  " ",
  "# scan option with 'vmedevice' entries in the input_config_fn are mutually exclusive",
  "-scan all                # scan A16|A24|A32 regions",
  "-scan a16                # scan A16 region",
  "-scan a24                # scan A24 region",
  "-scan a32                # scan A32 region",
  "-scan a16a24             # scan A16|A24 regions",
  "-scan                    # same as '-scan all'",
  " ",
  ""};

static char *help_text_repeater[] = {
  "repeater v120[crate_num]    # specified which v120 device 0-3",
  "    -a16 address            # base address for A16 region",
  "    -s16 size               # size of A16 region",
  "   [-S16 SLOW|MED|FAST|MAX] # specifies VME cycle speed for A16 access",
  "    -a24 address            # base address for A24 region",
  "    -s24 size               # size of A24 region",
  "   [-S24 SLOW|MED|FAST|MAX] # specifies VME cycle speed for A24 access",
  "    -a32 address            # base address for A32 region",
  "    -s32 size               # size of A32 region",
  "   [-S32 SLOW|MED|FAST|MAX] # specifies VME cycle speed for A32 access",
  " ",
  ""};

static char *help_text_vmedevice[] = {
  "vmedevice vmedevice[crate_num][board_num]   # specified board type (e.g. v545, v450, v470, ...) and board number",
  "    -base vme_addr                          # base VME address of specified board",
  " ",
  ""};

#define V120_CONFIG_REPEATER_HELP	0x0000000000000001UL
#define V120_CONFIG_VMEDEVICE_HELP	0x0000000000000002UL
#define V120_CONFIG_HELP		0x0000000000000004UL
#define V120_CONFIG_INPUT_CONFIG_FN	0x0000000000000008UL
#define V120_CONFIG_OUTPUT_CONFIG_FN	0x0000000000000010UL
#define V120_CONFIG_QUIET_FLAG		0x0000000000000020UL
#define V120_CONFIG_DEBUG_FLAG		0x0000000000000040UL
#define V120_CONFIG_SCAN_FLAG		0x0000000000000080UL

static OPTION_TABLE option_table[] = {
// opt_str	arg_str		arg_value	data				type_mask	routine			chgflg_mask             
  {"-help",     "repeater",	0,              (void *)help_text_repeater,	0,              print_help_text,        V120_CONFIG_REPEATER_HELP},
  {"-help",     "vmedevice",	0,              (void *)help_text_vmedevice,	0,              print_help_text,        V120_CONFIG_VMEDEVICE_HELP},
  {"-help",     "",		0,              (void *)help_text,          	0,              print_help_text,        V120_CONFIG_HELP},
  {"-i",     	"*",		MAX_FN_LENGTH,	(void *)input_config_fn,	T_STRING,	load_string,		V120_CONFIG_INPUT_CONFIG_FN}, // input repeater file
  {"-o",     	"*",		MAX_FN_LENGTH,	(void *)output_config_fn,	T_STRING,	load_string,		V120_CONFIG_OUTPUT_CONFIG_FN},
  {"-q",     	"",		0,		(void *)0,			0,		set_chgflg,		V120_CONFIG_QUIET_FLAG},
  {"-debug",   	"",		0,		(void *)0,			0,		set_chgflg,		V120_CONFIG_DEBUG_FLAG},
  {"-scan",   	"all",		ALL_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG},
  {"-scan",   	"a16",		A16_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG},
  {"-scan",   	"a24",		A24_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG},
  {"-scan",   	"a32",		A32_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG},
  {"-scan",   	"a16a24",	A16A24_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG},
  {"-scan",   	"",		ALL_SPACE,	(void *)&scan_flag,		T_INT,		load_enum,		V120_CONFIG_SCAN_FLAG}, // testing for null arg must be last test
  {"",          "",		0,		(void *)0,			0,              0,                      0}
};

// local versions of parameters for v120_conf_file_option_table

int main(int argc, char *argv[])
{
  int i;
  char cmdline[MAX_CMD_LINE] = {""};
  char parsed_cmdline[MAX_CMD_LINE] = {""};
  int arg_c;
  char *arg_v[MAX_ARG_V];
  FILE *fpi, *fpo;
  int num_chars = 0;

  whoami = argv[0];

  // initialize v120 dev num to -1

  memset((void *)v120_dev, 0, sizeof(V120_DEV) * MAX_DEV);
  for (i = 0; i < MAX_DEV; i++)
    v120_dev[i].crate_num = -1;

  // parse command line options/args

  parse_options(argc - 1, &argv[1], option_table, &chgflg);

  // if v120_config -help

  if (chgflg & (V120_CONFIG_REPEATER_HELP | V120_CONFIG_VMEDEVICE_HELP | V120_CONFIG_HELP))
    return(0);

  // check for vmedevice lines in input_conf_fn when -scan option is used

  if (chgflg & V120_CONFIG_SCAN_FLAG)
    if (check_for_vmedevice_in_config(input_config_fn) == 1)
      {
        printf("FATAL %s/%s: cannot have \"vmedevice\" lines defined in input configuration file: %s, with -scan option\n", whoami, __FUNCTION__, input_config_fn);
        exit(-1);
      }

  // open input configuration file 

  if (chgflg & V120_CONFIG_INPUT_CONFIG_FN)
    {
      if ((fpi = fopen(input_config_fn, "r")) == (FILE *)0)
        {
          printf("FATAL %s/%s: cannot open input configuration file: %s\n", whoami, __FUNCTION__, input_config_fn);
          exit(-1);
        }
    }
  else
    {
      printf("FATAL %s/%s: input configuration file needs to be specified\n", whoami, __FUNCTION__);
      exit(-1);
    }
  
  // open output configuration file

  if (chgflg & V120_CONFIG_OUTPUT_CONFIG_FN)
    {
      if ((fpo = fopen(output_config_fn, "w")) == (FILE *)0)
        {
          printf("FATAL %s/%s: cannot open output configuration file: %s\n", whoami, __FUNCTION__, output_config_fn);
          exit(-1);
        }
    }
  else
    {
      printf("FATAL %s/%s: output configuration file needs to be specified\n", whoami, __FUNCTION__);
      exit(-1);
    }

  // read options/args in input_config_fn file, and write probe results to output_config_fn file

  while ((num_chars = read_line(cmdline, MAX_CMD_LINE, fpi)) > 0)
    {
      arg_c = parse_args(cmdline, arg_v, MAX_ARG_V, parsed_cmdline, MAX_CMD_LINE);

      if (strcmp(arg_v[0], "repeater") == 0)
        config_repeater(arg_c, &arg_v[0], fpi, fpo);
      else if (strcmp(arg_v[0], "vmedevice") == 0)
        config_vmedevice(arg_c, &arg_v[0], fpi, fpo);
    }
  
  // check for optional scan of VME cards

  if (chgflg & V120_CONFIG_SCAN_FLAG)
    v120_scan_for_vme_cards(v120_dev, scan_flag, fpo);

  // print out V120_DEV structure

  if (chgflg & V120_CONFIG_DEBUG_FLAG)
    for (i = 0; i < MAX_DEV; i++)
      print_v120_dev(&v120_dev[i]);

  // close input_config_fn file

  if (fpi != (FILE *)0)
    fclose(fpi);
  
  // close output_config_fn file

  if (fpo != (FILE *)0)
    fclose(fpo);

  exit(0);
}


void print_v120_dev(V120_DEV *dev)
{
  if (dev->crate_num >= 0)
    {
      if (dev->crate_num == 0)
        printf("\n");
      
      printf("------------------------------------------------------------------\n\n");
      printf("v120_dev[%d].crate_num = %d\n", dev->crate_num, dev->crate_num);
      printf("v120_dev[%d].online = %d\n\n", dev->crate_num, dev->online);

      printf("v120_dev[%d].a16.base = %p\n", dev->crate_num, dev->a16.base);
      printf("v120_dev[%d].a16.start_page = 0x%x\n", dev->crate_num, dev->a16.start_page);
      printf("v120_dev[%d].a16.end_page = 0x%x\n", dev->crate_num, dev->a16.end_page);
      printf("v120_dev[%d].a16.vme_addr = 0x%lx\n", dev->crate_num, (unsigned long)dev->a16.vme_addr);
      printf("v120_dev[%d].a16.len = 0x%x\n", dev->crate_num, (unsigned int)dev->a16.len);
      printf("v120_dev[%d].a16.config = 0x%x\n", dev->crate_num, (unsigned int)dev->a16.config);
      printf("v120_dev[%d].a16.tag = %s\n", dev->crate_num, dev->a16.tag);
      printf("v120_dev[%d].a16.udata = 0x%lx\n\n", dev->crate_num, (unsigned long)dev->a16.udata);

      printf("v120_dev[%d].a24.base = %p\n", dev->crate_num, dev->a24.base);
      printf("v120_dev[%d].a24.start_page = 0x%x\n", dev->crate_num, dev->a24.start_page);
      printf("v120_dev[%d].a24.end_page = 0x%x\n", dev->crate_num, dev->a24.end_page);
      printf("v120_dev[%d].a24.vme_addr = 0x%lx\n", dev->crate_num, (unsigned long)dev->a24.vme_addr);
      printf("v120_dev[%d].a24.len = 0x%x\n", dev->crate_num, (unsigned int)dev->a24.len);
      printf("v120_dev[%d].a24.config = 0x%x\n", dev->crate_num, (unsigned int)dev->a24.config);
      printf("v120_dev[%d].a24.tag = %s\n", dev->crate_num, dev->a24.tag);
      printf("v120_dev[%d].a24.udata = 0x%lx\n\n", dev->crate_num, (unsigned long)dev->a24.udata);

      printf("v120_dev[%d].a32.base = %p\n", dev->crate_num, dev->a32.base);
      printf("v120_dev[%d].a32.start_page = 0x%x\n", dev->crate_num, dev->a32.start_page);
      printf("v120_dev[%d].a32.end_page = 0x%x\n", dev->crate_num, dev->a32.end_page);
      printf("v120_dev[%d].a32.vme_addr = 0x%lx\n", dev->crate_num, (unsigned long)dev->a32.vme_addr);
      printf("v120_dev[%d].a32.len = 0x%x\n", dev->crate_num, (unsigned int)dev->a32.len);
      printf("v120_dev[%d].a32.config = 0x%x\n", dev->crate_num, (unsigned int)dev->a32.config);
      printf("v120_dev[%d].a32.tag = %s\n", dev->crate_num, dev->a32.tag);
      printf("v120_dev[%d].a32.udata = 0x%lx\n\n", dev->crate_num, (unsigned long)dev->a32.udata);
    }
}


// put crate_regs in V120_DEV struct

int v120_scan_for_vme_cards(V120_DEV *pv120_dev, int addr_space_scan_flag, FILE *fpo)
{
  int i;
  int addr_space;
  V120_DEV *pv;
  int card_num = 0;
  void *pvme;
  unsigned int vme_addr;
  void *pvaddr_base;
  int addr_space_len;
  unsigned int dev_vme_addr;

  for (pv = pv120_dev; pv < pv120_dev + MAX_DEV; pv++)
    {
      if ((pv->crate_num >= 0) && (pv->online == 1))
        {
          for (addr_space = A16_SPACE; addr_space <= A32_SPACE; addr_space <<= 1)
            if (addr_space_scan_flag & addr_space)
              {
                switch(addr_space)
                  {
                  case A16_SPACE:
                    pvaddr_base = (void *)pv->a16.base; // virtual address pointer
                    dev_vme_addr = pv->a16.vme_addr;    // vme address
                    addr_space_len = pv->a16.len;       // vme region length
                    break;
                  case A24_SPACE:
                    pvaddr_base = (void *)pv->a24.base;
                    dev_vme_addr = pv->a24.vme_addr;
                    addr_space_len = pv->a24.len;
                    break;
                  case A32_SPACE:
                    pvaddr_base = (void *)pv->a32.base;
                    dev_vme_addr = pv->a32.vme_addr;
                    addr_space_len = pv->a32.len;
                    break;
                  }
        
                pvme = (void *)pvaddr_base;
                while (pvme < pvaddr_base + addr_space_len)
                  {
                    vme_addr = dev_vme_addr + (pvme - pvaddr_base);
                    if (probe_vme_vaddr(pv->crate_regs, pvme, 16, 0) == 1)
                      {
                        for (i = 0; i < sizeof(vme_search_specs) / sizeof(VME_SEARCH_SPECS_SDEF); i++)
                          {
                            if ((*(unsigned short *)(pvme + vme_search_specs[i].vximfr_offset) == vme_search_specs[i].vximfr) &&
                                (*(unsigned short *)(pvme + vme_search_specs[i].vxitype_offset) == vme_search_specs[i].vxitype))
                              {
                                strcpy((char *)pv->cards[card_num].devname, (char *)vme_search_specs[i].devname);
                                strcpy((char *)pv->cards[card_num].devdesc, (char *)vme_search_specs[i].devdesc);
                                pv->cards[card_num].vxitype = vme_search_specs[i].vxitype;
                                pv->cards[card_num].devnum += vme_search_specs[i].num_cards_of_same_type;
                                vme_search_specs[i].num_cards_of_same_type += 1;
                                pv->cards[card_num].vmeaddr = vme_addr;
                                pv->cards[card_num].pbase_vaddr = pvme;
                                pvme += vme_search_specs[i].device_spacing;
                                pv->cards[card_num].online = 1;
                                card_num += 1;
                                break;
                              }
                          }
                      }
                    else
                      pvme += MIN_ADDR_STEP;
                  }
              }

          // print out scan results

          for (i = 0; i < MAX_CARDS_IN_CHASSIS; i++)
            if (pv->cards[i].vxitype != 0)
              {
                int addr_space;
                if (find_vme_addr_space(pv120_dev, pv->cards[i].vmeaddr, &addr_space) != -1)
                  fprintf(fpo, "vmedevice %s[%d][%d] -base 0x%x -a %d -o true\t# %s\n", pv->cards[i].devname, pv->crate_num, pv->cards[i].devnum, pv->cards[i].vmeaddr, get_addr_space(addr_space), pv->cards[i].devdesc);
              }
        }
    }

  return(0);
}


// configure v120 repeaters

#define V120_SET_CRATE_NUM		0x0000000000000001UL
#define V120_SET_A16_BASE		0x0000000000000002UL
#define V120_SET_A16_SIZE		0x0000000000000004UL
#define V120_SET_A16_SPEED		0x0000000000000008UL
#define V120_SET_A24_BASE		0x0000000000000010UL
#define V120_SET_A24_SIZE		0x0000000000000020UL
#define V120_SET_A24_SPEED		0x0000000000000040UL
#define V120_SET_A32_BASE		0x0000000000000080UL
#define V120_SET_A32_SIZE		0x0000000000000100UL
#define V120_SET_A32_SPEED		0x0000000000000200UL

int config_repeater(int arg_c, char *arg_v[], FILE *fpi, FILE *fpo)
{
  unsigned long chgflg = 0;
  char tmp[256];
  
  static unsigned int crate_num;

  static unsigned int a16_base;
  static int a16_size;
  static int a16_speed;

  static unsigned int a24_base;
  static int a24_size;
  static int a24_speed;

  static unsigned int a32_base;
  static int a32_size;
  static int a32_speed;

  static OPTION_TABLE option_table[] = {
    // opt_str		arg_str		arg_value	data				type_mask	routine			chgflg_mask             
    {"repeater",	"v120[*]",	0,		(void *)&crate_num,		T_UINT,         load_crate_num,         V120_SET_CRATE_NUM},	
    {"-a16",		"*",		0,		(void *)&a16_base,		T_UINT,		load_constant,          V120_SET_A16_BASE},	
    {"-s16",		"*",		0,		(void *)&a16_size,		T_INT,		load_constant,          V120_SET_A16_SIZE},	
    {"-S16",		"SLOW",		V120_SSLOW,	(void *)&a16_speed,		T_INT,		load_enum,              V120_SET_A16_SPEED},	// speed = 1 usec,   DTACK timeout = 100 usec, timeout >= 10 usec
    {"-S16",		"MED",		V120_SMED,	(void *)&a16_speed,		T_INT,		load_enum,              V120_SET_A16_SPEED},	// speed = 500 nsec, DTACK timeout = 100 usec, timeout >= 30 usec
    {"-S16",		"FAST",		V120_SFAST,	(void *)&a16_speed,		T_INT,		load_enum,              V120_SET_A16_SPEED},	// speed = 200 nsec, DTACK timeout =  50 usec, timeout >= 30 usec
    {"-S16",		"MAX",		V120_SMAX,	(void *)&a16_speed,		T_INT,		load_enum,              V120_SET_A16_SPEED},	// speed = MAX,      DTACK timeout =  10 usec, timeout >= 60 usec
    {"-a24",		"*",		0,		(void *)&a24_base,		T_UINT,		load_constant,          V120_SET_A24_BASE},	
    {"-s24",		"*",		0,		(void *)&a24_size,		T_INT,		load_constant,          V120_SET_A24_SIZE},	
    {"-S24",		"SLOW",		V120_SSLOW,	(void *)&a24_speed,		T_INT,		load_enum,              V120_SET_A24_SPEED},	// speed = 1 usec,   DTACK timeout = 100 usec, timeout >= 10 usec
    {"-S24",		"MED",		V120_SMED,	(void *)&a24_speed,		T_INT,		load_enum,              V120_SET_A24_SPEED},	// speed = 500 nsec, DTACK timeout = 100 usec, timeout >= 30 usec
    {"-S24",		"FAST",		V120_SFAST,	(void *)&a24_speed,		T_INT,		load_enum,              V120_SET_A24_SPEED},	// speed = 200 nsec, DTACK timeout =  50 usec, timeout >= 30 usec
    {"-S24",		"MAX",		V120_SMAX,	(void *)&a24_speed,		T_INT,		load_enum,              V120_SET_A24_SPEED},	// speed = MAX,      DTACK timeout =  10 usec, timeout >= 60 usec
    {"-a32",		"*",		0,		(void *)&a32_base,		T_UINT,		load_constant,          V120_SET_A32_BASE},	
    {"-s32",		"*",		0,		(void *)&a32_size,		T_INT,		load_constant,          V120_SET_A32_SIZE},	
    {"-S32",		"SLOW",		V120_SSLOW,	(void *)&a32_speed,		T_INT,		load_enum,              V120_SET_A32_SPEED},	// speed = 1 usec,   DTACK timeout = 100 usec, timeout >= 10 usec
    {"-S32",		"MED",		V120_SMED,	(void *)&a32_speed,		T_INT,		load_enum,              V120_SET_A32_SPEED},	// speed = 500 nsec, DTACK timeout = 100 usec, timeout >= 30 usec
    {"-S32",		"FAST",		V120_SFAST,	(void *)&a32_speed,		T_INT,		load_enum,              V120_SET_A32_SPEED},	// speed = 200 nsec, DTACK timeout =  50 usec, timeout >= 30 usec
    {"-S32",		"MAX",		V120_SMAX,	(void *)&a32_speed,		T_INT,		load_enum,              V120_SET_A32_SPEED},	// speed = MAX,      DTACK timeout =  10 usec, timeout >= 60 usec
    {"",        	  "",		0,		(void *)0,			0,              0,                      0}
  };

  crate_num = -1;
  a16_base = 0;
  a16_size = 0;
  a16_speed = V120_SMAX;        // default max VME speed
  a24_base = 0;
  a24_size = 0;
  a24_speed = V120_SMAX;        // default max VME speed
  a32_base = 0;
  a32_size = 0;
  a32_speed = V120_SMAX;        // default max VME speed

  parse_options(arg_c, &arg_v[0], option_table, &chgflg);

  if ((crate_num < 0) || (crate_num >= MAX_DEV))
    {
#ifdef DEBUG
      printf("ERROR %s/%s: crate_num = %d\n", whoami, __FUNCTION__, create_num);
#endif      
      return(-1);
    }

  v120_dev[crate_num].crate_num = crate_num;

#ifdef DEBUG
  printf("a16_base = 0x%x\n", a16_base);
  printf("a24_base = 0x%x\n", a24_base);
  printf("a32_base = 0x%x\n", a32_base);
#endif

  if (a16_base != 0)
    {
      v120_dev[crate_num].a16.vme_addr = a16_base;
      v120_dev[crate_num].a16.len = a16_size;
      v120_dev[crate_num].a16.config = V120_A16 | V120_ESHORT | a16_speed | V120_D16;
      v120_dev[crate_num].a16.tag = (char *)v120_dev[crate_num].a16_tag;
      sprintf(tmp, "v120[%d]_a16", crate_num);
      strcpy((char *)v120_dev[crate_num].a16_tag, tmp);
    }
      
  if (a24_base != 0)
    {
      v120_dev[crate_num].a24.vme_addr = a24_base;
      v120_dev[crate_num].a24.len = a24_size;
      v120_dev[crate_num].a24.config = V120_A24 | V120_ESHORT | a24_speed | V120_D16;
      v120_dev[crate_num].a24.tag = (char *)v120_dev[crate_num].a24_tag;
      sprintf(tmp, "v120[%d]_a24", crate_num);
      strcpy((char *)v120_dev[crate_num].a24_tag, tmp);
    }
      
  if (a32_base != 0)
    {
      v120_dev[crate_num].a32.vme_addr = a32_base;
      v120_dev[crate_num].a32.len = a32_size;
      v120_dev[crate_num].a32.config = V120_A32 | V120_ESHORT | a32_speed | V120_D16;
      v120_dev[crate_num].a32.tag = (char *)v120_dev[crate_num].a32_tag;
      sprintf(tmp, "v120[%d]_a32", crate_num);
      strcpy((char *)v120_dev[crate_num].a32_tag, tmp);
    }

  // process option/args set in input_config_fn file

  if (v120_setup_memory_regions(&v120_dev[crate_num]) == 0)
    {
      fprintf(fpo, "repeater v120[%d] ", crate_num);
      if (v120_dev[crate_num].a16.vme_addr != 0)
        fprintf(fpo, "-a16 0x%x -s16 0x%x -S16 %s -sp16 %d -ep16 %d ", (unsigned int)v120_dev[crate_num].a16.vme_addr, (unsigned int)v120_dev[crate_num].a16.len, print_vme_speed(v120_dev[crate_num].a16.config & V120_PD_SPEED_MASK), v120_dev[crate_num].a16.start_page, v120_dev[crate_num].a16.end_page);
      if (v120_dev[crate_num].a24.vme_addr != 0)
        fprintf(fpo, "-a24 0x%x -s24 0x%x -S24 %s -sp24 %d -ep24 %d ", (unsigned int)v120_dev[crate_num].a24.vme_addr, (unsigned int)v120_dev[crate_num].a24.len, print_vme_speed(v120_dev[crate_num].a24.config & V120_PD_SPEED_MASK), v120_dev[crate_num].a24.start_page, v120_dev[crate_num].a24.end_page);
      if (v120_dev[crate_num].a32.vme_addr != 0)
        fprintf(fpo, "-a32 0x%x -s32 0x%x -S32 %s -sp32 %d -ep32 %d ", (unsigned int)v120_dev[crate_num].a32.vme_addr, (unsigned int)v120_dev[crate_num].a32.len, print_vme_speed(v120_dev[crate_num].a32.config & V120_PD_SPEED_MASK), v120_dev[crate_num].a32.start_page, v120_dev[crate_num].a32.end_page);
    }
  else
    {        
      fprintf(fpo, "repeater v120[%d] ", crate_num);
      if (v120_dev[crate_num].a16.vme_addr != 0)
        fprintf(fpo, "-a16 0x%x -s16 0x%x -S16 %s ", (unsigned int)v120_dev[crate_num].a16.vme_addr, (unsigned int)v120_dev[crate_num].a16.len, print_vme_speed(v120_dev[crate_num].a16.config & V120_PD_SPEED_MASK));
      if (v120_dev[crate_num].a24.vme_addr != 0)
        fprintf(fpo, "-a24 0x%x -s24 0x%x -S24 %s ", (unsigned int)v120_dev[crate_num].a24.vme_addr, (unsigned int)v120_dev[crate_num].a24.len, print_vme_speed(v120_dev[crate_num].a24.config & V120_PD_SPEED_MASK));
      if (v120_dev[crate_num].a32.vme_addr != 0)
        fprintf(fpo, "-a32 0x%x -s32 0x%x -S32 %s ", (unsigned int)v120_dev[crate_num].a32.vme_addr, (unsigned int)v120_dev[crate_num].a32.len, print_vme_speed(v120_dev[crate_num].a32.config & V120_PD_SPEED_MASK));
#ifdef DEBUG
      printf("ERROR %s/%s: cannot v120[%d] memory regions\n", whoami, __FUNCTION__, crate_num);
#endif
    }

  fprintf(fpo, "-o %s\n", print_online(v120_dev[crate_num].online));
  fflush(fpo);

  return(0);
}


// setup v120 A16/A24/A32 memory regions

int v120_setup_memory_regions(V120_DEV *v120_dev)
{
  static int next_vme_page = 0;
  
  // open the device

  if ((v120_dev->crate_handle = v120_open(v120_dev->crate_num)) == (V120_HANDLE *)0)
    {
#ifdef DEBUG
      printf("ERROR %s/%s: v120_open() error, crate_num = %d, error = %s\n", whoami, __FUNCTION__, (int)v120_dev->crate_num, strerror(errno));
#endif
      v120_dev->online = 0;
      return(-1);
    }

  v120_dev->online = 1;

  // get crate registers

  if ((v120_dev->crate_regs = v120_get_config(v120_dev->crate_handle)) == (V120_CONFIG *)0)
    {
#ifdef DEBUG
      printf("ERROR %s/%s: v120_get_config() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, v120_dev->crate_num, strerror(errno));
#endif
      return(-1);
    }

  // setup A16 space

  if (v120_dev->a16.vme_addr != 0)
    {
      if (v120_add_vme_region(v120_dev->crate_handle, (VME_REGION *)&v120_dev->a16) == (VME_REGION *)0)
        {
#ifdef DEBUG
          printf("ERROR %s/%s: v120_add_vme_region() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, (int)v120_dev->crate_num, strerror(errno));
#endif
          return(-1);
        }
    }

  if ((next_vme_page = v120_allocate_vme(v120_dev->crate_handle, 0)) == -1)
    {
#ifdef DEBUG
      printf("ERROR %s/%s: v120_allocate_vme() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, v120_dev->crate_num, strerror(errno));
#endif
      return(-1);
    }

  // setup A24 space

  if (v120_dev->a24.vme_addr != 0)
    {
      if (v120_add_vme_region(v120_dev->crate_handle, (VME_REGION *)&v120_dev->a24) == (VME_REGION *)0)
        {
#ifdef DEBUG
          printf("ERROR %s/%s: v120_add_vme_region() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, (int)v120_dev->crate_num, strerror(errno));
#endif
          return(-1);
        }
    }

  if ((next_vme_page = v120_allocate_vme(v120_dev->crate_handle, next_vme_page)) == -1)
    {
#ifdef DEBUG
      printf("ERROR %s/%s: v120_allocate_vme() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, v120_dev->crate_num, strerror(errno));
#endif
      return(-1);
    }

  // setup A32 space

  if (v120_dev->a32.vme_addr != 0)
    {
      if (v120_add_vme_region(v120_dev->crate_handle, (VME_REGION *)&v120_dev->a32) == (VME_REGION *)0)
        {
#ifdef DEBUG
          printf("ERROR %s/%s: v120_add_vme_region() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, (int)v120_dev->crate_num, strerror(errno));
#endif
          return(-1);
        }
    }

  if ((next_vme_page = v120_allocate_vme(v120_dev->crate_handle, next_vme_page)) == -1)
    {
#ifdef DEBUG
      printf("ERROR %s/%s: v120_allocate_vme() error, crate_num = %d; error = %s\n", whoami, __FUNCTION__, v120_dev->crate_num, strerror(errno));
#endif
      return(-1);
    }

  return(0);
}


// configure VME cards

#define VME_SET_BOARD_NUM		0x0000000000000001UL
#define VME_SET_VME_BASE_ADDR		0x0000000000000002UL

int config_vmedevice(int arg_c, char *arg_v[], FILE *fpi, FILE *fpo)
{
  unsigned long chgflg = 0;
  V120_DEV *pv;
  int addr_space;
  void *pbase_vaddr;
  VME_SPECS_SDEF *pc;
  static int array[2];
  static int __attribute__((unused)) crate_num;
  static int __attribute__((unused)) board_num;
  static unsigned int vme_base_addr;

  static OPTION_TABLE option_table[] = {
    // opt_str		arg_str		arg_value	data				type_mask	routine			chgflg_mask             
    {"vmedevice",	"v210[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v220[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v230[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v250[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v340[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v344[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v346[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v350[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v360[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v365[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v370[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v375[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v380[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v385[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v410[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v420[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v450[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v460[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v470[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v490[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"vmedevice",	"v545[*][*]",	0,		(void *)&array[0],		T_INT,          load_crate_board_num,   VME_SET_BOARD_NUM},	
    {"-base",		"*",		0,		(void *)&vme_base_addr,		T_UINT,		load_constant,		VME_SET_VME_BASE_ADDR},
    {"",          	"",		0,		(void *)0,			0,              0,                      0}
  };

  chgflg = 0;
  vme_base_addr = 0;
  
  // arg_v[0] = "vmedevice"
  // arg_v[1] = "devname[repeater_num][board_num]"
  // arg_v[2] = "-base"
  // arg_v[3] = "hex_addr"

  parse_options(arg_c, &arg_v[0], option_table, &chgflg);

  crate_num = array[0];
  board_num = array[1];

  if (vme_base_addr != 0)
    {
      if (crate_num >= MAX_DEV)
        {
          printf("ERROR %s/%s: crate number %d error\n", whoami, __FUNCTION__, crate_num);
          return(-1);
        }
      else
        pv = &v120_dev[crate_num];
        
      // return the address space for the vme_base_addr

      find_vme_addr_space(pv, vme_base_addr, &addr_space);

      // get the virtual address of the board in the mapped VME range

      if ((pbase_vaddr = probe_vme_addr_in_mapped_range(pv, vme_base_addr, addr_space)) > 0)
        {
          // device is found
          pc = load_card_info(pv, pbase_vaddr, 0, vme_base_addr, addr_space);
          fprintf(fpo, "%s %s -base 0x%x -a %d -o %s\t# %s\n", arg_v[0], arg_v[1], vme_base_addr, get_addr_space(addr_space), print_online(pc->online), pc->devdesc);
          fflush(fpo);
        }
      else
        {
          // device is not found
          char *parg_str = strdup(arg_v[1]);
          parg_str = strtok(parg_str, "[");
          pc = load_card_info(pv, 0, parg_str, vme_base_addr, addr_space);
          fprintf(fpo, "%s %s -base 0x%x -a %d -o %s\t# %s\n", arg_v[0], arg_v[1], vme_base_addr, get_addr_space(addr_space), print_online(pc->online), pc->devdesc);
          fflush(fpo);
          if (parg_str != (char *)0)
            free(parg_str);
        }
    }

  return(0);
}


// return A16_SPACE/A24_SPACE/A32_SPACE for specified VME address

int find_vme_addr_space(V120_DEV *pv, unsigned int vmeaddr, int *paddr_space)
{
  if (pv->online == 0)
    {
      if (paddr_space != (int *)0)
        *paddr_space = 0;
      return(-1);
    }

  if ((vmeaddr >= pv->a16.vme_addr) && (vmeaddr < (pv->a16.vme_addr + pv->a16.len)))
    {
      if (paddr_space != (int *)0)
        *paddr_space = A16_SPACE;
      return(1);
    }
  else if ((vmeaddr >= pv->a24.vme_addr) && (vmeaddr < (pv->a24.vme_addr + pv->a16.len)))
    {
      if (paddr_space != (int *)0)
        *paddr_space = A24_SPACE;
      return(1);
    }
  else if ((vmeaddr >= pv->a32.vme_addr) && (vmeaddr < (pv->a32.vme_addr + pv->a16.len)))
    {
      if (paddr_space != (int *)0)
        *paddr_space = A32_SPACE;
      return(1);
    }

  return(-1);
}
  

// convert VME address to virtual address in mapped range, and probe the virtual address

void *probe_vme_addr_in_mapped_range(V120_DEV *pv, unsigned int vmeaddr, int addr_space)
{
  int online = 0;
  void *pbase_vaddr;

  switch(addr_space)
    {
    case A16_SPACE:
      pbase_vaddr = pv->a16.base + vmeaddr - pv->a16.vme_addr;
      online = probe_vme_vaddr(pv->crate_regs, pbase_vaddr, SHORTBITS, 0);
      break;

    case A24_SPACE:
      pbase_vaddr = pv->a24.base + vmeaddr - pv->a24.vme_addr;
      online = probe_vme_vaddr(pv->crate_regs, pbase_vaddr, SHORTBITS, 0);
      break;

    case A32_SPACE:
      pbase_vaddr = pv->a32.base + vmeaddr - pv->a32.vme_addr;
      online = probe_vme_vaddr(pv->crate_regs, pbase_vaddr, SHORTBITS, 0);
      break;
    }

  if (online)
    return(pbase_vaddr);
  else
    return(0);
}


// load card info into v120_dev[].cards

VME_SPECS_SDEF *load_card_info(V120_DEV *pv, void *pbase_vaddr, char *dev_name, unsigned int vme_addr, unsigned int addr_range)
{
  int i;
  VME_SPECS_SDEF *pc;

  if (pbase_vaddr == (void *)0) // if pbase_vaddr == 0, then card has not been found and is offline
    {
      for (i = 0; i < sizeof(vme_search_specs) / sizeof(VME_SEARCH_SPECS_SDEF); i++)
        {
          if (strcmp(dev_name, (char *)vme_search_specs[i].devname) == 0)
            {
              pv->cards[pv->last_card_index].online = 0;
              goto contin;
            }
        }
    }
  else
    {
      for (i = 0; i < sizeof(vme_search_specs) / sizeof(VME_SEARCH_SPECS_SDEF); i++)
        {
          if ((*(unsigned short *)(pbase_vaddr + vme_search_specs[i].vximfr_offset) == vme_search_specs[i].vximfr) &&
              (*(unsigned short *)(pbase_vaddr + vme_search_specs[i].vxitype_offset) == vme_search_specs[i].vxitype))
            {
              pv->cards[pv->last_card_index].online = 1;

            contin:
              strcpy((char *)pv->cards[pv->last_card_index].devname, (char *)vme_search_specs[i].devname);
              strcpy((char *)pv->cards[pv->last_card_index].devdesc, (char *)vme_search_specs[i].devdesc);
              pv->cards[pv->last_card_index].vxitype = vme_search_specs[i].vxitype;
              pv->cards[pv->last_card_index].devnum += vme_search_specs[i].num_cards_of_same_type;
              pv->cards[pv->last_card_index].vmeaddr = vme_addr;
              pv->cards[pv->last_card_index].pbase_vaddr = (void *)pbase_vaddr;
              pc = &pv->cards[pv->last_card_index];
              vme_search_specs[i].num_cards_of_same_type += 1;
              pv->last_card_index += 1;
              break;
            }
        }
    }

  return(pc);
}


// when using -scan option, cannot have "vmedevice" lines defined in input configuration file

int check_for_vmedevice_in_config(char *input_config_fn)
{
  int num_chars;
  int __attribute__((unused)) arg_c;
  char *arg_v[MAX_ARG_V];
  char cmdline[MAX_CMD_LINE] = {""};
  char parsed_cmdline[MAX_CMD_LINE] = {""};
  FILE *fpi;
  static int found;

  found = 0;
  
  // open input_config_fn file

  if ((fpi = fopen(input_config_fn, "r")) == (FILE *)0)
    return(-1);

  while ((num_chars = read_line(cmdline, MAX_CMD_LINE, fpi)) > 0)
    {
      arg_c = parse_args(cmdline, arg_v, MAX_ARG_V, parsed_cmdline, MAX_CMD_LINE);

      if (strcmp(arg_v[0], "vmedevice") == 0)
        {
          found = 1;
          break;
        }
    }

  if (fpi != (FILE *)0)
    fclose(fpi);

  return(found);
};


// simple print online routine as "true" or "false"

char *print_online(int online_flag)
{
  static char online_string[8];

  if (online_flag)
    strcpy(online_string, "true");
  else
    strcpy(online_string, "false");

  return(online_string);
}


// return address space for enum

int get_addr_space(int addr_enum)
{
  switch(addr_enum)
    {
    case OFFLINE_SPACE:
      return(0);
    case A16_SPACE:
      return(16);
    case A24_SPACE:
      return(24);
    case A32_SPACE:
      return(32);
    default:
      return(-1);
    }
}


// print VME speed

char *print_vme_speed(int speed)
{
  static char speed_string[8];

  memset((void *)speed_string, 0, sizeof(speed_string));
  switch(speed)
    {
    case V120_SSLOW:
      strcpy(speed_string, "SLOW");
      break;
    case V120_SMED:
      strcpy(speed_string, "MED");
      break;
    case V120_SFAST:
      strcpy(speed_string, "FAST");
      break;
    case V120_SMAX:
      strcpy(speed_string, "MAX");
      break;
    }

  return(speed_string);
}
