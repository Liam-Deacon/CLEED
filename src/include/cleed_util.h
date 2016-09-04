/*********************************************************************
 *                          cleed_types.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/29.01.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * This file defines utility functions used throughout CLEED,
 * providing a unified interface.
 */

#ifndef CLEED_UTIL_H
#define CLEED_UTIL_H

#if (defined(_GNU_SOURCE) || defined(_POSIX_SOURCE) || \
     defined(_POSIX_C_SOURCE) || defined(_BSD_SOURCE) || defined(_SVID_SOURCE))
#include <strings.h>
#elif _MSC_VER
#define strncasecmp(x,y,z) _strnicmp((x),(y),(z))
#define strcasecmp(x,y) _stricmp((x),(y))
#define strdup(x,y) _strdup(x,y)
#else
#define strncasecmp(x,y,z) cleed_strncasecmp((x),(y),(z))
#define strcasecmp(x,y) cleed_strcasecmp((x),(y))
#define strdup(x,y) cleed_strdup(x,y)

/* use rudimentary implementation of strncasecmp */
static int cleed_strncasecmp(const char *x, const char *y, size_t n) {
  int i;
  if (!x || !y || n<=0) return x == y ? 0 : -1; /* added for NULL safety */
  for (i = 0; i < n; i++) {
    if (tolower(x[i]) != tolower(y[i])) return (int)(x[i] - y[i]);
    else if (x[i] == '\0') return 0;
  }
}

/* use rudimentary implementation of strcasecmp */
static int cleed_strcasecmp(const char *x, const char *y) {
  size_t len_x = x != NULL ? strlen(x) : 0;
  size_t len_y = y != NULL ? strlen(y) : 0;
  return strncasecmp(x, y, len_x < len_y ? len_x : len_y);
}

/* strdup is not standard C (its POSIX) */
static inline char *cleed_strdup(const char *str)
{
  int n = strlen(str) + 1;
  char *dup = malloc(n);
  return dup ? memcpy(dup, str, n) : NULL;
}

#endif

#endif /* CLEED_UTIL_H */
