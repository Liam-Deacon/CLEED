/*********************************************************************
 *                           RFAC.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: master include file for R factor program.
 *
 * Changes:
 *   GH/1997.08.30 - creation
 *********************************************************************/

/*! \file
 *  \brief master include file for R factor program.
 */

#ifndef RFAC_H
#define RFAC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * Other (basic) includes
 *********************************************************************/

#include "gh_stddef.h"

/*!FIXME: real is always float */
#define REAL_IS_FLOAT    /* should be float for DEC alpha */
#include "real.h"

/*********************************************************************
 * Include type definitions and constant values and functions for RFAC
 *********************************************************************/

#include "rfac_def.h"
#include "rfac_func.h"

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_H */
