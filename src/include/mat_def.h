/*********************************************************************
 *                           MAT_DEF.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1994.06.06 - creation
 *   LD/2014.08.10 - added Doygen markup
 *                 - changed rows & cols of mat from int to size_t type
 *********************************************************************/

/*!
 * \file
 * \brief Include file for matrix operations. It defines \c struct #mat
 * and associated definitions.
 */

#ifndef MAT_DEF_H
#define MAT_DEF_H

/*********************************************************************
 * structures and types for matrices
 *********************************************************************/
#include <stdio.h>
#include "cleed_real.h" /* type definition for real */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

/*********************************************************************
 * values for:
 *  - mat_type
 *  - num_type
 *********************************************************************/

enum { MATRIX = 0xaffe };   /*!< magic number for matrix */

/*
 * matrix types:
 * Use only lower half of high bytes for mat_type, 
 * i.e. 0x0FFF >= MAT_* > 0x00FF (NUM_MASK)
 */
typedef enum {
  MAT_MASK = 0x0F00,
  MAT_NORMAL = 0x0100,   /*!< M x N matrix */
  MAT_SQUARE = 0x0200,   /*!< M x M matrix */
  MAT_SCALAR = 0x0300,   /*!< 1 x 1 matrix */
  MAT_DIAG = 0x0800      /*!< diagonal matrix (must be highest number) */
} mat_type;

/*
 * block types:
 * Use only high half of high bytes for mat_type, 
 * i.e. 0xFFFF >= BLK_* > 0x0FFF (NUM_MASK)
 */

typedef enum {
  BLK_MASK = 0xF000,
  BLK_SINGLE = 0x1000,    /*!< single matrix, i.e. not part of an array */
  BLK_ARRAY = 0x4000,     /*!< part of a matrix array */
  BLK_END = 0x5000        /*!< terminator of a matrix array */
} mat_blk;

/*
 * number types:
 * Use only low bytes for num_type, i.e. NUM_* <= 0xFF (NUM_MASK)
 */
typedef enum {
  NUM_MASK = 0xFF,
  NUM_REAL = 0x02,
  NUM_IMAG = 0x03,
  NUM_COMPLEX = 0x04
} mat_enum;

/*********************************************************************
 * Macros for matrix operations
 *********************************************************************/

/*!
 * Access real matrix element \$f (m,n) \$f of matrix Mat.
 *
 * \c m and \c n must be integers,
 * Mat must be of type #mat.
 */
#define RMATEL(m,n,Mat) *((Mat)->rel + ((m)-1) * (Mat)->cols + (n))
/*!
 * Access imaginary matrix element \f$ (m,n) \f$ of matrix Mat.
 *
 * \c m and \c n must be integers,
 * Mat must be of type #mat.
 */
#define IMATEL(m,n,Mat) *((Mat)->iel + ((m)-1) * (Mat)->cols + (n))


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*! \struct mat_str
 *  \brief type definition for mat
 *
 * \note The matrix element (m,n) is in the ((m-1)*rows + n)th position of
 * the arrays rel or iel, respectively.
 */
struct mat_str      /*!< real or complex matrix */
{
  int mag_no;        /*!< magic number */
  int blk_type;      /*!< type of matrix array (single, block) */
  mat_type mat_type; /*!< type of matrix (square, real etc.) */
  mat_enum num_type; /*!< type of matrix elements */
  size_t rows;       /*!< 1st dimension of matrix (number of rows) */
  size_t cols;       /*!< 2nd dimension of matrix (number of columns) */
  real *rel;         /*!< pointer to real matrix elements */ 
  real *iel;         /*!< pointer to complex matrix elements */
};

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
#endif

/*! \typedef mat
 *  \brief pointer to the upper matrix structure (used to refer to a matrix)
 */
typedef struct mat_str* mat;

/*!
 * Access real matrix element \$f (i,j) \$f of matrix Mat.
 *
 * \param i Row number
 * \param j Column number
 * \param Mat Pointer to CLEED matrix
 *
 * \note \p m and \p n must be positive integers, \p Mat must be of type #mat.
 * \note \p Mat is stored in row-major order.
 */
static inline real *rmatel(size_t i, size_t j, mat Mat)
{
  return (Mat->rel + (i-1) * Mat->cols + j);
}
/*!
 * Access imaginary matrix element \f$ (i,j) \f$ of matrix Mat.
 *
 * \param i Row number
 * \param j Column number
 * \param Mat Pointer to CLEED matrix
 *
 * \note \p m and \p n must be positive integers, \p Mat must be of type #mat.
 * \note \p Mat is stored in row-major order.
 */
static inline real *imatel(size_t i, size_t j, mat Mat)
{
  return (Mat->iel + ((i-1) * Mat->cols + j));
}

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */
#endif

#endif /* MAT_DEF_H */
