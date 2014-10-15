/*********************************************************************
 *                           MAT.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: master include file for matrix operations
 *
 * Changes:
 *   GH/1995.08.17 - creation
 *********************************************************************/

#ifndef MAT_H
#define MAT_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * Include type definitions and constant values and functions for MAT
 *********************************************************************/
typedef long long long_int;
#define LONG_INT_IS_LONG_LONG

#define  REAL_IS_DOUBLE
#include "real.h"
#include "mat_def.h"
#include "mat_func.h"

/*********************************************************************
 * Other (basic) includes
 *********************************************************************/
#include "gh_stddef.h"
#include "cpl.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* MAT_H */
