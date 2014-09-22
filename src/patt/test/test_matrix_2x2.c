#include "matrix_2x2.h"
#include <stdio.h>

int main()
{
  matrix_2x2_t *mat = matrix_2x2_init_all(1., 2., 3., 4.);
  matrix_2x2_printf(stdout, mat);
  return 0;
}