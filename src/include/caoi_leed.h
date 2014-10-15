/************************************************************************
 *                           CAOI_LEED.H
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  include file for angle of incidence additions
 *   - type definitions
 *   - constant values
 *
 * Changes:
 *   ZZ/2008.03.23 - Creation
 *   LD/2014.10.07 - Added PROG* definitions, C++ wrapper and some
 *                   Doxygen markup
 *
 ************************************************************************/

#ifndef LEED_AOI_H
#define LEED_AOI_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>

#ifndef STRSIZE
#define STRSIZE 256       /*!< maximum length of string */
#endif

#ifndef STDERR
#define STDERR stderr     /*!< error stream for output */
#endif

#define PROG "cleed_aoi"
#define PROG_COPYRIGHT "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2007-2009 Zhasmina Zheleeva\n" \
"Copyright (C) 2014 Liam Deacon"
#define PROG_LICENSE ""
#define PROG_VERSION ".. - Development"
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define PROG_SHORTDESC "Angle of Incidence (AOI) LEED wrapper program"

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

/*********************************************************************
include global values for multiple datasets
*********************************************************************/
/*!
 *  \brief provides the number of multiple a datasets
 */
size_t sa;

/*! \fn bsrinp(char \param *filebsr, int \param number)
 *  \param *filebsr The .bsr file to read from & write to.
 *  \param number The set number for multiple datasets.
 */
void bsrinp(char *filebsr, size_t number);

/*********************************************************************
END
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* LEED_AOI_H */
