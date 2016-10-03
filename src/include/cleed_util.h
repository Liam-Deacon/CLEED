/*********************************************************************
 *                          cleed_complex.h
 *
 *  Copyright (C) 2015-2016 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file cleed_util.h
 * \author Liam Deacon
 *
 * Provides definitions to homogenise functions between different systems and standards.
 */

#ifndef CLEED_UTIL_H
#define CLEED_UTIL_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if _MSC_VER /* mangle function names to make MSC happy */
#define strncasecmp(x,y,z) _strnicmp((x),(y),(z))
#define fileno(x) _fileno(x)
#define fopen(x) fopen_s(x)
#define isatty(x) _isatty(x)
#define strcpy(x,y) strcpy_s(x,y)
#define strncpy(x,y,z) strncpy_s(x,y,z)
#define strcat(x,y) strcat_s(x,y)
#else /* use posix symbols */
#include <strings.h>
#endif

#if defined(__cplusplus) || defined(c_plusplus)
} /* extern "C" */
#endif

#endif /* CLEED_UTIL_H */
