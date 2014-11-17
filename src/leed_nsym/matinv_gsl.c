#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>

static gsl_matrix_t *mat_to_gsl_matrix(mat M, real *elements_ptr)
{
  size_t i_row, i_col;
  gsl_matrix *G = gsl_matrix_alloc(M->rows, M->cols);
  
  for (i_row = 0; i_row < M->rows; i_row++)
  {
    for (i_col = 0; i_col < M->cols; i_col++)
    {
      gsl_matrix_set(G, i_row, i_col, 
                     elements_ptr + (i_row - 1) * M->cols + i_col);
    }
  }
  
  return G;
}

static real *gsl_matrix_to_mat_elem_ptr(gsl_matrix *G)
{
  mat m;
  
  m->rows = G->size1;
  m->cols = G->size2;
  
  
  return m;
}

static gsl_matrix *gsl_matrix_get_inverse(gsl_matrix *M)
{
  int s;
  gsl_matrix *inverse = gsl_matrix_alloc(M->size1, M->size2);
  size_t n = ((M->size1 >= M->size2) ? M->size1 : M->size2);
  gsl_permutation *perm = gsl_permutation_alloc(n);
  
  /* perform LU decomposition of matrix M */
  gsl_linalg_LU_decomp(M, perm, &s);
  
  /* Invert the matrix */
  gsl_linalg_LU_invert(M, perm, inverse);
  
  /* free memory */
  gsl_permutation_free(perm);
  
  /* return inverse of matrix */
  return inverse;
}
