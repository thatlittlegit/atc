dnl (c) 2021 thatlittlegit
dnl This file is part of the atc project.
dnl Licensed under the GNU General Public License, version 3.0 only.
dnl SPDX-License-Identifier: GPL-3.0-only
AC_PREREQ([2.69])
AC_INIT([atc], [1.0], [personal@thatlittlegit.tk])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([atc.c])
AC_LANG_WERROR
AC_PROG_CC

AC_ARG_WITH([getopt],
    [AS_HELP_STRING([--with-getopt], [
        the argument parser to use.
        'yes' means to autodetect, 'no' and arguments are not parsed.
        Options are 'yes', 'no', 'getopt', 'getopt-long', 'builtin'
    ])],
    # dnl ') <-- vim
    AS_VAR_SET([optparser], $enableval),
    AS_VAR_SET([optparser], yes))

AC_CHECK_FUNC([clock_gettime],, AC_ERROR([clock_gettime must be defined]))

AS_VAR_IF([optparser], [yes], AS_VAR_SET([optparser_any]))
AC_DEFUN([ATC_UNLESS], [AS_VAR_IF([$1], $2)])

AC_DEFUN([ATC_TRY_OPTPARSER], [
    AS_IF([test "$optparser" = "$1" || test "$optparser" = "yes"], [
        AS_MESSAGE([trying $1 for arguments parser...])
        AC_CHECK_HEADER([$2], [
        AC_CHECK_FUNC([$3], [
            dnl success
            AS_UNSET([optparser_any])
            AS_VAR_SET([optparser], [done])
            AC_DEFINE([$4], [1], [$5])
        ], [
            dnl function not found
            ATC_UNLESS([optparser_any], AS_ERROR([function $3 for $1 not found]))
        ])], [
            dnl header not found
            ATC_UNLESS([optparser_any], AS_ERROR([header $2 for $1 not found]))
        ])
    ])
    AH_TEMPLATE([$4], [$5])
])

ATC_TRY_OPTPARSER(getopt-long, getopt.h, getopt_long, HAVE_GETOPT_LONG, [Use getopt_long as the arguments parser.])
ATC_TRY_OPTPARSER(getopt, unistd.h, getopt, HAVE_GETOPT, [Use getopt as the arguments parser.])

AS_VAR_IF([optparser], [], [
    dnl use no argument parsing
    AS_MESSAGE([using no argument parser])
    AC_DEFINE([HAVE_NO_ARGS_PARSER], [0], [Are we using an arguments parser?])
    AH_TEMPLATE([HAVE_NO_ARGS_PARSER], [0], [Are we using an arguments parser?])
    AS_VAR_SET([optparser], [done])
])

AS_VAR_IF([optparser], [done],, [
    dnl use built-in, no define
    AS_MESSAGE([using built-in arguments parser])
    AS_UNSET([optparser_any])
    AS_VAR_SET([optparser], [builtin])
])

AC_CHECK_HEADER([time.h], [
    AC_CHECK_FUNC([clock_gettime], [
        AC_DEFINE([HAVE_CLOCK_GETTIME], [1], [Defined if we should use clock_gettime.])
        AH_TEMPLATE([HAVE_CLOCK_GETTIME], [1], [Defined if we should use clock_gettime.])
        AS_VAR_SET([foundtime], [1])
    ])
])

AS_VAR_SET_IF([foundtime],, [
    AC_CHECK_HEADER([sys/time.h], [
        AC_CHECK_FUNC([gettimeofday], [
            AC_DEFINE([HAVE_GETTIMEOFDAY], [1], [Defined if we have gettimeofday.])
            AH_TEMPLATE([HAVE_GETTIMEOFDAY], [Defined if we have gettimeofday.])
            AS_VAR_SET([foundtime], [1])
        ])
    ])
])

AS_VAR_SET_IF([foundtime],, AC_ERROR([Could not find time function like gettimeofday or clock_gettime.]))

AC_CHECK_HEADER([time.h], [
    AC_CHECK_FUNC([nanosleep], [
        AC_DEFINE([HAVE_NANOSLEEP], [1], [Defined if we have nanosleep.])
        AH_TEMPLATE([HAVE_NANOSLEEP], [Defined if we have nanosleep.])
        AS_VAR_SET([foundsleep], [1])
    ])
])

AS_VAR_SET_IF([foundsleep],, [
    AC_CHECK_HEADER([unistd.h], [
        AC_CHECK_FUNC([usleep], [
            AC_DEFINE([HAVE_USLEEP], [1], [Defined if we have usleep.])
            AH_TEMPLATE([HAVE_USLEEP], [Defined if we have usleep.])
            AS_VAR_SET([foundsleep], [1])
        ])
    ])
])

AS_VAR_SET_IF([foundsleep],, AC_ERROR([Could not find sleep function like nanosleep.]))

AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])

AC_OUTPUT

AH_TEMPLATE([HAVE_GETOPT], [Define if getopt exists.])
AH_TEMPLATE([HAVE_GETOPT_LONG], [Define if getopt_long exists.])
