/************************************************************************
 * coord.h
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description: Header provides an xyz coordinate data type and functions
 * Change Log:
 *   2014/07/30 - Creation
 ************************************************************************/
 
#ifndef COORD_H
#define COORD_H

#define COORD_COPY(a, b) {a->x = b->x; a->y = b->y; a->z = b->z;}
#define COORD_COPY_XYZ(a, x0, y0, z0) {a->x = x0; a->y = y0; a->z = z0;}
#define COORD_MAGNITUDE(a) (sqrt(a->x*a->x + a->y*a->y + a->z*a->z))

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

typedef struct {
  double x;
  double y;
  double z;
} coord_t;

coord_t *coord_init();
void coord_free(coord_t *pos);
double coord_get_x(const coord_t *pos);
double coord_get_y(const coord_t *pos);
double coord_get_z(const coord_t *pos);
double coord_get_magnitude(const coord_t *pos);
size_t coord_get_allocated_size(const coord_t *pos);
void coord_set_x(coord_t *pos, double x);
void coord_set_y(coord_t *pos, double y);
void coord_set_z(coord_t *pos, double z);
void coord_set(coord_t *pos, double x, double y, double z);
void coord_copy(coord_t *dst, const coord_t *src);
void coord_printf(FILE *f, const coord_t *pos);

#ifdef __cplusplus
}

namespace cleed {

class Coordinate {

  public:
    explicit Coordinate() : pos(::coord_init()) {}
    Coordinate(double x, double y, double z);
    Coordinate(const Coordinate &position);
    Coordinate(const coord_t *position);
    ~Coordinate();
    
    /* setters */
    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setCoordinate(double x, double y, double z);
    void setCoordinate(const Coordinate &position);
    void setCoordinate(const coord_t *position);
    
    /* getters */
    double getMagnitude();
    double getX();
    double getY();
    double getZ();
    const coord_t *get_coord_t();
    
    /* other methods */
    void print(FILE *f = stdout);
    
  protected:
    coord_t *pos;
};

} /* namespace cleed */
#endif

#endif /* COORD_H */
