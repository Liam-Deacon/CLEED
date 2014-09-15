#include "patt.h"

struct _matrix_2x2
{
  double M11;
  double M12;
  double M21;
  double M22;

} matrix_default = {0., 0., 0., 0.};

typedef _matrix_2x2 matrix_2x2;

struct _vector_xy
{
  double x;
  double y;
} xy_vector_default = {0., 0.};

typedef _vector_xy vector_xy;

struct _pattern
{
  char *title;
  vector_xy a1;
  vector_xy a2;
  matrix_2x2 M_GS;
  matrix_2x2 *M_SS;
  size_t n_domains;
  double radius;
};

typedef _pattern pattern;