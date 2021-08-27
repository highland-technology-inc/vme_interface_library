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
// Date: Thu Aug 19 16:06:26 2021

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "read_line_proto.h"

#define MAX_LINE_LENGTH 1024


int read_line(char *line, int line_length, FILE *fpi)
{
  int total_num_chars_read = 0;
  char input_line[MAX_LINE_LENGTH] = {""};
  char *p = input_line;
  char *q;

 readnextline:

  if (fgets(p, MAX_LINE_LENGTH, fpi) == (char *)0)
    return(EOF);

  if ((q = strchr(p, '#')) != (char *)0)
    {
      if (q == p)
        *q = '\0';
      else if ((q > p) && (*(q - 1) != '\\'))
        *q = '\0';        
    }
      
  if (strlen(p) == 0)
    goto readnextline;

  strtok(p, "\r\n");
  q = p + strlen(p) - 1;

  if (*q == '\\')
    {
      *q = '\0';
      p = q;
      goto readnextline;
    }

  total_num_chars_read = strlen(input_line);
  if (total_num_chars_read < line_length)
    strcpy(line, input_line);

  return(strlen(input_line));
}


int strstrip(char *string, char *stripchars)
{
  char *pr = string;
  char *pw = string;

  while (*pr)
    {
      *pw = *pr++;

      if (char_in_string(*pw, stripchars) == 0)
        pw += 1;
    }
  *pw = '\0';

  return(strlen(string));
}


int char_in_string(char c, char *stripchars)
{
  int i;

  for (i = 0; i < strlen(stripchars); i++)
    if (c == stripchars[i])
      return(1);

  return(0);
}


