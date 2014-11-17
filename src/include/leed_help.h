/*********************************************************************
 *                           PROG_HELP.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: Header file for function prototypes:
 *  - usage()
 *    Provides syntax help and other information before exiting
 *  - info()
 *    Provides version information then exits
 *
 * Changes:
 *   LD/2014.04.14 - creation
 *********************************************************************/

#ifndef PROG_HELP_H
#define PROG_HELP_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

/* function prototypes */
void leed_usage(FILE *output);
void leed_usage_sym(FILE *output);
void leed_info();
void leed_info_sym();

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* PROG_HELP_H */
