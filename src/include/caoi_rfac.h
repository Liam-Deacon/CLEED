/************************************************************************
 *                           CAOI_LEED.H
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   ZZ/2008.03.03 - Original creation
 *   LD/2014.10.07 - Added definitions, C++ wrapper and some Doxygen markup
 ************************************************************************/

#ifndef RFAC_AOI_H
#define RFAC_AOI_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "gh_stddef.h"

#define PROG "crfac_aoi"
#define PROG_COPYRIGHT "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2007-2009 Zhasmina Zheleeva" \
"Copyright (C) 2013-2014 Liam Deacon"
#define PROG_LICENSE "GNU General Public License v3"
#define PROG_VERSION ".. Development"
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon <liam.m.deacon@gmail.com>" 
#define PROG_SHORTDESC "Angle of Incidence (AOI) R-Factor program"

/* platform information */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
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

/*********************************************************************
include global values for multiple datasets
*********************************************************************/
size_t sa;
int ctrinp(const char filectr[STRSZ]);
char proj_name[STRSZ];

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_AOI_H */
