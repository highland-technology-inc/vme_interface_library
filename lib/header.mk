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
# Date: Sun Aug 08 10:43:07 2021

# common variable assignments

include ../../v120_dirs.mk		# path of calling Makefile down one level

SHELL = /bin/bash

PWD = $(shell pwd)
PROGRAM = $(notdir $(PWD))
LIBRARY = lib$(PROGRAM).a

CC = gcc
OP = -O0
CFLAGS = $(OP) -g -Wall

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(subst .c,.o,$(SOURCES))

INCLUDES = -I . 
