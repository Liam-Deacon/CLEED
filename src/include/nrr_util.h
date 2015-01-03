#ifndef NRR_UTIL_H
#define NRR_UTIL_H

real *vector(int nl, int nh);
real **matrix(int nrl, int nrh, int ncl, int nch);
real **convert_matrix(real *a, int nrl, int nrh, int ncl, int nch);
int *ivector(int nl, int nh);
int **imatrix(int, int, int, int);
real **submatrix(real **a, int oldrl, int oldrh, int oldcl, int newrl, int newcl);
int free_vector(real *v, int nl);
int free_ivector(int *v, int nl);
int free_matrix(real **m, int nrl, int nrh, int ncl);
int free_imatrix(int **m, int nrl, int nrh, int ncl);
int free_submatrix(real **b, int nrl);
int free_convert_matrix(real **b, int nrl);
void nrerror(const char*);

#endif /* NRR_UTIL_H */
