/*********************************************************************
 *                           LEED.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.10.07 - creation
 *
 *********************************************************************/

/*!
 * \file
 * \brief Master include file for LEED functions, definitions and types.
 */

#ifndef LEED_H
#define LEED_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * Basic includes
 *********************************************************************/

/*!FIXME: real is always double type */
#define REAL_IS_DOUBLE

#include "real.h"
#include "gh_stddef.h"
#include "mat.h"

/*********************************************************************
 * include type definitions and constant values from "leed_def.h"
 *********************************************************************/
#include "cpl.h"
#include "qm.h"

#include "leed_def.h"
#include "leed_func.h"

/*********************************************************************
 * END
 *********************************************************************/
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* LEED_H */
