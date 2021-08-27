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
