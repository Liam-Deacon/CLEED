#include "patt.h"

typedef struct _matrix_2x2
{
  double M11;
  double M12;
  double M21;
  double M22;

} matrix_default = {0., 0., 0., 0.};

typedef struct _matrix_2x2 matrix_2x2_t;

struct _vector_xy
{
  double x;
  double y;
} xy_vector_default = {0., 0.};

typedef struct _vector_xy vector_xy_t;

typedef struct pattern
{
  char *title;
  vector_xy_t a1;
  vector_xy_t a2;
  matrix_2x2_t M_GS;
  matrix_2x2_t *M_SS;
  size_t n_domains;
  double radius;
} pattern_t;
