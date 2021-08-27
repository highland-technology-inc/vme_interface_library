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
// Date: Sun Aug 08 10:40:05 2021

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// prototypes

#include "is_a_utils_proto.h"
#include "parse_options_proto.h"


int parse_options(int arg_c, char *arg_v[], OPTION_TABLE *option_table, unsigned long *chgflg)
{
  int arg_i = -1;
  OPTION_TABLE *popt;
  int match = 0;
  
  // cycle through opt/arg array

  while (++arg_i < arg_c)
    {
      // cycle option table to match current option

      popt = option_table - 1;
      
      while ((++popt)->routine != (int (*)()) 0)
        {
          match = 0;

          // option_table format for entries with null arg_str -- such as: print_help_text() and set_mask() and set_chgflg()

          if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) == 0) && (arg_c == 1)) 
            {
              if ((popt->routine)(popt, 0, chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for print help text() with an argument

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (arg_i + 1 < arg_c) && (strcmp(arg_v[arg_i + 1], popt->arg_str) == 0) && (popt->routine == print_help_text))
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for load enum()

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (arg_i + 1 < arg_c) && (strcmp(arg_v[arg_i + 1], popt->arg_str) == 0) && (popt->routine == load_enum)) 
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for load_constant()
          
          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strcmp(popt->arg_str, "*") == 0) && (popt->routine == load_constant)) 
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for load_string()
          
          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) > 0) && (popt->routine == load_string)) 
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for load_crate_num()

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) > 0) && (popt->routine == load_crate_num) && (match_dev_name(arg_v[arg_i + 1], popt->arg_str) == 1))
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }
          
          // format for load_crate_board_num()

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) > 0) && (popt->routine == load_crate_board_num) && (match_dev_name(arg_v[arg_i + 1], popt->arg_str) == 1))
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }
          
          // format for noop() with arg

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strcmp(popt->arg_str, "*") == 0) && (popt->routine == noop))
            {
              if ((popt->routine)(popt, arg_v[++arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for noop() without arg

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) == 0) && (popt->routine == noop))
            {
              if ((popt->routine)(popt, arg_v[arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }

          // format for set_chgflg() without arg

          else if ((strcmp(arg_v[arg_i], popt->opt_str) == 0) && (strlen(popt->arg_str) == 0) && (popt->routine == set_chgflg))
            {
              if ((popt->routine)(popt, arg_v[arg_i], chgflg) == -1)
                return(-1);
              match = 1;
              break;
            }
        }

      if ((popt->routine == (int (*)())0) && (match == 0) && (arg_i < arg_c))
        {
          int i;
          printf("ERROR parse_options: no match for option in line: ");
          for (i = 0; i < arg_c; i++)
            printf("%s ", arg_v[i]);
          printf("\n");
          return(-1);
        }
    }

  return(0);
}


int print_help_text(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  int i = 0;
  char **help_text = (char **)popt->data;

  while (strlen(help_text[i]) > 0)
    printf("%s\n", help_text[i++]);

  set_chgflg(popt, arg, chgflg);
  return(0);
}


int set_mask(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  if (popt->arg_value != 0)
    {
      switch(popt->type_mask)
        {
        case T_BYTE:
          *(char *)popt->data |= (char)popt->arg_value;
          break;
        case T_SHORT:
          *(short *)popt->data |= (short)popt->arg_value;
          break;
        case T_INT:
          *(int *)popt->data |= (int)popt->arg_value;
          break;
        case T_LONG:
          *(long *)popt->data |= (long)popt->arg_value;
          break;
        case T_FLOAT:
        case T_DOUBLE:
          return(-1);
        case T_UBYTE:
          *(unsigned char *)popt->data |= (unsigned char)popt->arg_value;
          break;
        case T_USHORT:
          *(unsigned short *)popt->data |= (unsigned short)popt->arg_value;
          break;
        case T_UINT:
          *(unsigned int *)popt->data |= (unsigned int)popt->arg_value;
          break;
        case T_ULONG:
          *(unsigned long *)popt->data |= (unsigned long)popt->arg_value;
          break;
        }

      set_chgflg(popt, arg, chgflg);
    }
    
  return(0);
}


int set_chgflg(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  if ((chgflg != 0) && (popt->chgflg_mask != 0))
    *chgflg |= popt->chgflg_mask;

  return(0);
}


int load_enum(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  switch(popt->type_mask)
    {
    case T_BYTE:
      *(char *)popt->data = (char)popt->arg_value;
      break;
    case T_SHORT:
      *(short *)popt->data = (short)popt->arg_value;
      break;
    case T_INT:
      *(int *)popt->data = (int)popt->arg_value;
      break;
    case T_LONG:
      *(long *)popt->data = (long)popt->arg_value;
      break;
    case T_FLOAT:
      *(float *)popt->data = (float)popt->arg_value;
      break;
    case T_DOUBLE:
      *(double *)popt->data = (double)popt->arg_value;
      break;
    case T_UBYTE:
      *(unsigned char *)popt->data = (unsigned char)popt->arg_value;
      break;
    case T_USHORT:
      *(unsigned short *)popt->data = (unsigned short)popt->arg_value;
      break;
    case T_UINT:
      *(unsigned int *)popt->data = (unsigned int)popt->arg_value;
      break;
    case T_ULONG:
      *(unsigned long *)popt->data = (unsigned long)popt->arg_value;
      break;
    }

  set_chgflg(popt, arg, chgflg);
  return(0);
}


int load_constant(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  char *p;
  int multiplier = 1;

  // check for [k|K] or [m|M] suffix

  if ((p = strpbrk(arg, "kKmM")) != (char *)0)
    {
      switch(*p)
        {
        case 'k':
        case 'K':
          *p = '\0';
          multiplier = 1024;
          break;
        case 'm':
        case 'M':
          *p = '\0';
          multiplier = 1024 * 1024;
          break;
        }
    }

  switch(popt->type_mask)
    {
    case T_BYTE:
      if (is_a_decimal_int(arg) || is_a_octal_int(arg) || is_a_hex_int(arg))
        {
          char c;
          if (sscanf(arg, "%hhi", &c) != 1)
            return(-1);

          *(char *)popt->data = (char)(multiplier * c);
        }
      else
        return(-1);
      break;

    case T_SHORT:
      if (is_a_decimal_int(arg) || is_a_octal_int(arg) || is_a_hex_int(arg))
        {
          short s;
          if (sscanf(arg, "%hi", &s) != 1)
            return(-1);
        
          *(short *)popt->data = (short)(multiplier * s);
        }
      else
        return(-1);
      break;

    case T_INT:
      if (is_a_decimal_int(arg) || is_a_octal_int(arg) || is_a_hex_int(arg))
        {
          int i;
          if (sscanf(arg, "%i", &i) != 1)
            return(-1);
        
          *(int *)popt->data = (int)(multiplier * i);
        }
      else
        return(-1);
      break;

    case T_LONG:
      if (is_a_decimal_int(arg) || is_a_octal_int(arg) || is_a_hex_int(arg))
        {
          long int l;
          if (sscanf(arg, "%li", &l) != 1)
            return(-1);

          *(unsigned long *)popt->data = (long)(multiplier * l);
        }
      else
        return(-1);
      break;

    case T_FLOAT:
      if (is_a_float(arg))
        {
          float f;
          if (sscanf(arg, "%f", &f) != 1)
            return(-1);

          *(float *)popt->data = f;
        }
      else
        return(-1);
      break;

    case T_DOUBLE:
      if (is_a_double(arg))
        {
          double d;
          if (sscanf(arg, "%lf", &d) != 1)
            return(-1);

          *(double *)popt->data = d;
        }
      else
        return(-1);
      break;

    case T_UBYTE:
      if (is_a_hex_uint(arg))
        {
          unsigned char uc;
          if (sscanf(arg, "%hhx", &uc) != 1)
            return(-1);

          *(unsigned char *)popt->data = (unsigned char)(multiplier * uc);
        }
      else if (is_a_octal_uint(arg))
        {
          unsigned char uc;
          if (sscanf(arg, "%hho", &uc) != 1)
            return(-1);

          *(unsigned char *)popt->data = (unsigned char)(multiplier * uc);
        }
      else if (is_a_decimal_uint(arg))
        {
          unsigned char uc;
          if (sscanf(arg, "%hhu", &uc) != 1)
            return(-1);

          *(unsigned char *)popt->data = (unsigned char)(multiplier * uc);
        }
      else
        return(-1);
      break;

    case T_USHORT:
      if (is_a_hex_uint(arg))
        {
          unsigned short us;
          if (sscanf(arg, "%hx", &us) != 1)
            return(-1);

          *(unsigned short *)popt->data = (unsigned short)(multiplier * us);
        }
      else if (is_a_octal_uint(arg))
        {
          unsigned short us;
          if (sscanf(arg, "%ho", &us) != 1)
            return(-1);

          *(unsigned short *)popt->data = (unsigned short)(multiplier * us);
        }
      else if (is_a_decimal_uint(arg))
        {
          unsigned short us;
          if (sscanf(arg, "%hu", &us) != 1)
            return(-1);

          *(unsigned short *)popt->data = (unsigned short)(multiplier * us);
        }
      else
        return(-1);
      break;

    case T_UINT:
      if (is_a_hex_uint(arg))
        {
          unsigned int ui;
          if (sscanf(arg, "%x", &ui) != 1)
            return(-1);

          *(unsigned int *)popt->data = (unsigned int)(multiplier * ui);
        }
      else if (is_a_octal_uint(arg))
        {
          unsigned int ui;
          if (sscanf(arg, "%o", &ui) != 1)
            return(-1);

          *(unsigned int *)popt->data = (unsigned int)(multiplier * ui);
        }
      else if (is_a_decimal_uint(arg))
        {
          unsigned int ui;
          if (sscanf(arg, "%u", &ui) != 1)
            return(-1);

          *(unsigned int *)popt->data = (unsigned int)(multiplier * ui);
        }
      else
        return(-1);
      break;

    case T_ULONG:
      if (is_a_hex_uint(arg))
        {
          unsigned long ul;
          if (sscanf(arg, "%lx", &ul) != 1)
            return(-1);

          *(unsigned long *)popt->data = (unsigned long)(multiplier * ul);
        }
      else if (is_a_octal_uint(arg))
        {
          unsigned long ul;
          if (sscanf(arg, "%lo", &ul) != 1)
            return(-1);

          *(unsigned long *)popt->data = (unsigned long)(multiplier * ul);
        }
      else if (is_a_decimal_uint(arg))
        {
          unsigned long ul;
          if (sscanf(arg, "%lu", &ul) != 1)
            return(-1);

          *(unsigned long *)popt->data = (unsigned long)(multiplier * ul);
        }
      else
        return(-1);
      break;
    }

  set_chgflg(popt, arg, chgflg);
  return(0);
}


int load_string(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  int max_string_len = 0;

  max_string_len = (int)popt->arg_value;
  memset(popt->data, 0, max_string_len);
  strncpy(popt->data, arg, max_string_len - 1);
  set_chgflg(popt, arg, chgflg);
  return(0);
}


int noop(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  return(0);
}


int load_crate_num(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  char *ps = strdup(arg);
  char *p, *q;
  char __attribute__((unused)) *pname;
  int *pdata = popt->data;
  
  p = ps;
  pname = strtok_r(p, "[]", &q);
  p = q;
  *pdata = atoi(strtok_r(p, "[]", &q));
  free(ps);
  set_chgflg(popt, arg, chgflg);

  return(0);
}


int load_crate_board_num(OPTION_TABLE *popt, char *arg, unsigned long *chgflg)
{
  char *ps = strdup(arg);
  char *p, *q;
  char __attribute__((unused)) *pname;
  int *pdata = popt->data;

  p = ps;
  pname = strtok_r(p, "[]", &q);
  p = q;
  *pdata++ = atoi(strtok_r(p, "[]", &q));
  p = q;
  *pdata++ = atoi(strtok_r(p, "[]", &q));
  free(ps);
  set_chgflg(popt, arg, chgflg);

  return(0);
}


int match_dev_name(char *name1, char *name2)
{
  int rtc = 0;
  char *p1, *p2;
  
  p1 = strdup(name1);
  p2 = strdup(name2);

  p1 = strtok(p1, "[");
  p2 = strtok(p2, "[");

  if (strcmp(p1, p2) == 0)
    rtc = 1;

  free(p1);
  free(p2);
  return(rtc);
}
