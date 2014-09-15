#include "matrix_2x2.h"

using namespace cleed;


void Matrix2x2::print(FILE *stream) 
{
::matrix_2x2_printf(stream, mat);
}

void Matrix2x2::setMatrix(double m11, double m12, double m21, double m22)
{
  ::matrix_2x2_set_all(mat, m11, m12, m21, m22);
}

void Matrix2x2::setMatrixElement(std::size_t row, std::size_t col, double value)
{
  ::matrix_2x2_set_element(mat, col, row, value);
}

Matrix2x2::~Matrix2x2() { ::matrix_2x2_free(mat); }

const std::vector<double> Matrix2x2::getAllMatrixElements()
{
  std::vector<double> l(4);

}