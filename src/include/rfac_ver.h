/*********************************************************************
LD/24.04.14

 Include file for

  - rfac version information

Changes:

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef RFAC_VER_H
#define RFAC_VER_H

#define PROG "rfac"
#define PROG_COPYRIGHT "Copyright (c) 1994-2014 Georg Held" \
"Copyright (C) 2014 Liam Deacon"
#define PROG_LICENSE ""
#define PROG_VERSION ".. Development"
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "" 
#define PROG_SHORTDESC "R-Factor program"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define PROG_PLAT "Linux"
#else
#define PROG_PLAT "Unix"
#endif

#endif /* RFAC_VER_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
