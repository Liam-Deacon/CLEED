/*********************************************************************
 *                           SEARCH.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
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
 * \author Georg Held <g.held@reading.ac.uk>
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

#define CLEED_REAL_IS_DOUBLE

#include "cleed_real.h"
#include "cleed_vector.h"
#include "cleed_matrix.h"

#include "leed_def.h"


/*********************************************************************
 * Include type definitions and constant values and functions for SEARCH
 *********************************************************************/

#if USE_GSL == 0 || !defined(USE_GSL) /* use numerical recipes */
# if !defined(USE_GSL)
#   define USE_GSL 0
# endif
# include "nrr.h"
#else /* use the GNU Scientific Library (open source) */
# include <gsl/gsl_vector.h>
# include <gsl/gsl_matrix.h>
# include <gsl/gsl_multimin.h>
#endif

#include "search_def.h"
#include "search_func.h"
#include "search_ver.h"

search_atom *sr_atoms;
search *sr_search;
char *sr_project;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SEARCH_H */
