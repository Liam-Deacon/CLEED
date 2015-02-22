/************************************************************************
 *                        COORD.H
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Change Log:
 *   LD/2014.07.30 - Creation
 ************************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Header provides an \f$ (x,y,z) \f$ coordinate data type #coord and
 * its associated functions.
 */

#ifndef COORD_H
#define COORD_H

#include <stdio.h>
#include <stddef.h>

#if !__GNUC__
#define __attribute__(x) /* empty - disables GNU attribute extension */
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  double x;       /*!< x-axis position */
  double y;       /*!< y-axis position */
  double z;       /*!< z-axis position */
} coord;          /*!< Structure for x,y,z coordinates */

__attribute__((returns_nonnull)) coord *coord_init();
__attribute__((nonnull)) void coord_free(coord *pos);
__attribute__((nonnull)) double coord_get_x(const coord *pos);
__attribute__((nonnull)) double coord_get_y(const coord *pos);
__attribute__((nonnull)) double coord_get_z(const coord *pos);
__attribute__((nonnull)) double coord_get_magnitude(const coord *pos);
__attribute__((nonnull)) size_t coord_get_allocated_size(const coord *pos);
__attribute__((nonnull)) void coord_set_x(coord *pos, double x);
__attribute__((nonnull)) void coord_set_y(coord *pos, double y);
__attribute__((nonnull)) void coord_set_z(coord *pos, double z);
__attribute__((nonnull)) void coord_set(coord *pos, double x, double y, double z);
__attribute__((nonnull)) void coord_copy(coord *dst, const coord *src);
__attribute__((nonnull(2))) void coord_printf(FILE *f, const coord *pos);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* COORD_H */
