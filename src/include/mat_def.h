/*********************************************************************
GH/06.06.94

include file for matrix operations:
 - type definitions
 - constant values

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef MAT_DEF_H
#define MAT_DEF_H

/*********************************************************************
 structures and types for matrices
*********************************************************************/
/*
 * type definition for real
 */
#include "real.h"

/*********************************************************************
 values for:
 mat_type
 num_type 
*********************************************************************/
/* 
 * magic number
 */
#define MATRIX         0xaffe

/* 
 * matrix types:
 * Use only lower half of high bytes for mat_type, 
 * i.e. 0x0FFF >= MAT_* > 0x00FF (NUM_MASK)
 */
#define MAT_MASK       0x0F00

#define MAT_NORMAL     0x0100   /* M x N matrix */
#define MAT_SQUARE     0x0200   /* M x M matrix */
#define MAT_SCALAR     0x0300   /* 1 x 1 matrix */

#define MAT_DIAG       0x0800   /* diagonal matrix (must be highest number) */

/*
 * block types:
 * Use only high half of high bytes for mat_type, 
 * i.e. 0xFFFF >= BLK_* > 0x0FFF (NUM_MASK)
 */

#define BLK_MASK       0xF000

#define BLK_SINGLE     0x1000   /* single matrix, i.e. not part of an array */
#define BLK_ARRAY      0x4000   /* part of a matrix array */
#define BLK_END        0x5000   /* terminator of a matrix array */

/*
 * number types:
 * Use only low bytes for num_type, i.e. NUM_* <= 0xFF (NUM_MASK)
*/
#define NUM_MASK    0xFF

#define NUM_REAL    0x02
#define NUM_IMAG    0x03
#define NUM_COMPLEX 0x04

/*********************************************************************
Macros for matrix operations
*********************************************************************/
/*
 * R(I)MATEL(m,n,Mat) - access matrix element (m,n) of matrix Mat (mat)
 *
 * m and n must be integers,
 * Mat must be of type mat.
 */

#define RMATEL(m,n,Mat) *((Mat)->rel + ((m)-1) * (Mat)->cols + (n))
#define IMATEL(m,n,Mat) *((Mat)->iel + ((m)-1) * (Mat)->cols + (n))

/*
 * type definition for mat
 */

struct mat_str      /* real or complex matrix */
{
  int mag_no;        /*!< magic number */
  int blk_type;      /*!< type of matrix array (single, block) */
  int mat_type;      /*!< type of matrix (square, real etc.) */
  int num_type;      /*!< type of matrix elements */ 
  int rows;          /*!< 1st dimension of matrix (number of rows) */
  int cols;          /*!< 2nd dimension of matrix (number of columns) */
  real *rel;         /*!< pointer to real matrix elements */ 
  real *iel;         /*!< pointer to complex matrix elements */
/* 
 * The matrix element (m,n) is in the ((m-1)*dim1 + n)th position of the arrays
 * del or dcel, respectively.
 */
};

/*
 * pointer to the upper matrix structure (used to refer to a matrix)
 */
typedef struct mat_str*  mat;

#endif /* MAT_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif