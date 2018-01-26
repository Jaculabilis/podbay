AC_PREREQ([2.69])
AC_INIT([barcode_service], [0.0.1], [andrewgrantbrooks@gmail.com])
AC_CONFIG_SRCDIR([barcode_service.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Wextra -Wpedantic])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([czmq], [zsock_new])
AC_CHECK_LIB([zmq], [zmq_ctx_new])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
