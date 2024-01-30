/*********************************************************************
 *                           MAT.H
 *
 *  Copyright 1995-2014 Georg Held <georg.held@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.17 - creation
 *********************************************************************/

/*!
 * \file
 * \brief Master include file for matrix operations.
 */

#ifndef __MAT_H__
#define __MAT_H__

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * Include type definitions and constant values and functions for MAT
 *********************************************************************/
#define LONG_INT_IS_LONG_LONG

#ifndef  CLEED_REAL_IS_DOUBLE
# define CLEED_REAL_IS_DOUBLE
#endif

#include "cleed_real.h"
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

#endif /* __MAT_H__ */
