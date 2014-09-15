/*********************************************************************
GH/10.02.94

functions for matrix operations
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef MAT_FUNC_H
#define MAT_FUNC_H

#include "real.h"

  /* modulus of a matrix */
real matabs(mat);
  /* allocate matrix memory  in file matalloc.c*/
mat matalloc(mat, int, int, int);
  /* allocate array of matrices in file matarralloc.c*/
mat matarralloc(mat, int);
  /* free array of matrices in file matarrfree.c*/
int matarrfree(mat);
  /* check validity of pointer */
int matcheck(mat); 
  /*  extract a column from a matrix */
mat matcol(mat, mat, int); 
  /* transpose and conjugate a matrix */
mat matconj(mat, mat); 
  /* Copy matrices */
mat matcop(mat, mat);
  /* difference between matrices */
real matdiff(mat, mat);
  /* Extract a submatrix from a larger one */
mat matext(mat , mat , int , int , int , int );
  /* Free memory allocated for a matrix */
int matfree(mat);
  /* matrix inversion in file matcgau.c */
mat matgausinv(mat);
  /* copy one matrix into another */
mat matins(mat, mat, int, int);
  /* matrix inversion in file matinv.c */
mat matinv(mat, mat);
mat matinv_old(mat, mat);
  /* matrix multiplication in file matmul.c */
mat matmul(mat, mat, mat);
  /* convert order */
int matnattovht (mat , int, int );
int matline( mat , int , int , int , int );
  /*  extract a row from a matrix */
mat matrow(mat, mat, int); 
  /* matrix multiplication with complex number in file matscal.c */  
mat matscal(mat, mat, real, real); 
  /* print a matrix in file matshow.c */  
int matshow(mat); 
  /* print the modulus of a matrix in file matshow.c */  
int matshowabs(mat); 
  /* print the parameters of a matrix in file matshowpar.c */  
int matshowpar(mat); 
  /* square modulus of a matrix */
mat matsqmod(mat, mat); 
  /* trace of a matrix */
int mattrace(mat, mat); 
  /* transpose a matrix */
mat mattrans(mat, mat); 

/*********************************************************************
 read and write
 <stdio.h> for type definition of FILE
*********************************************************************/
#include <stdio.h>

int matwrite(mat , FILE *);
mat matread (mat , FILE *);
mat matrdlm (mat , int, const char *);


/*********************************************************************
lower level functions
*********************************************************************/
  /* LU decomposition in file ludcmp.c */
int  ludcmp( real *, int *, int);
  /* LU decomposition (real) in file matrlu.c */
int  r_ludcmp( real *, int *, int);
int  r_luinv( real *, real *, int *, int);
real *r_lubksb( real *, int *, real *, int);
  /* LU decomposition (complex) in file matclu.c */
int  c_ludcmp( real *, real *, int *, int);
int  c_luinv( real *, real *, real *, real *, int *, int);
int  c_lubksb( real *, real *, int *, real *, real *, int);
  /* matrix multiplication for square mat. in file matrm.c */
real * r_sqmul( real *, real *, real *, int);

#endif /* MAT_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif