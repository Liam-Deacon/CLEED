/*********************************************************************
GH/27.09.00

include file for 
 - current version of LEED program

Changes:
GH/27.09.00 - create
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef LEED_VER_H
#define LEED_VER_H

#ifdef LEED_VERSION
#undef LEED_VERSION
#endif

#ifdef LEED_NAME
#undef LEED_NAME
#endif

/* current symmetry version */
#define LEED_VERSION "cleed_sym - version 1.1 (2014.07.04 - )"
#define LEED_NAME "CLEED_SYM"

/* additional program information */
#define PROG "cleed_sym"
#define PROG_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 1999-2000 Wolfgang Brown" \
"Copyright (C) 2014 Liam Deacon"
#define PROG_LICENSE "GNU General Public License v3"
#define PROG_VERSION "2014.07.04 "
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define PROG_SHORTDESC "Symmetrised LEED program"

/* detect os */
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


#endif /* LEED_VER_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
