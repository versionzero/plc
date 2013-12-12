dnl Process this file with autoconf to produce a configure script.

AC_INIT([plc], [0.2], [ben.burnett@gmail.com])

AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([src/plc.cc])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2])

dnl enable all warnings an treat warnings as errors
CXXFLAGS='-Wall -Werror'

dnl option to enable debug output from our code
AC_ARG_ENABLE(debug,
[  --enable-debug	  Turn on debugging],
[case "${enableval}" in
      yes|true) debug=true ;;
      no|false) debug=false ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
esac],[debug=false])
AM_CONDITIONAL(NDEBUG, test x$debug = xfalse)

AC_PROG_CXX
AC_LANG_CPLUSPLUS

AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CONFIG_FILES([
Makefile 
doc/Makefile
m4/Makefile
src/Makefile
tests/Makefile
])
AC_OUTPUT
