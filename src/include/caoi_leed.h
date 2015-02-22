/************************************************************************
 *                           CAOI_LEED.H
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   ZZ/2008.03.23 - Creation
 *   LD/2014.10.07 - Added definitions, C++ wrapper and some Doxygen markup
 ************************************************************************/

#ifndef LEED_AOI_H
#define LEED_AOI_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "gh_stddef.h"

#define PROG "cleed_aoi"
#define PROG_COPYRIGHT "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2007-2009 Zhasmina Zheleeva\n" \
"Copyright (C) 2013-2014 Liam Deacon"
#define PROG_LICENSE "GNU General Public License v3"
#define PROG_VERSION ".. - Development"
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define PROG_SHORTDESC "Angle of Incidence (AOI) LEED wrapper program"

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
void bsrinp (char *filebsr, size_t number);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* LEED_AOI_H */
