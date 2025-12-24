/*********************************************************************
 *													Coordinate.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/23.01.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Header providing a C++ object-orientated wrapper class for the #coord \c
 * struct to provide a more intuitive interface.
 */


#ifndef __COORDINATE_HH__
#define __COORDINATE_HH__

#ifdef __cplusplus
#include <iostream>
#include "coord.h"

namespace cleed {

  class Coordinate : public coord {
    public:
      Coordinate();
      Coordinate(double x, double y, double z);
      Coordinate(const Coordinate &position);
      Coordinate(const coord *position);
      ~Coordinate();

      friend std::ostream& operator<<(std::ostream& out, const Coordinate& pos);

      // setters
      Coordinate& setX(double x);
      Coordinate& setY(double y);
      Coordinate& setZ(double z);
      Coordinate& setCoordinate(double x, double y, double z);
      Coordinate& setCoordinate(const Coordinate &position);
      Coordinate& setCoordinate(const coord *position);

      // getters
      double getMagnitude();
      double getX();
      double getY();
      double getZ();
      const coord *get_coord();

      // other methods
      void print(FILE *f = stdout);
    public:
      coord *pos;
  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __COORDINATE_HH__ */
