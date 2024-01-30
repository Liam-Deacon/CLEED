/*********************************************************************
 *                           SEARCH.H
 *
 *  Copyright 1994-2014 Georg Held <georg.held@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.15 - creation
 *********************************************************************/

/*!
 * \file
 * \brief Master include file for search program.
 * \author Georg Held <georg.held@diamond.ac.uk>
 *
 */

#ifndef SEARCH_H
#define SEARCH_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


/*********************************************************************
 * Other (basic) includes
 *********************************************************************/

#include "gh_stddef.h"

#if !defined(CLEED_REAL_IS_DOUBLE) || defined (CLEED_REAL_IS_FLOAT)
#define CLEED_REAL_IS_DOUBLE
#endif

#include "cleed_real.h"
#include "cleed_vector.h"
#include "cleed_matrix.h"

#include "leed_def.h"


/*********************************************************************
 * Include type definitions and constant values and functions for SEARCH
 *********************************************************************/
#if USE_GSL == 1  /* use the GNU Scientific Library (open source) */
# include <gsl/gsl_vector.h>
# include <gsl/gsl_matrix.h>
# include <gsl/gsl_multimin.h>
#else  /* use numerical recipes - NOTE: Not open source */
# include "nrr.h"
#endif

#include "search_def.h"
#include "search_func.h"
#include "search_ver.h"

extern search_atom *sr_atoms;
extern search *sr_search;
extern char *sr_project;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SEARCH_H */
