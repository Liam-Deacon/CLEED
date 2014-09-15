#include "matrix_2x2.h"
#include <string.h>

void matrix_2x2_printf(FILE *stream, matrix_2x2_t *mat)
{
  fprintf(stream, "M = %9.6f, %9.6f\n    %9.6f, %9.6f\n", 
          mat->M11, mat->M12, mat->M21, mat->M22);
}

void matrix_2x2_set_all(matrix_2x2_t *mat, double m11, double m12,
                                           double m21, double m22)
{
  mat->M11 = m11;
  mat->M12 = m12;
  mat->M21 = m21;
  mat->M22 = m22;
}

double matrix_2x2_get_element(matrix_2x2_t *mat, size_t col, size_t row)
{
  double matrix[2][2];
  
  col++;
  row++;
  
  matrix[0][0] = mat->M11;
  matrix[0][1] = mat->M12;
  matrix[1][0] = mat->M21;
  matrix[1][1] = mat->M22;
  
  if (col > 2 || row > 2) {
    return 0.;
  }
  else {
    return matrix[col-1][row-1];
  }
}

void matrix_2x2_set_element(matrix_2x2_t *mat, size_t col, 
                            size_t row, double value)
{ 
  if (col > 1 || row > 1) return;
  
  if (col == 0 && row == 0) mat->M11 = value;
  else if (col == 0 && row == 1) mat->M12 = value;
  else if (col == 1 && row == 0) mat->M21 = value;
  else if (col == 1 && row == 1) mat->M22 = value;
}

matrix_2x2_t *matrix_2x2_init()
{
  matrix_2x2_t *mat = (matrix_2x2_t*) malloc(sizeof(matrix_2x2_t));
  mat->M11 = 0.;
  mat->M12 = 0.;
  mat->M21 = 0.;
  mat->M22 = 0.;
  return mat;
}

matrix_2x2_t *matrix_2x2_init_all(double m11, double m12, 
    double m21, double m22)
{
  matrix_2x2_t *mat = (matrix_2x2_t*) malloc(sizeof(matrix_2x2_t));
  mat->M11 = m11;
  mat->M12 = m12;
  mat->M21 = m21;
  mat->M22 = m22;
  return mat;
}

void matrix_2x2_free(matrix_2x2_t *mat)
{
  free(mat);
}

matrix_2x2_t *matrix_2x2_copy(matrix_2x2_t *mat)
{
  matrix_2x2_t *copy = mat; 
  return copy;
}