dnl (c) 2021 thatlittlegit
dnl This file is part of the atc project.
dnl Licensed under the GNU General Public License, version 3.0 only.
dnl SPDX-License-Identifier: GPL-3.0-only
AC_PREREQ([2.69])
AC_INIT([atc], [0.1], [<personal@thatlittlegit.tk>])
AC_CONFIG_SRCDIR([atc.c])
AC_LANG_WERROR
AC_PROG_CC

AC_CHECK_FUNC([clock_gettime])

AC_CHECK_HEADERS([unistd.h],
    AC_CHECK_FUNC([getopt],
        AS_VAR_SET([optparser])
        AC_DEFINE([HAVE_GETOPT], [1], [Define if getopt exists.])))
AC_CHECK_HEADERS([getopt.h],
    AC_CHECK_FUNC([getopt_long],
        AS_VAR_SET([optparser])
        AC_DEFINE([HAVE_GETOPT_LONG], [1], [Define if getopt_long exists.])))

AS_VAR_SET_IF([optparser],, AC_ERROR([missing an arguments parser.]))

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])
AC_OUTPUT

AH_TEMPLATE([HAVE_GETOPT], [Define if getopt exists.])
AH_TEMPLATE([HAVE_GETOPT_LONG], [Define if getopt_long exists.])
