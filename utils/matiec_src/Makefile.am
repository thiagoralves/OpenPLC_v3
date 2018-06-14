include common.mk

bin_PROGRAMS = iec2c iec2iec

SUBDIRS = absyntax absyntax_utils stage1_2 stage3 stage4 

ACLOCAL_AMFLAGS=-I config

HGVERSION= $(shell hg -R $(top_srcdir) parents --template '{node|short}' 2> /dev/null || grep node $(top_srcdir)/.hg_archival.txt 2> /dev/null || true )
AM_CXXFLAGS += -DHGVERSION="\"${HGVERSION}\""

iec2c_LDADD = stage1_2/libstage1_2.a \
	stage3/libstage3.a \
	stage4/generate_c/libstage4_c.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a 

iec2iec_LDADD = stage1_2/libstage1_2.a \
	stage3/libstage3.a \
	stage4/generate_iec/libstage4_iec.a \
	absyntax/libabsyntax.a \
	absyntax_utils/libabsyntax_utils.a 

iec2c_SOURCES = main.cc

iec2iec_SOURCES = main.cc

