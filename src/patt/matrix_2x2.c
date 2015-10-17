/*********************************************************************
 *                           MATRIX_2X2.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Functions for handling #matrix_2x2 objects.
 */

#include "matrix_2x2.h"
#include <string.h>

/*!
 * Prints values of 2x2 matrix \p mat to file \p stream
 */
void matrix_2x2_printf(FILE *stream, matrix_2x2 *mat)
{
  fprintf(stream, "M = %9.6f, %9.6f\n    %9.6f, %9.6f\n", 
          mat->M11, mat->M12, mat->M21, mat->M22);
}

/*!
 * Sets matrix elements of \p mat
 *
 * \param mat Matrix to be modified.
 * \param m11
 * \param m12
 * \param m21
 * \param m22
 */
void matrix_2x2_set_all(matrix_2x2 *mat, double m11, double m12,
                                           double m21, double m22)
{
  mat->M11 = m11;
  mat->M12 = m12;
  mat->M21 = m21;
  mat->M22 = m22;
}

/*!
 * Returns the value of the element of \p mat specified by \p col and \p row
 *
 * \param mat 2x2 matrix to query.
 * \param col column of \p mat
 * \param row row of \p mat
 * \return value of matrix element.
 */
double matrix_2x2_get_element(matrix_2x2 *mat, size_t col, size_t row)
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

void matrix_2x2_set_element(matrix_2x2 *mat, size_t col, 
                            size_t row, double value)
{ 
  if (col > 1 || row > 1) return;
  
  if (col == 0 && row == 0) mat->M11 = value;
  else if (col == 0 && row == 1) mat->M12 = value;
  else if (col == 1 && row == 0) mat->M21 = value;
  else if (col == 1 && row == 1) mat->M22 = value;
}

matrix_2x2 *matrix_2x2_init()
{
  matrix_2x2 *mat = (matrix_2x2*) malloc(sizeof(matrix_2x2));
  mat->M11 = 0.;
  mat->M12 = 0.;
  mat->M21 = 0.;
  mat->M22 = 0.;
  return mat;
}

matrix_2x2 *matrix_2x2_init_all(double m11, double m12, 
    double m21, double m22)
{
  matrix_2x2 *mat = (matrix_2x2*) malloc(sizeof(matrix_2x2));
  mat->M11 = m11;
  mat->M12 = m12;
  mat->M21 = m21;
  mat->M22 = m22;
  return mat;
}

/*!
 * Frees \p mat from memory.
 *
 * \param mat Pointer to #matrix_2x2 instance to be freed.
 */
void matrix_2x2_free(matrix_2x2 *mat)
{
  free(mat);
}

/*!
 * Copies
 *
 * \param mat
 * \return Pointer to a copy of \p mat
 */
matrix_2x2 *matrix_2x2_copy(const matrix_2x2 *mat)
{
  matrix_2x2 *copy = (matrix_2x2*) mat;
  return copy;
}
