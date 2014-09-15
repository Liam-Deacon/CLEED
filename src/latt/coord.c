#include "coord.h"
#include <stdlib.h>
#include <math.h>

coord_t *coord_init()
{
  coord_t *pos = (coord_t*) malloc(sizeof(coord_t));
  
  pos->x = 0.;
  pos->y = 0.;
  pos->z = 0.;

  return (pos);
}

void coord_free(coord_t *pos)
{
  free(pos);
  pos = NULL;
}

size_t coord_get_allocated_size(const coord_t *pos)
{
  return (sizeof(pos)/sizeof(coord_t));
}

void coord_copy(coord_t *dst, const coord_t *src)
{
  coord_set(dst, src->x, src->y, src->z);
}

double coord_get_x(const coord_t *pos)
{
  return (pos->x);
}

double coord_get_y(const coord_t *pos)
{
  return (pos->y);
}

double coord_get_z(const coord_t *pos)
{
  return (pos->z);
}

double coord_get_magnitude(const coord_t *pos)
{
  return (sqrt(pos->x*pos->x + pos->y*pos->y + pos->z*pos->z));
}

void coord_set_x(coord_t *pos, double x)
{
  pos->x = x;
}

void coord_set_y(coord_t *pos, double y)
{
  pos->y = y;
}

void coord_set_z(coord_t *pos, double z)
{
  pos->z = z;
}

void coord_set(coord_t *pos, double x, double y, double z)
{
  pos->x = x; 
  pos->y = y; 
  pos->z = z;
}

void coord_printf(FILE *f, const coord_t *pos)
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
