/**
 * @file mat_def.h
 * @brief Type definitions and constants for matrix operations.
 *
 * This file provides:
 * - Matrix type flags and magic numbers
 * - Element access macros (RMATEL, IMATEL)
 * - The mat structure definition
 *
 * @ingroup matrix_ops
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef MAT_DEF_H
#define MAT_DEF_H

/**
 * @defgroup matrix_types Matrix Type Flags
 * @ingroup matrix_ops
 * @brief Constants for matrix type identification.
 * @{
 */

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

/** @brief Magic number to identify valid matrix structures */
#define MATRIX         0xaffe

/* 
 * matrix types:
 * Use only lower half of high bytes for mat_type, 
 * i.e. 0x0FFF >= MAT_* > 0x00FF (NUM_MASK)
 */
#define MAT_MASK       0x0F00   /**< Mask for matrix type bits */

#define MAT_NORMAL     0x0100   /**< M x N rectangular matrix */
#define MAT_SQUARE     0x0200   /**< M x M square matrix */
#define MAT_SCALAR     0x0300   /**< 1 x 1 scalar matrix */
#define MAT_DIAG       0x0800   /**< Diagonal matrix (must be highest number) */

/*
 * block types:
 * Use only high half of high bytes for mat_type, 
 * i.e. 0xFFFF >= BLK_* > 0x0FFF (NUM_MASK)
 */

#define BLK_MASK       0xF000   /**< Mask for block type bits */

#define BLK_SINGLE     0x1000   /**< Single matrix (not part of array) */
#define BLK_ARRAY      0x4000   /**< Part of a matrix array */
#define BLK_END        0x5000   /**< Terminator of a matrix array */

/*
 * number types:
 * Use only low bytes for num_type, i.e. NUM_* <= 0xFF (NUM_MASK)
*/
#define NUM_MASK    0xFF        /**< Mask for number type bits */

#define NUM_REAL    0x02        /**< Real-valued matrix */
#define NUM_IMAG    0x03        /**< Imaginary-valued matrix */
#define NUM_COMPLEX 0x04        /**< Complex-valued matrix */

/** @} */ /* end of matrix_types group */

/**
 * @defgroup matrix_access Element Access Macros
 * @ingroup matrix_ops
 * @brief Macros for accessing matrix elements.
 * @{
 */

/*********************************************************************
Macros for matrix operations
*********************************************************************/

/**
 * @brief Access real part of matrix element (m,n).
 *
 * @param m Row index (1-based).
 * @param n Column index (1-based).
 * @param Mat Pointer to matrix structure.
 * @return Reference to the real part of element (m,n).
 *
 * @note m and n must be integers; Mat must be of type mat.
 */
#define RMATEL(m,n,Mat) *((Mat)->rel + ((m)-1) * (Mat)->cols + (n))

/**
 * @brief Access imaginary part of matrix element (m,n).
 *
 * @param m Row index (1-based).
 * @param n Column index (1-based).
 * @param Mat Pointer to matrix structure.
 * @return Reference to the imaginary part of element (m,n).
 *
 * @note m and n must be integers; Mat must be of type mat.
 */
#define IMATEL(m,n,Mat) *((Mat)->iel + ((m)-1) * (Mat)->cols + (n))

/** @} */ /* end of matrix_access group */

/**
 * @defgroup matrix_struct Matrix Structure
 * @ingroup matrix_ops
 * @brief The core matrix data structure.
 * @{
 */

/**
 * @brief Real or complex matrix structure.
 *
 * The matrix element (m,n) is stored at position ((m-1)*cols + n)
 * in the rel (real) or iel (imaginary) arrays.
 *
 * @note Uses 1-based indexing for compatibility with Fortran-style code.
 */
struct mat_str
{
  int mag_no;        /**< Magic number (MATRIX = 0xaffe) for validation */
  int blk_type;      /**< Block type (single, array, end) */
  int mat_type;      /**< Matrix type (normal, square, diagonal) */
  int num_type;      /**< Element type (real, imaginary, complex) */ 
  int rows;          /**< Number of rows (1st dimension) */
  int cols;          /**< Number of columns (2nd dimension) */
  real *rel;         /**< Pointer to real matrix elements */ 
  real *iel;         /**< Pointer to imaginary matrix elements */
};

/**
 * @brief Pointer to matrix structure (main handle for matrix operations).
 */
typedef struct mat_str*  mat;

/** @} */ /* end of matrix_struct group */

#endif /* MAT_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif