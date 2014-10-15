/*********************************************************************
 *                           LEED_VER.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: versioning information for standard LEED program
 *
 * Changes:
 *   GH/2000.09.27 - create
 *   LD/2014.07.04 - added PROG* definitions
 *********************************************************************/

#ifndef LEED_VER_H
#define LEED_VER_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


#ifdef LEED_VERSION
#undef LEED_VERSION
#endif

#ifdef LEED_NAME
#undef LEED_NAME
#endif

/* current symmetry version */
#define LEED_VERSION "cleed_nsym (2014.07.04 - )"
#define LEED_NAME "CLEED_NSYM"

/* additional program information */
#define PROG "cleed_nsym"
#define PROG_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 2014 Liam Deacon"
#define PROG_LICENSE "GNU General Public License v3"
#define PROG_VERSION "2014.07.04 "
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon (liam.deacon@diamond.ac.uk)" 
#define PROG_SHORTDESC "Calculate LEED intensities for a given geometry"

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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* LEED_VER_H */
