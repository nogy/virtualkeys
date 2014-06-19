#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(virtualkey, 0.01, nogyka@gmail.com)
AM_INIT_AUTOMAKE(virtualkey, 0.01)
AC_OUTPUT(Makefile src/Makefile)
AC_CONFIG_SRCDIR([src/virtualkey.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(X11, XOpenDisplay)
AC_CHECK_LIB(Xtst, XTestGrabControl)
AC_CHECK_LIB(Xi, XListInputDevices)
AC_CHECK_LIB(argtable2, arg_parse)

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([strstr])

AC_OUTPUT
