/*********************************************************************
 * functions used for the r-factor program
 *********************************************************************/

#ifndef RFAC_FUNC_H
#define RFAC_FUNC_H

#include "rfac_def.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*********************************************************************
 * General routines (utility functions)
 *********************************************************************/

int bgets(char *buffer, long offset, long max, char *string);
char *file2buffer(const char *filename);

rfac_args *rfac_rdargs(int argc, char **argv);

size_t rfac_lines(const char *buffer);
size_t rfac_clines(const char *buffer);
size_t rfac_nclines(const char *buffer);

const char *rfac_version(void);

void rfac_help(FILE *output);
void rfac_info(void);

const char *rfactor_name(rfactor_type type);
void rfactor_print(FILE *stream, rfactor_type type, real r_min, real rr,
                   real s_min, real e_range);

size_t rfac_get_number_of_angles(const char *control_filename);

void rfac_intindl(char *line, rfac_spot *spot, size_t n_beam);

/*********************************************************************
 * data output
 *********************************************************************/

int rfac_ctr2out(char *control_filename, FILE *output);
int rfac_output(rfac_args *args, rfac *rfac_vals, size_t n_cur, size_t n_g);

/*********************************************************************
 * data input
 *********************************************************************/

rfac_ivcur *rfac_ivcur_read(const char *control_file, const char *theory_file);
rfac_iv *rfac_iv_read_cleed(rfac_ivcur *iv_cur, char *buffer, char *indices);
rfac_iv *rfac_iv_read(const char *filename);

/*********************************************************************
 * sort, spline, smooth
 *********************************************************************/

int rfac_ivcur_process(rfac_ivcur *iv_cur, real vi);
real rfac_splint(real eng, rfac_iv_data *list, size_t leng);
void rfac_iv_spline(rfac_iv *iv);
int rfac_iv_sort(rfac_iv *iv);
int rfac_iv_lorentz_smooth(rfac_iv *iv, real vi);

/*********************************************************************
 * r_factors
 *********************************************************************/

size_t rfac_mklide(real *e_int, real *t_int, real *e_norm, real t_norm,
                   real shift, rfac_iv_data *elist, size_t n_eng,
                   rfac_iv_data *tlist, size_t n_the);
size_t rfac_mklist(real *e_int, real *t_int, real *e_norm, real t_norm,
                   rfac_iv_data *elist, size_t n_eng,
                   rfac_iv_data *tlist, size_t n_the);

real rfac_r1(const real *e_int, const real *t_int, const real *norm);
real rfac_r2(const real *e_int, const real *t_int, const real *norm);
real rfac_rb(const real *e_int, const real *t_int, const real *norm);
real rfac_rp(const real *e_int, const real *t_int, const real *norm, real vi);
real rfac_rmin(rfac_ivcur *iv_cur, rfac_args *args, real *r_min_ptr,
               real *rr_ptr, real *shift_ptr);

/*********************************************************************
 * low-level routines
 *********************************************************************/

rfac_args *rfac_args_init(void);
rfac_iv *rfac_iv_init(void);
rfac_iv *rfac_iv_alloc(size_t n_eng);
void rfac_iv_free(rfac_iv *iv);
void rfac_iv_copy(rfac_iv *dest, const rfac_iv *src);

void rfac_ivcur_free(rfac_ivcur *iv_cur);
void rfac_ivcur_free_all(rfac_ivcur *iv_cur);

int rfac_iv_to_arrays(const rfac_iv *iv, double *x, double *fx, size_t *n);
void rfac_ivcur_print(const rfac_ivcur *ivs);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_FUNC_H */
