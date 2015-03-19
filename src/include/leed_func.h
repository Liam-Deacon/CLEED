/*********************************************************************
 *                           LEED_FUNC.H
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
 *   GH/2000.09.29 - creation
 *   GH/2000.09.27 - version SYM 1.1 / TEMP 0.5
 *   GH/2000.09.29 - remove unused functions.
 *   LD/2014.10.07 - updated function prototypes to reflect typedef
 *                   structure names.
 *                 - added C linkage with C++ compiler.
 *********************************************************************/

/*!
 * \file
 * \brief function prototype declarations for the \c cleed_nsym and \c
 * cleed_sym programs.
 */

#ifndef LEED_FUNC_H
#define LEED_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#if !__GNUC__
#define __attribute__(x) /* empty */
#endif

/*********************************************************************
 * Input
 *********************************************************************/
/* Decide which atoms belong to which layer; file linplayer.c */
__attribute__((nonnull(1,2,3)))
int leed_inp_bul_layer(leed_crystal *, leed_atom *, const real *);

__attribute__((nonnull(1,2,3)))
int leed_inp_bul_layer_sym(leed_crystal *, leed_atom *, const real *);

real leed_inp_debye_temp(real , real , real );
int leed_inp_overlayer(leed_crystal *, leed_atom *);
int leed_inp_overlayer_sym(leed_crystal *, leed_atom *);

/* read a matrix with specified l1 m1 l2 m2 */
mat leed_inp_mat_lm(mat, size_t, const char *);

/* read phase shifts; file linpphase.c */
int leed_inp_phase(const char *, real *, leed_phase **);
int leed_inp_phase_nd(const char *, real *, int, leed_phase **);
size_t leed_update_phase(size_t);

/* read bulk parameters; file linprdbul.c */
int leed_inp_read_bul(leed_crystal **, leed_phase **, char *);
int leed_inp_read_bul_nd(leed_crystal **, leed_phase **, const char *);
int leed_inp_read_bul_sym(leed_crystal **, leed_phase **, char *);

/* read overlayer parameters; file linprdovl.c */
int leed_read_overlayer(leed_crystal **, leed_phase **, leed_crystal *, char *);
int leed_read_overlayer_nd(leed_crystal **, leed_phase **,
                           leed_crystal *, const char *);
int leed_read_overlayer_sym(leed_crystal **, leed_phase **, leed_crystal *, char *);

/* read other parameters; file linprdpar.c */
int leed_inp_leed_read_par(leed_var **, leed_energy **,
                           leed_crystal *, const char *);

/* show all parameters; file linpshowbop.c */
void leed_inp_show_beam_op(leed_crystal *, leed_crystal *, leed_phase *);

/* read and write parameters */
int leed_write_par(leed_crystal *, leed_phase *, leed_var *,
              leed_energy *, leed_beam *, FILE *);
int leed_read_par(leed_crystal **, leed_phase **, leed_var **,
             leed_energy **, leed_beam **, FILE *);

int leed_check_rotation_sym(leed_crystal *);
int leed_check_mirror_sym(leed_crystal *);

int leed_energy_loop();

leed_args *leed_args_init();
leed_args *leed_args_parse(int argc, char *argv[]);
void leed_args_free(leed_args *args);

leed_model *leed_model_init(leed_args *args);
void leed_model_calculate_energies(leed_model *model);
void leed_model_free(leed_model *model);

void leed_beam_free(leed_beam *beams);
void leed_crystal_free(leed_crystal *crystal);
void leed_phase_free(leed_phase *phs_shifts);
void leed_var_free(leed_var *var);

/*********************************************************************
 * beams (bm) and parameter control (pc) and output (out)
 *********************************************************************/
/* Free rotation matrices (lbmrotmat.c) */
int leed_beam_rotation_matrix_free(real **);

/* Find the beams to be included (lbmgen.c) */
int leed_beam_gen(leed_beam **, leed_crystal *, leed_var *, real);

/* Find the beams to be included (lbmgenrot.c) */
int leed_beam_gen_sym(leed_beam **, leed_crystal *, leed_crystal *,
                      leed_var *, real);

/* create rotation matrices */
real **leed_beam_get_rotation_matrices(int );

/* Find the beams to be included at the current energy (lbmselect.c) */
int leed_beam_get_selection(leed_beam **, leed_beam *,
             leed_var *, real);

/* Find the beams of a particular beam set (lbmset.c) */
int leed_beam_set(leed_beam **, leed_beam *, size_t);

/*********************************************************************
 * Parameter control
 *********************************************************************/
/* reset parameter structure (lpcreset.c) */
int leed_par_reset(leed_var *, leed_crystal *);

/* update energy (lpcupdate.c) and tl (lpcmktl.c) */
int leed_par_update(leed_var *, leed_phase *, real);
int leed_par_update_nd(leed_var *, leed_phase *, real);
mat *leed_par_mktl(mat *, const leed_phase *, size_t, real);
mat *leed_par_mktl_nd(mat *, const leed_phase *, size_t, real);

/* temperature dependent scattering factors */
mat leed_par_temp_tl(mat, mat, real, real, size_t, size_t);
mat leed_par_cumulative_tl(mat, mat, real, real, real, real, size_t, size_t);
int pc_mk_ms(mat *, mat *, mat *, mat *, mat *, mat *, size_t);

/*********************************************************************
 * Output
 *********************************************************************/
void leed_output_header(FILE *);
int leed_out_head_2(const char *, const char *, FILE *);
int leed_output_beam_list(leed_beam **, const leed_beam *,
                          const leed_energy *, FILE *);
int leed_output_intensities(const mat, const leed_beam *, const leed_beam *,
                            const leed_var *, FILE *);
int leed_output_iint_sym(mat, leed_beam *, leed_beam *, leed_var *, FILE *);

/* check cpu time */
double leed_cpu_time(FILE *, const char *);

/*********************************************************************
 * Layer doubling (ld)
 *********************************************************************/
/* layer doubling for 2 layers */
void leed_ld_2lay(mat *, mat *, mat *, mat *,
             mat, mat, mat, mat, mat, mat, mat, mat, 
             leed_beam *, real *);
mat leed_ld_2lay_rpm(mat, mat, mat, mat, mat, mat, leed_beam *, real *);

/* layer doubling for periodic layers */
mat leed_ld_2n (mat, mat, mat, mat, mat, leed_beam *, real *);

/* layer doubling for potential step */
mat leed_ld_potstep(mat, mat, leed_beam *, real, real *);
mat leed_ld_potstep0(mat, mat, leed_beam *, real, real *);

/*********************************************************************
 * Multiple scattering (ms)
 *********************************************************************/
/* Don't know yet */
int leed_ms(mat *, mat *, leed_var *, leed_layer *, leed_beam *);
int leed_ms_nd(mat *, mat *, mat *, mat *,
               leed_var *, leed_layer *, leed_beam *);
int leed_ms_sym(mat *, mat *, leed_var *, leed_layer *, leed_beam *);
int leed_ms_compl(mat *, mat *, mat *, mat *,
                  leed_var *, leed_layer *, leed_beam *);
int leed_ms_compl_nd(mat *, mat *, mat *, mat *,
                     leed_var *, leed_layer *, leed_beam *);
int leed_ms_compl_sym(mat *, mat *, mat *, mat *,
                      leed_var *, leed_layer * ,leed_beam *);

/* lattice sum for one layer (lmslsumii.c) */
mat leed_ms_lsum_ii(mat, real, real, real *, real *, size_t, real);

/* lattice sum for two layers (lmslsumij(sym).c) */
int leed_ms_lsum_ij(mat *, mat *, real, real,
                    real *, real *, real *, size_t, real);
mat leed_ms_lsum_ij_sym(mat, real, real, real *, real *, real *, int, real, int);

/* partial inversion */
mat ms_partinv(mat, const mat, size_t, size_t);

/* Green's function (lmstmatii/ij/ijsym.c, lmsgmatijsym.c) */
mat leed_ms_tmat_ii(mat, mat, mat, size_t);
mat leed_ms_tmat_nd_ii(mat, const mat, mat, size_t);
mat leed_ms_tmat_ij(mat , mat, mat, size_t);
mat leed_ms_tmat_ij_sym(mat, mat, mat, int, int);

/* Transformation L -> k (lmsymat.c/lmsymmat.c) */
mat leed_ms_ymat(mat, size_t, const leed_beam *, size_t);
mat leed_ms_ymat_set(mat, size_t, leed_beam *, size_t);
mat leed_ms_ymmat(mat, int, leed_beam *, int);
mat leed_ms_ymat_r(mat, int, leed_beam *, int);

/* Transformations of Ylm (lmsypy.c) */
mat leed_ms_yp_ym(mat, mat);
mat leed_ms_yp_yxp(mat, mat);
mat leed_ms_yp_yxm(mat, mat);

/* Summation over all beams for composite layer in case of symmetry */
mat leed_ms_comp_k_sum(mat, leed_beam *, leed_atom *, int, int, int);

/*********************************************************************
 * lower level functions
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* LEED_FUNC_H */
