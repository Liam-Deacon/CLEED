#include "coord.h"
#include <stdlib.h>
#include <math.h>

coord *coord_init()
{
  coord *pos = (coord*) malloc(sizeof(coord));
  
  pos->x = 0.;
  pos->y = 0.;
  pos->z = 0.;

  return (pos);
}

void coord_free(coord *pos)
{
  if (pos != NULL)
	free(pos);
  pos = NULL;
}

size_t coord_get_allocated_size(const coord *pos)
{
  return (sizeof(pos)/sizeof(coord));
}

void coord_copy(coord *dst, const coord *src)
{
  coord_set(dst, src->x, src->y, src->z);
}

double coord_get_x(const coord *pos)
{
  return (pos->x);
}

double coord_get_y(const coord *pos)
{
  return (pos->y);
}

double coord_get_z(const coord *pos)
{
  return (pos->z);
}

double coord_get_magnitude(const coord *pos)
{
  return (sqrt(pos->x*pos->x + pos->y*pos->y + pos->z*pos->z));
}

void coord_set_x(coord *pos, double x)
{
  pos->x = x;
}

void coord_set_y(coord *pos, double y)
{
  pos->y = y;
}

void coord_set_z(coord *pos, double z)
{
  pos->z = z;
}

void coord_set(coord *pos, double x, double y, double z)
{
  pos->x = x; 
  pos->y = y; 
  pos->z = z;
}

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
