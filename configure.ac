dnl Process this file with autoconf to produce a configure script.

AC_INIT([plc], [0.1], [ben.burnett@gmail.com])

AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([src/plc.cc])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(plc,0.1)

CXXFLAGS='-Wall -Werror'

AC_PROG_CXX
AC_LANG_CPLUSPLUS

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CONFIG_FILES([
Makefile 
doc/Makefile
src/Makefile
m4/Makefile
])
AC_OUTPUT
