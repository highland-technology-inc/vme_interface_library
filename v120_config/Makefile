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
# Date: Sun Aug 08 13:01:09 2021

include ../v120_dirs.mk

SHELL = /bin/bash

PWD = $(shell pwd)
PROGRAM = $(notdir $(PWD))

CC = gcc
OP = -O0
CFLAGS = $(OP) -g -Wall

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(subst .c,.o,$(SOURCES))

INCLUDES = -I . -I $(V120_CLT_DIR)/include -I ../lib/read_line -I ../lib/parse_args -I ../lib/parse_options -I ../lib/probe_vme_vaddr -I ../lib/is_a_utils 

LFLAGS = -L ../lib/read_line -L ../lib/parse_args -L ../lib/parse_options -L ../lib/probe_vme_vaddr -L ../lib/is_a_utils -L $(V120_CLT_DIR)/libV120/.libs

LIBS = -lread_line -lparse_args -lparse_options -lprobe_vme_vaddr -lis_a_utils -lV120

ALL = $(PROGRAM)

all: $(ALL)

%.o: %.c $(HEADERS)
	umask 002; $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(PROGRAM): $(OBJECTS)
	umask 002; $(CC) $^ -o $@ $(LFLAGS) $(LIBS) 

.PHONY: clean
clean:
	@rm -f $(PROGRAM) $(OBJECTS) *~ \#*

.PHONY: maketest
maketest:
	@echo "PWD = $(PWD)"
	@echo "PROGRAM = $(PROGRAM)"
	@echo "CC = $(CC)"
	@echo "OP = $(OP)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "SOURCES = $(SOURCES)"
	@echo "HEADERS = $(HEADERS)"
	@echo "OJECTS = $(OBJECTS)"
	@echo "INCLUDES = $(INCLUDES)"
	@echo "LFAGS = $(LFLAGS)"
	@echo "LIBS = $(LIBS)"
	@echo "ALL = $(ALL)"
