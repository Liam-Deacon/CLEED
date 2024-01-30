/*********************************************************************
 *                       RFAC_VER.H
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.04.14 - creation.
 ********************************************************************/

#ifndef RFAC_VER_H
#define RFAC_VER_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

static const char RFAC[] = "rfac";
static const char RFAC_COPYRIGHT[] = "Copyright (c) 1994-2014 Georg Held"
"Copyright (C) 2014 Liam Deacon";
static const char RFAC_LICENSE[] = "GNU General Public License v3";
static const char RFAC_VERSION[] = ".. Development";
static const char RFAC_AUTHOR[] = "Georg Held (g.held@reading.ac.uk)";
static const char RFAC_MAINTAINER[] = ""; 
static const char RFAC_SHORTDESC[] = "R-Factor program";

/* platform information */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
static const char RFAC_PLAT[] = "Win32";
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
static const char RFAC_PLAT[] = "Win32_Cygwin";
#elif defined(__ANDROID__)
static const char RFAC_PLAT[] = "Android";
#elif defined(__linux__) || defined(__linux)
static const char RFAC_PLAT[] = "Linux";
#else
static const char RFAC_PLAT[] = "Unix";
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_VER_H */
