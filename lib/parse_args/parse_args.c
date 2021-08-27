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
// Date: Sun Aug 08 10:36:07 2021

#include <ctype.h>
#include <string.h>
#include <stdlib.h>


int parse_args(char *cmdline, char *arg_v[], int max_arg_c, char *local_storage, int local_storage_length)
{
  int arg_c = 0;
  char *pi = cmdline;
  char *pieq;                   // pointer to ending quote
  char *po = local_storage;
  int token_started_flag = 0;

  if ((pi == (char *)0) || (po == (char *)0))
    return(-1);

  if (strlen(cmdline) > local_storage_length)
    return(-1);

 next_char:

  if ((*pi == '\n') || (*pi == '\0') || (*pi == '#'))
    {
      *po++ = '\0';
      goto end;
    }
  
  if (*pi == '\\')
    {
      pi++;
      goto next_char;
    }

  if (*pi == '"')
    {
      if ((pieq = strchr(++pi, '"')) == (char *)0)
        return(-1);
      
      arg_v[arg_c++] = po;
      
      while (pi < pieq)
        *po++ = *pi++;

      *po++ = '\0';
      pi++;
      
      goto next_char;
    }
  
  if (isgraph(*pi))             // if printable character excluding space
    {
      if (token_started_flag == 0)
        {
          token_started_flag = 1;
          arg_v[arg_c++] = po;
        }

      *po++ = *pi++;
      goto next_char;
    }

  if (isblank(*pi))
    {
      if (token_started_flag == 1)
        {
          *po++ = '\0';
          token_started_flag = 0;
        }
      
      pi++;
      goto next_char;
    }

 end:

  return(arg_c);
}

