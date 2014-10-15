/*********************************************************************
 *                           CPL_MACR.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: Header file declaring macros for complex operations.
 *
 * Changes:
 *   GH/1994.06.02 - creation
 *
 ********************************************************************/

#ifndef CPL_MACRO_H
#define CPL_MACRO_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <math.h>

#define cri_abs R_cabs  /*!< ABS - absolute values **/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* CPL_MACRO_H */
