#!/bin/bash

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Author: Dean W. Anneser
# Company: RTLinux Solutions LLC for Highland Technology, Inc.
# Date: 
# $1 = base address
# $2 = address range

# Wrapper for "v120 read address word_count"
# To display 0x500 16-bit words of V545 whose base address is 0xffff0000:
#   watch -n1 "./read.sh 0xffff0000 0x500"

typeset -i start_address=$1
typeset -i end_address=start_address+$2
typeset -i word_count=$2/2
typeset -i address=$start_address
typeset -i word_step=8
typeset -i mask=0xffff-${word_step}-1

((word_count&=${mask}))

while ((address < end_address)); do
  # skip a line ever 0x100 addresses
  if (( $((address & 0xff)) == 0x0 )); then
     printf "\n"
  fi
  printf "0x%8.8x: " $address
  v120 read $address $word_step
  address+=word_step*2
done
