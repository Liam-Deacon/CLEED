/*********************************************************************
 *                           SEARCH_VER.H
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/24.04.14 - creation
 *********************************************************************/

#ifndef SEARCH_VER_H
#define SEARCH_VER_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

static const char SEARCH[] = "csearch";
static const char SEARCH_COPYRIGHT[] = "Copyright (c) 1994-2014 Georg Held";
static const char SEARCH_LICENSE[] = "GNU General Public License v3";
static const char SEARCH_VERSION[] = ".. Development";
static const char SEARCH_AUTHOR[] = "Georg Held (g.held@reading.ac.uk)";
static const char SEARCH_MAINTAINER[] = "Liam Deacon <liam.deacon@diamond.ac.uk>"; 
static const char SEARCH_SHORTDESC[] = "Search program to find R-Factor minimum";

/* platform information */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
static const char SEARCH_PLAT[] = "Win32";
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
static const char *SEARCH_PLAT[] = "Win32_Cygwin";
#elif defined(__ANDROID__)
static const char SEARCH_PLAT[] = "Android";
#elif defined(__linux__) || defined(__linux)
static const char SEARCH_PLAT[] = "Linux";
#elif defined(__unix__) || defined(__unix)
static const char SEARCH_PLAT[] = "Unix";
#else
static const char SEARCH_PLAT[] = "Unknown";
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SEARCH_VER_H */
