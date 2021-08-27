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
# Date: Sun Aug 08 13:39:18 2021

include ../../v120_dirs.mk		# path of calling Makefile down one level

# disable default suffix rules

MAKEFLAGS = -r

# do not automatically delete object files

.PRECIOUS: %.o

SHELL = /bin/bash

PWD = $(shell pwd)
PROGRAM = $(notdir $(PWD))

CC = gcc
OP = -O0
CFLAGS = $(OP) -g -Wall -Wno-unused-but-set-variable

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(subst .c,.o,$(SOURCES))

INCLUDES = -I . -I ../../lib/$(PROGRAM)_utils -I ../../lib/get_vaddr_for_device -I ../../lib/deg_conv -I ../../lib/ht_utils -I ../../lib/misc_math_utils -I ../../lib/parse_args -I ../../lib/parse_options -I ../../lib/is_a_utils -I ../../lib/read_line -I $(V120_CLT_DIR)/include

LFLAGS = -L ../../lib/$(PROGRAM)_utils -L ../../lib/get_vaddr_for_device -L ../../lib/deg_conv -L ../../lib/ht_utils -L ../../lib/misc_math_utils -L ../../lib/parse_args -L ../../lib/parse_options -L ../../lib/is_a_utils -L ../../lib/read_line -L $(V120_CLT_DIR)/libV120/.libs

LIBS = -l$(PROGRAM)_utils -lget_vaddr_for_device -ldeg_conv -lht_utils -lmisc_math_utils -lparse_options -lparse_args -lis_a_utils -lread_line -lV120 -lm

