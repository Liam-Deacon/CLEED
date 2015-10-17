/*********************************************************************
 *                           COORD.C
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * Implements methods for manipulating #coord objects.
 */

#include "coord.h"
#include <stdlib.h>
#include <math.h>

/*!
 * Initializes #coord instance with a position (0, 0, 0).
 *
 * \return Pointer to initialized #coord object
 * \retval \c NULL if memory could not be allocated.
 */
inline coord *coord_init()
{
  coord *pos = (coord*) calloc(1, sizeof(coord));
  return (pos);
}

/*!
 * Frees #coord object \p pos from memory.
 *
 * \param pos #coord object to free from memory.
 */
inline void coord_free(coord *pos)
{
  if (pos != NULL) free(pos);
  pos = NULL;
}

/*!
 * Returns the amount of size allocated to the #coord object \p pos in memory.
 *
 * \param[in] pos Pointer to #coord structure.
 * \return The maximum number of #coord objects with allocated memory to \p pos.
 */
size_t coord_get_allocated_size(const coord *pos)
{
  return (sizeof(pos)/sizeof(pos[0]));
}

/*!
 * Copies #coord data from \p src into \p dst
 *
 * \param dst destination for coordinate data.
 * \param src source of coordinate data.
 */
inline void coord_copy(coord *dst, const coord *src)
{
  if (src != NULL)
    coord_set(dst, src->x, src->y, src->z);
}

/*!
 * Getter function for @coord::x
 *
 * \param[in] pos Pointer to #coord structure.
 * \return @coord::x of \p pos
 */
inline double coord_get_x(const coord *pos)
{
  return (pos->x);
}

/*!
 * Getter function for @coord::y
 *
 * \param[in] pos Pointer to #coord structure.
 * \return @coord::y of \p pos
 */
inline double coord_get_y(const coord *pos)
{
  return (pos->y);
}

/*!
 * Getter function for @coord::z
 *
 * \param[in] pos Pointer to #coord structure.
 * \return @coord::z of \p pos
 */

inline double coord_get_z(const coord *pos)
{
  return (pos->z);
}

/*!
 * Calculates the magnitude of the vector given by \p pos
 *
 * \param[in] pos Pointer to #coord structure.
 * \return the magnitude of \p pos
 */
inline double coord_get_magnitude(const coord *pos)
{
  return (sqrt(pos->x*pos->x + pos->y*pos->y + pos->z*pos->z));
}

/*!
 * Setter function for @coord::x
 *
 * \param[in,out] pos Pointer to #coord object to change.
 * \param new_x value of x position.
 */
inline void coord_set_x(coord *pos, double new_x)
{
  pos->x = new_x;
}

/*!
 * Setter function for @coord::y
 *
 * \param[in,out] pos Pointer to #coord object to change.
 * \param new_y value of y position.
 */
inline void coord_set_y(coord *pos, double new_y)
{
  pos->y = new_y;
}

/*!
 * Setter function for @coord::z
 *
 * \param[in,out] pos Pointer to #coord object to change.
 * \param new_z value of z position.
 */
inline void coord_set_z(coord *pos, double new_z)
{
  pos->z = new_z;
}

/*!
 * Setter function for assigning (\p x, \p y, \p z) coordinate \p pos
 *
 * \param[in,out] pos Pointer to #coord object to change.
 * \param x value of x-axis position.
 * \param y value of y-axis position.
 * \param z value of z-axis position.
 */
inline void coord_set(coord *pos, double x, double y, double z)
{
  pos->x = x; 
  pos->y = y; 
  pos->z = z;
}

/*!
 * Prints (x, y, z) coordinates of \p pos
 *
 * \param f file pointer to print output to e.g. \c stdout
 * \param pos source of coordinates.
 */
inline void coord_printf(FILE *f, const coord *pos)
{
  if (pos != NULL)
    fprintf(f, "x = %7.4f, y = %7.4f, z = %7.4f\n", pos->x, pos->y, pos->z);
  else 
    fprintf(f, "NULL\n");
}
