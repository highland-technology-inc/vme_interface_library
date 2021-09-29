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
// Date: Sat Aug 07 17:23:11 2021

#include <math.h>

double clamp(double x, double xmin, double xmax)
{
  if (x > xmax)
    return(xmax);
  else if (x < xmin)
    return(xmin);
  else
    return(x);
}


double ratio(double x, double xmin, double xmax, double ymin, double ymax)
{
  if (xmax == xmin)
    return(0.0L);
  else
    return(((clamp(x, xmin, xmax) - xmin) / (xmax - xmin)) * (ymax - ymin) + ymin);
}


double rotational_range(double deg, double range)
{
  double __attribute__((unused)) integral;
  
  deg = range * modf((deg / range), &integral);
  if (deg == -0.0L)
    deg = 0.0L;

  return(deg);
}


int inrange(double x, double low, double high)
{
  if ((x < low) || (x > high))
    return(-1);
  return(0);
}


