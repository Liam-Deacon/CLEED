/************************************************************************
 *                        FTSMOOTH_DEF.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 ************************************************************************/

/*! \file
 *
 * Header file for \c ftsmooth (Fourier Transformation Smooth) definitions.
 */

#ifndef FTSMOOTH_DEF_H
#define FTSMOOTH_DEF_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

/*! \typedef offset
 *  \brief indicates the offset operation type
 */
typedef enum {
  OFFSET_X_TO_VALUE=1,        /*!< make x += offset */
  OFFSET_X_TO_ZERO,           /*!< make x_min = 0 */
  OFFSET_Y_TO_VALUE,          /*!< make f(x) += offset */
  OFFSET_Y_TO_ZERO            /*!< make y_min = 0 */
} fts_offset;

enum {
  INVALID_ARGUMENT_ERROR=-1
};

static const double SQRT_PI = 0.79788456080286541644;     /*!< sqrt(2/PI) */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* FTSMOOTH_DEF_H */

