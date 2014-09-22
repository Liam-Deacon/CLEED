#include "matrix_2x2.h"
#include <stdio.h>

int main()
{
  cleed::Matrix2x2 mat(2.,2,3,4);
  mat.setMatrixElement(1,1, 10.);
  mat.print();
  return 0;
}