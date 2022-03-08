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
// Date: Sat Aug 07 17:24:30 2021

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count_char_in_string(char *string, char c)
{
  static int count;
  char *p = string - 1;

  count = 0;

  if (string == (char *)0)
    return(-1);

  while (*++p != '\0')
    if (*p == c)
      count++;

  return(count);
}


int string_contains_matched_brackets(char *string)
{
  char *p, *q;
  
  if ((p = strchr(string, '[')) == (char *)0) // check for [
    return(-1);
  
  if ((q = strchr(string, ']')) == (char *)0) // check for ]
    return(-1);
  
  if (p > q)                    // check for ][
    return(-1);

  return(1);
}


int return_string_in_brackets(char *string, char *bracketed_string)
{
  if (string_contains_matched_brackets(string) == -1)
    return(-1);
  
  bracketed_string = strchr(string, '[') + 1;
  return(0);
}


int return_integer_in_brackets(char *string, int *subscript)
{
  char *s, *p, *q;
  int bracketed_value;

  s = strdup(string);

  if ((p = strchr(s, '[')) == (char *)0) // check for [
    {
      free(s);
      return(-1);
    }
  
  if ((q = strchr(s, ']')) == (char *)0) // check for ]
    {
      free(s);
      return(-1);
    }
  
  if (p > q)                    // check for ][
    {
      free(s);
      return(-1);
    }

  if (sscanf(p + 1, "%d", &bracketed_value) != 1)
    {
      free(s);
      return(-1);
    }
  
  free(s);
  *subscript = bracketed_value;
  return(0);
}


void remove_leading_whitespace(char *string)
{
  int i;
  char *p = string;

  while (isspace(*p))
    p++;

  i = strlen(p);
  memmove(string, p, i);
  string[i] = '\0';
  return;
}


int is_a_comment(char *string)
{
  char *p = string;

  while (isspace(*p))
    p++;

  if (*p == '#')
    return(1);

  return(0);
}


int comma_del_string_to_mask(char *string, void *mask, int mask_len_bytes)
{
  int num_bits_in_mask = 0;
  char *pstring;
  char *p;
  int bitnum;
  int rtc = 0;

  switch(mask_len_bytes)
    {
    case 1:
    case 2:
    case 4:
    case 8:
      num_bits_in_mask = 8 * mask_len_bytes;
      break;
    default:
      return(-1);
    }

  pstring = strdup(string);

  p = strtok(pstring, ",");
  
  while (p != (char *)0)
    {
      bitnum = atoi(p);
      if (bitnum >= num_bits_in_mask)
        {
          rtc = -1;
          goto end;
        }

      switch(mask_len_bytes)
        {
        case 1:
          *(uint8_t *)mask = (uint8_t)(1 << bitnum);
          break;
        case 2:
          *(uint16_t *)mask = (uint16_t)(1 << bitnum);
          break;
        case 4:
          *(uint32_t *)mask = (uint32_t)(1 << bitnum);
          break;
        case 8:
          *(uint64_t *)mask = (uint64_t)(1L << bitnum);
          break;
        }

      p = strtok(0, ",");
    }

 end:
  free(pstring);
  return(rtc);
}
