/*********************************************************************
 *                           COORD.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *********************************************************************/

/*! \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 * Implements methods for manipulating #coord objects.
 */

#include "coord.h"
#include <stdlib.h>
#include <math.h>

/*!
 * Initializes #coord instance with a position (0, 0, 0).
 *
 * \return pointer to #coord object
 * \retval \c NULL if memory could not be allocated.
 */
coord *coord_init()
{
  coord *pos = (coord*) malloc(sizeof(coord));
  
  if (pos == NULL) return NULL;   /* memory could not be allocated */

  pos->x = 0.;
  pos->y = 0.;
  pos->z = 0.;

  return (pos);
}

/*!
 * Frees #coord object \p pos from memory.
 *
 * \param pos object to free.
 */
void coord_free(coord *pos)
{
  if (pos != NULL)
	free(pos);
  pos = NULL;
}

/*!
 *
 * \param pos
 * \return The maximum number of #coord objects with allocated memory to \p pos.
 */
size_t coord_get_allocated_size(const coord *pos)
{
  return (sizeof(pos)/sizeof(coord));
}

/*!
 * Copies #coord data from \p src into \p dst
 *
 * \param dst destination for coordinate data.
 * \param src source of coordinate data.
 */
void coord_copy(coord *dst, const coord *src)
{
  coord_set(dst, src->x, src->y, src->z);
}

/*!
 * Getter function for @coord::x
 *
 * \param pos
 * \return @coord::x of \p pos
 */
double coord_get_x(const coord *pos)
{
  return (pos->x);
}

/*!
 * Getter function for @coord::y
 *
 * \param pos
 * \return @coord::y of \p pos
 */
double coord_get_y(const coord *pos)
{
  return (pos->y);
}

/*!
 * Getter function for @coord::z
 *
 * \param pos
 * \return @coord::z of \p pos
 */

double coord_get_z(const coord *pos)
{
  return (pos->z);
}

/*!
 * Calculates the magnitude of the vector given by \p pos
 *
 * \param pos
 * \return the magnitude of \p pos
 */
double coord_get_magnitude(const coord *pos)
{
  return (sqrt(pos->x*pos->x + pos->y*pos->y + pos->z*pos->z));
}

/*!
 * Setter function for @coord::x
 *
 * \param pos #coord object to change.
 * \param x value of x position.
 */
void coord_set_x(coord *pos, double x)
{
  pos->x = x;
}

/*!
 * Setter function for @coord::y
 *
 * \param pos #coord object to change.
 * \param y value of y position.
 */
void coord_set_y(coord *pos, double y)
{
  pos->y = y;
}

/*!
 * Setter function for @coord::z
 *
 * \param pos #coord object to change.
 * \param z value of z position.
 */
void coord_set_z(coord *pos, double z)
{
  pos->z = z;
}

/*!
 * Setter function for assigning (@p x, \p y, \p z) coordinate \p pos
 *
 * \param pos #coord object to change.
 * \param x value of x-axis position.
 * \param y value of y-axis position.
 * \param z value of z-axis position.
 */
void coord_set(coord *pos, double x, double y, double z)
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
void coord_printf(FILE *f, const coord *pos)
{
  if (pos != NULL)
  {
    fprintf(f, "x = %7.4f, y = %7.4f, z = %7.4f\n", pos->x, pos->y, pos->z);
  }
  else 
  {
    fprintf(f, "NULL\n");
  }
}
