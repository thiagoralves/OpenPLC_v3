#!/bin/bash
# matiec - a compiler for the programming languages defined in IEC 61131-3
#
# Copyright (C) 2003-2011  Mario de Sousa (msousa@fe.up.pt)
# Copyright (C) 2007-2011  Laurent Bessard and Edouard Tisserant
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Shell script to build test for windows

STFILE=$1

shift

CFLAGS=$*


#CC=gcc
CC=i686-mingw32-gcc

../iec2c $STFILE -I ../lib 
#2>/dev/null

$CC -I ../lib -c STD_RESSOURCE.c $CFLAGS

$CC -I ../lib -c  STD_CONF.c $CFLAGS

$CC -I ../lib -c  plc.c $CFLAGS

$CC -I ../lib main.c STD_CONF.o STD_RESSOURCE.o plc.o $CFLAGS -o test.exe
