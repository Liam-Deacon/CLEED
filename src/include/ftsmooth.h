/************************************************************************
 *                            FTSMOOTH.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.05.30 - Creation
 *   GH/1995.06.01 - Start transformation from f(x=0) = 0 and
 *                   interpolate values from -x_0 to x_0.
 *   GH/1995.06.07 - x_0 = n_x/4 * x_step
 *   LD/2013.06.18 - Allow trimming of datasets with
 *                   '--range <arg1> <arg2>'
 *   LD/2014.07.04 - Added VERSION*, BUG* and PROG* defines
 *                 - Added C linkage with C++ compiler
 *   LD/2014.09.30 - OpenMP support and initial Doxygen markup
 ************************************************************************/

/*! \file
 *
 * Header file for \c ftsmooth (Fourier Transformation Smooth)
 */

#ifndef FTSMOOTH_H
#define FTSMOOTH_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <float.h>
#include <ctype.h>

#if defined(USE_OPENMP) && !defined(DEBUG)
#include <omp.h>            /* add openMP support if desired */
#endif

#include "ftsmooth_def.h"
#include "ftsmooth_func.h"
#include "ftsmooth_ver.h"


#endif /* FTSMOOTH_H */

