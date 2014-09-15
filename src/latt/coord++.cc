#include "coord.h"

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
  this->pos = position.pos;
}

Coordinate::Coordinate(const coord_t *position)
{
  ::coord_copy(this->pos, position);
}

Coordinate::~Coordinate()
{
  ::coord_free(this->pos);
}
    
/* setters */
void Coordinate::setX(double x)
{
  ::coord_set_x(this->pos, x);
}

void Coordinate::setY(double y)
{
  ::coord_set_y(this->pos, y);
}

void Coordinate::setZ(double z)
{
  ::coord_set_z(this->pos, z);
}

void Coordinate::setCoordinate(double x, double y, double z)
{
  ::coord_set(this->pos, x, y, z);
}

void Coordinate::setCoordinate(const Coordinate &position)
{
  this->pos = position.pos;
}

void Coordinate::setCoordinate(const coord_t *position)
{
  pos = (coord_t*) position;
}
    
/* getters */
double Coordinate::getMagnitude()
{
  return (coord_get_magnitude(this->pos));
}

double Coordinate::getX()
{
  return (coord_get_x(this->pos));
}

double Coordinate::getY()
{
  return (coord_get_y(this->pos));
}

double Coordinate::getZ()
{
  return (coord_get_z(this->pos));
}

const coord_t *Coordinate::get_coord_t()
{
  return ((const coord_t*) this->pos);
}

/* other methods */
void Coordinate::print(FILE *f)
{
  ::coord_printf(f, this->pos);
}
