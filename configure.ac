#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([digitalnetindex], [0.7], [saito@manieth.com])
#LT_INIT
AC_CONFIG_SRCDIR([src/Makefile.am])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE
#LT_INIT
#AC_PROG_RANLIB
AC_CANONICAL_HOST

# Checks for programs.
: ${CFLAGS=""}
: ${CXXFLAGS=""}
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for header files.
#AC_HEADER_STDC
#AX_EXT


AC_LANG_PUSH(C++)
AX_CXX_COMPILE_STDCXX_0X # my HPC Grid does not support C++11

AX_CXX_COMPILE_STDCXX_14(noext, optional) # keep this order
#AX_CXX_COMPILE_STDCXX_11(noext, mandatory)
AX_CXX_COMPILE_STDCXX_11(noext, optional) # my HPC Grid does not support C++11

AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h])

AC_CHECK_LIB(sqlite3, sqlite3_open, [], [ AC_MSG_ERROR(Need sqlite3) ])

AC_LANG_POP

AX_GCC_BUILTIN(__builtin_popcount)
AX_GCC_BUILTIN(__builtin_popcountl)
AX_GCC_BUILTIN(__builtin_popcountll)

AC_CONFIG_FILES([Makefile src/Makefile sqlite3/Makefile])
AC_OUTPUT
