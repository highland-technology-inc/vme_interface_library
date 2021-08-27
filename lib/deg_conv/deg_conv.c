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
// Date: Sat Aug 07 16:49:20 2021

// from degC

float degC_to_degF(float degC)
{
  return(degC * 1.8 + 32.0);
}

float degC_to_degK(float degC)
{
  return(degC + 273.15);
}

float degC_to_degR(float degC)
{
  return(degC * 1.8 + 32.0 + 459.67);
}

// from degF

float degF_to_degC(float degF)
{
  return((degF - 32.0) / 1.8);
}

float degF_to_degK(float degF)
{
  return((degF + 459.67) / 1.8);
}

float degF_to_degR(float degF)
{
  return(degF + 459.67);
}

// from degK

float degK_to_degC(float degK)
{
  return(degK - 273.15);
}

float degK_to_degF(float degK)
{
  return(degK * 1.8 - 459.67);
}

float degK_to_degR(float degK)
{
  return(degK * 1.8);
}

// from degR

float degR_to_degC(float degR)
{
  return((degR - 32.0 - 459.67) / 1.8);
}

float degR_to_degF(float degR)
{
  return(degR - 459.67);
}

float degR_to_degK(float degR)
{
  return(degR / 1.8);
}
