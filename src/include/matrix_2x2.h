/*********************************************************************
 *                           MATRIX_2X2.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: header for matrix_2x2 and associated function
 *              prototypes.
 *
 * Changes:
 *   LD/2014.08.23 - creation
 *********************************************************************/

#ifndef MATRIX_2X2_H
#define MATRIX_2X2_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
#include <cstddef>
#include <vector>

  extern "C" {
#endif

/*! \typedef \struct matrix_2x2
 *  \brief a 2x2 element matrix type.
 */
typedef struct matrix_2x2 
{
  double M11;
  double M12;
  double M21;
  double M22;

} matrix_2x2;

extern matrix_2x2 matrix_2x2_default;

/* matrix_2x2 functions */
matrix_2x2 *matrix_2x2_init();
matrix_2x2 *matrix_2x2_init_all(double m11, double m12, 
                                  double m21, double m22);
void matrix_2x2_set_element(matrix_2x2 *mat, size_t col, 
                            size_t row, double value);
void matrix_2x2_set_all(matrix_2x2 *mat, double m11, double m12, 
                                           double m21, double m22);
void matrix_2x2_printf(FILE *stream, matrix_2x2 *mat);
double matrix_2x2_get_element(matrix_2x2 *mat, size_t col, size_t row);
void matrix_2x2_free(matrix_2x2 *mat);
matrix_2x2 *matrix_2x2_copy(matrix_2x2 *mat);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

class Matrix2x2 {
  public:
    explicit Matrix2x2() : mat(::matrix_2x2_init()) {}
    explicit Matrix2x2(double m11, double m12, double m21, double m22
                        ): mat(::matrix_2x2_init_all(m11, m12, m21, m22)) {}
    Matrix2x2(const Matrix2x2 &Mat) : mat (::matrix_2x2_copy(Mat.mat)) {}
    ~Matrix2x2();
    
    void setMatrix(double m11, double m12, double m21, double m22);
    void setMatrixElement(std::size_t row, std::size_t col, double value = 0.);
    void print(FILE *stream = stdout);
    
    const std::vector<double> getAllMatrixElements();
    const double getSingleMatrixElement(std::size_t row, std::size_t col);
    
  private:
    matrix_2x2 *mat;
};

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* MATRIX_2X2_H */
