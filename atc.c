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
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if defined(HAVE_GETOPT_LONG)
#include <getopt.h>
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

static const char VERSIONTEXT[] = PACKAGE_NAME " " PACKAGE_VERSION "\n";

static const char* progname = "atc";

struct atc_options {
    int early;
};

static int parse_options(struct atc_options* opts, int argc, char** argv);
static int getopt_impl(int argc, char** argv);

static int parse_options(struct atc_options* opts, int argc, char** argv)
{
    int chr;

    opts->early = 0;

    while ((chr = getopt_impl(argc, argv)) > 0) {
        switch (chr) {
        /* unknown option */
        case '?':
            fprintf(stderr, "%s: unknown option %c\n", progname, optopt);
            return -EXIT_FAILURE;
        /* missing argument */
        case ':':
            fprintf(stderr, "%s: option %c requires an argument\n", progname,
                optopt);
            return -EXIT_FAILURE;
        /* help */
        case 'h':
            fputs(HELP, stdout);
            return -EXIT_SUCCESS;
        /* version */
        case 'v':
        case 'V':
            fputs(VERSIONTEXT, stdout);
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
#endif
}

int main(int argc, char** argv)
{
    struct timespec clock;
    struct atc_options atc_options;
    int ret;

    if (argc > 1) {
        progname = argv[0];
    }

    if ((ret = parse_options(&atc_options, argc, argv)) <= 0) {
        return -ret;
    }

    if (clock_gettime(CLOCK_REALTIME, &clock) < 0) {
        perror("atc");
        return EXIT_FAILURE;
    }

    clock.tv_sec = 0;
    clock.tv_nsec = 1000000000 - clock.tv_nsec - atc_options.early;
    nanosleep(&clock, NULL);
    return EXIT_SUCCESS;
}
