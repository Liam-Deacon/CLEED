/*********************************************************************
 *                           COORD++.CC
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
 * Implements the #Coordinate class, which provides an object-orientated
 * wrapping to the C #coord struct and its associated methods.
 */

#include <string>
#include "Coordinate.hh"

using namespace cleed;

Coordinate::Coordinate(double x, double y, double z)
{
  this->pos = ::coord_init();
  ::coord_set_x(this->pos, x);
  ::coord_set_y(this->pos, y);
  ::coord_set_z(this->pos, z);
}

Coordinate::Coordinate(const Coordinate &position)
{
  this->pos = ::coord_init();
  ::coord_copy(this->pos, position.pos);
}

Coordinate::Coordinate(const coord *position)
{
  this->pos = ::coord_init();
  ::coord_copy(this->pos, position);
}

Coordinate::~Coordinate()
{
  ::coord_free(this->pos);
}
    
/* setters */
inline Coordinate& Coordinate::setX(double x)
{
  ::coord_set_x(this->pos, x);
  return *this;
}

inline Coordinate& Coordinate::setY(double y)
{
  ::coord_set_y(this->pos, y);
  return *this;
}

inline Coordinate& Coordinate::setZ(double z)
{
  ::coord_set_z(this->pos, z);
  return *this;
}

inline Coordinate& Coordinate::setCoordinate(double x, double y, double z)
{
  ::coord_set(this->pos, x, y, z);
  return *this;
}

inline Coordinate& Coordinate::setCoordinate(const Coordinate &position)
{
  this->pos = position.pos;
  return *this;
}

inline Coordinate& Coordinate::setCoordinate(const coord *position)
{
  pos = (coord*) position;
  return *this;
}
    
/* getters */
inline double Coordinate::getMagnitude()
{
  return (coord_get_magnitude(this->pos));
}

inline double Coordinate::getX()
{
  return (coord_get_x(this->pos));
}

inline double Coordinate::getY()
{
  return (coord_get_y(this->pos));
}

inline double Coordinate::getZ()
{
  return (coord_get_z(this->pos));
}

inline const coord *Coordinate::get_coord()
{
  return ((const coord*) this->pos);
}

/* other methods */
inline void Coordinate::print(FILE *f)
{
  ::coord_printf(f, this->pos);
}

std::ostream& operator<<(std::ostream &out, const Coordinate &pos)
{
  const std::string &delim = std::string(", ");
  out << pos.x << delim << pos.y << delim << pos.z;
  return out;
}
