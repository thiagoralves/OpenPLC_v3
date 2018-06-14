#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([matiec], [0.1], [msousa@fe.up.pt, beremiz-devel@lists.sourceforge.net])
AC_CONFIG_HEADERS([config/config.h])
AC_CONFIG_AUX_DIR(config)

AM_INIT_AUTOMAKE([foreign])

# Before checking for CXX and CC, set CFLAGS and CXXFLAGS because they
# are otherwise initialized to contain -g -O2
if test "x$CFLAGS" = "x"; then
CFLAGS=
fi
if test "x$CXXFLAGS" = "x"; then
CXXFLAGS=
fi

# Checks for programs.
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_AWK

# Check bison version, we need a version great or equal than 2.4 to build matiec.
version_bison="$(bison --version | sed q | cut -d' ' -f4)"
version_bison=${version_bison:0:3}

AS_IF([awk -v ver="$version_bison" 'BEGIN { if (ver < 2.4) exit 1; }'],
	[have_bison_correct=yes], [have_bison_correct=no])


if test "x${have_bison_correct}" = xno; then
  echo "------------------------------------------"
  echo " Wrong bison version: $version_bison < 2.4 "
  echo "------------------------------------------"
  (exit 1); exit 1;
fi

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stdint.h stdlib.h string.h strings.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime memset pow strcasecmp strdup strtoul strtoull])


AC_CONFIG_MACRO_DIR([config])

AC_CONFIG_FILES([Makefile \
	absyntax/Makefile \
	absyntax_utils/Makefile \
	stage1_2/Makefile \
	stage3/Makefile \
	stage4/Makefile \
	stage4/generate_c/Makefile \
	stage4/generate_iec/Makefile])
AC_OUTPUT


