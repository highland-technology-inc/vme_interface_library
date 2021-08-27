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
// Date: Sat Aug 07 17:21:43 2021

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "is_a_utils_proto.h"

#define FORMAT_SPEC_LENGTH 8

char format_spec[][FORMAT_SPEC_LENGTH] = {
  {"%c"},
  {"%hhd"},
  {"%hhi"},
  {"%hd"},
  {"%hi"},
  {"%d"},
  {"%i"},
  {"%ld"},
  {"%li"},
  {"%f"},
  {"%lf"},
  {"%hhx"},
  {"%hho"},
  {"%hhu"},
  {"%uc"},
  {"%hx"},
  {"%ho"},
  {"%hu"},
  {"%x"},
  {"%o"},
  {"%u"},
  {"%lx"},
  {"%lo"},
  {"%lu"}};

  
// is a printf numeric format specifier

int is_a_format_spec(char *string)
{
  int i;

  for (i = 0; i < sizeof(format_spec) / FORMAT_SPEC_LENGTH; i++)
    if (strcmp(string, format_spec[i]) == 0)
      return(1);

  return(0);
}

// is an octal digit

int is_octal_digit(char c)
{
  if (isdigit(c) && (c != '8') && (c != '9'))
    return(1);
  else
    return(0);
}


// is a hex digit

int is_hex_digit(char c)
{
  if (isxdigit(c) || (c == 'x') || (c == 'X'))
    return(1);
  else
    return(0);
}


// is a decimal integer number

int is_a_decimal_int(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p == '-')
    p += 1;

  if ((*p == '0') && (isdigit(*(p + 1))))
    return(0);

  if (*p == '\0')
    return(0);

  while (*p != '\0')
    if (! isdigit(*p++))
      return(0);
        
  return(1);
}


// is an unsigned decimal number

int is_a_decimal_uint(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p == '0')
    return(0);

  while (*p != '\0')
    if (! isdigit(*p++))
      return(0);
        
  return(1);
}


// is an octal number

int is_a_octal_int(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p == '-')
    p += 1;

  if (*p++ != '0')
    return(0);

  if (*p == '\0')
    return(0);

  while (*p != '\0')
    if (! is_octal_digit(*p++))
      return(0);
        
  return(1);
}


// is an unsigned octal number

int is_a_octal_uint(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p++ != '0')
    return(0);

  while (*p != '\0')
    if (! is_octal_digit(*p++))
      return(0);
        
  return(1);
}


// is a hex number

int is_a_hex_int(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p == '-')
    p += 1;

  if (*p++ != '0')
    return(0);

  if (*p++ != 'x')
    return(0);

  if (*p == '\0')
    return(0);

  while (*p != '\0')
    if (! is_hex_digit(*p++))
      return(0);
        
  return(1);
}
  

// is an unsigned hex number

int is_a_hex_uint(char *string)
{
  char *p = string;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p++ != '0')
    return(0);

  if (*p++ != 'x')
    return(0);

  while (*p != '\0')
    if (! is_hex_digit(*p++))
      return(0);
        
  return(1);
}
  

// is a floating point number

int is_a_float(char *string)
{
  char *p = string;
  int decimal_count = 0;

  if ((p == (char *)0) || (*p == '\0'))
    return(0);

  if (*p == '-')
    p += 1;

  if ((*p == '0') && (*(p + 1) != '.'))
    return(0);

  while (*p != '\0')
    {
      if (*p == '.')
        {
          decimal_count += 1;
          p += 1;
        }
      else
        {
          if (! isdigit(*p++))
            return(0);
        }
    }

  if ((decimal_count == 0) || (decimal_count == 1))
    return(1);
  else
    return(0);
}


// is a long floating point number

int is_a_double(char *string)
{
  return(is_a_float(string));
}
         

// unsigned int expressed with 'k' or 'K' suffix 

int is_a_decimal_kilo_uint(char *string)
{
  int rtc = 0;
  int length = strlen(string);
  char *p;
  
  p = calloc(length, sizeof(char));

  if ((string[length - 1] == 'k') || (string[length - 1] == 'K'))
    {
      strncpy(p, string, length - 1);
      if ((is_a_decimal_uint(p)) || (is_a_octal_uint(p)) || (is_a_hex_uint(p)))
        rtc = 1;
    }

  free(p);
  return(rtc);
}


// unsigned int expressed with 'm' or 'M' suffix

int is_a_decimal_mega_uint(char *string)
{
  int rtc = 0;
  int length = strlen(string);
  char *p;
  
  p = calloc(length, sizeof(char));

  if ((string[length - 1] == 'm') || (string[length - 1] == 'M'))
    {
      strncpy(p, string, length - 1);
      if ((is_a_decimal_uint(p)) || (is_a_octal_uint(p)) || (is_a_hex_uint(p)))
        rtc = 1;
    }

  free(p);
  return(rtc);
}


