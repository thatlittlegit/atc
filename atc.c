/*
 * (c) 2021 thatlittlegit
 * This file is part of the atc project.
 * Licensed under the GNU General Public License, version 3.0 only.
 * SPDX-License-Identifier: GPL-3.0-only
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
/* we don't know who's building it, so all generic info */
#define PACKAGE_NAME "atc"
#define PACKAGE_VERSION "(unknown)"
#define PACKAGE_BUGREPORT "the distributor of the package"

#ifdef __unix__
#define HAVE_UNISTD_H
#define HAVE_GETOPT
#define HAVE_CLOCK_GETTIME
#define HAVE_NANOSLEEP
#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#if defined(HAVE_NANOSLEEP) || defined(HAVE_CLOCK_GETTIME)
#include <time.h>
#endif

#ifdef HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif

#if defined(HAVE_GETOPT_LONG)
#include <getopt.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#else
static int optind = 0;
#endif

static const char HELP[]
    = "usage: %s [-e <early>]...\n"
      "  --help, -h                show this help text\n"
      "  --version, -v, -V         show the atc version\n"
      "  --early-ns, -e <early>    exit <early> nanoseconds early\n"
      "  --early, -E <early>       exit <early> milliseconds early\n"
      "\n"
      "This is the help for " PACKAGE_NAME " " PACKAGE_VERSION ".\n"
      "Report bugs to " PACKAGE_BUGREPORT ".\n"
      "";

static const char* progname = "atc";

struct atc_options {
    int early;
};

static int parse_options(struct atc_options* opts, int argc, char** argv);
static int getopt_impl(int argc, char** argv);
static long currentns(void);
static void atc_nanosleep(long ns);

static int parse_options(struct atc_options* opts, int argc, char** argv)
{
    int chr;

    opts->early = 0;

#ifndef HAVE_NO_ARGS_PARSER
    while ((chr = getopt_impl(argc, argv)) > 0) {
        switch (chr) {
        /* help */
        case 'h':
            fputs(HELP, stdout);
            return -EXIT_SUCCESS;
        /* version */
        case 'v':
        case 'V':
            fputs(PACKAGE_NAME " " PACKAGE_VERSION "\n", stdout);
            return -EXIT_SUCCESS;
        /* early */
        case 'e':
        case 'E': {
            char* errorloc = NULL;
            float val = strtof(optarg, &errorloc);

            if (chr == 'E') {
                val *= 1000000.0f;
            }

            opts->early = (int)val;

            if (*errorloc != 0) {
                fprintf(stderr, "%s: invalid delay '%s'\n", progname, optarg);
                return -EXIT_FAILURE;
            }
            break;
        }
        /* missing argument */
        case ':':
            fprintf(stderr, "%s: option %c requires an argument\n", progname,
                optopt);
            return -EXIT_FAILURE;
        /* unknown option */
        case '?':
        default:
            fprintf(stderr, "%s: unknown option %c\n", progname, optopt);
            return -EXIT_FAILURE;
        }
    }

    return 1;
}

static int getopt_impl(int argc, char** argv)
{
    static const char optstring[] = ":hvVe:E:";

#if defined(HAVE_GETOPT_LONG)
    static const struct option optlong[] = {
        { "help", no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { "early-ns", required_argument, NULL, 'e' },
        { "early", required_argument, NULL, 'E' },
    };

    return getopt_long(argc, argv, optstring, optlong, NULL);
#elif defined(HAVE_GETOPT)
    return getopt(argc, argv, optstring);
#else
    if (optind >= argc) {
        return -1;
    }

    if (*argv[optind] != '-'
#ifdef _WIN32
        && *argv[optind] != '/'
#endif
    ) {
        optopt = argv[optind] + 1;
        return '?';
    }

    optarg = argv[optind + 1];
    return argv[optind][1];
#endif /* builtin argument parser */
#else /* HAVE_NO_ARGS_PARSER */
    /* this is in parse_options */
    return 1;
#endif /* HAVE_NO_ARGS_PARSER */
}

static long currentns(void)
{
#if defined(HAVE_CLOCK_GETTIME)
    struct timespec clock;
    if (clock_gettime(CLOCK_REALTIME, &clock) < 0) {
        perror("atc");
        return 0;
    }
    return clock.tv_nsec;
#elif defined(HAVE_GETTIMEOFDAY)
    struct timeval clock;
    /* can't fail except with EFAULT, which can't happen */
    gettimeofday(&clock, NULL);
    return (long)clock.tv_usec * 1000;
#else
#error "no way to get current time"
#endif
}

static void atc_nanosleep(long ns)
{
#if defined(HAVE_NANOSLEEP)
    struct timespec clock;
    clock.tv_sec = 0;
    clock.tv_nsec = ns;
    nanosleep(&clock, NULL);
#elif defined(HAVE_USLEEP)
    usleep(ns / 1000);
#else
#error "no way to sleep"
#endif
}

int main(int argc, char** argv)
{
    struct atc_options atc_options;
    int ret;

    if (argc > 1) {
        progname = argv[0];
    }

    if ((ret = parse_options(&atc_options, argc, argv)) <= 0) {
        return -ret;
    }

    atc_nanosleep(1000000000 - currentns() - atc_options.early);
    return EXIT_SUCCESS;
}
