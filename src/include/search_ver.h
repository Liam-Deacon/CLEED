/*********************************************************************
LD/24.04.14

 Include file for

  - search version information

Changes:

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_VER_H
#define SEARCH_VER_H

#include <stdio.h>

#define SEARCH "csearch"
#define SEARCH_COPYRIGHT "Copyright (c) 1994-2014 Georg Held"
#define SEARCH_LICENSE "GNU General Public License v3"
#define SEARCH_VERSION ".. Development"
#define SEARCH_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define SEARCH_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define SEARCH_SHORTDESC "Search program to find R-Factor minimum"

/* platform information */
#ifndef PROG_PLAT
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define PROG_PLAT "Linux"
#elif defined(__unix__) || defined(__unix)
#define PROG_PLAT "Unix"
#else
#define PROG_PLAT "Unknown"
#endif
#endif

#endif /* SEARCH_VER_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
