include ../common.mk

AM_YFLAGS = -d
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

# Make sure this header file is generated first (by bison), as it is included
# by other C++ code that will also be compiled.
BUILT_SOURCES = iec_bison.hh

%.hh: %.h
	cp $< $@

CLEANFILES = \
	iec_flex.cc \
	iec_bison.cc \
	iec_bison.hh

lib_LIBRARIES = libstage1_2.a
libstage1_2_a_SOURCES = \
	iec_flex.ll \
	iec_bison.yy \
    create_enumtype_conversion_functions.cc \
	stage1_2.cc 

libstage1_2_a_CPPFLAGS =  -DDEFAULT_LIBDIR='"lib"' -I../../absyntax -DYY_BUF_SIZE=65536 -fpermissive

