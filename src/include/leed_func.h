/*********************************************************************
GH/29.09.00

functions for the leed programs (sym and temp)

Changes:
GH/30.08.97

version SYM 1.1 / TEMP 0.5
GH/27.09.00 - version SYM 1.1 / TEMP 0.5
GH/29.09.00 - remove unused functions.
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef LEED_FUNC_H
#define LEED_FUNC_H

/*********************************************************************
 Input
*********************************************************************/
   /* Decide which atoms belong to which layer; file linplayer.c */
int leed_leed_inp_bul_layer(leed_cryst_t *, leed_atom_t *, real *);
int leed_leed_inp_bul_layer_sym(leed_cryst_t *, leed_atom_t *, real *);
real leed_inp_debye_temp(real , real , real );
int leed_inp_overlayer(leed_cryst_t *, leed_atom_t *);
int leed_inp_overlayer_sym(leed_cryst_t *, leed_atom_t *);

   /* read a matrix with specified l1 m1 l2 m2 */
mat leed_inp_mat_lm(mat , int , const char *);

   /* read phase shifts; file linpphase.c */
int leed_inp_phase(char * , real * , leed_phs_t **);
int leed_leed_inp_phase_nd(char * , real * , int , leed_phs_t **);
int leed_update_phase(int);

   /* read bulk parameters; file linprdbul.c */
int leed_inp_read_bul(leed_cryst_t ** , leed_phs_t ** , char *);
int leed_inp_read_bul_nd(leed_cryst_t ** , leed_phs_t ** , char *);
int leed_inp_read_bul_sym(leed_cryst_t ** , leed_phs_t ** , char *);

   /* read overlayer parameters; file linprdovl.c */
int leed_read_overlayer(leed_cryst_t ** , leed_phs_t ** , leed_cryst_t * , char *);
int leed_read_overlayer_nd(leed_cryst_t **, leed_phs_t **, leed_cryst_t *, char *);
int leed_read_overlayer_sym(leed_cryst_t ** , leed_phs_t ** , leed_cryst_t * , char *);
   /* read other parameters; file linprdpar.c */
int leed_inp_leed_read_par(leed_var_t **, leed_energy_t **, leed_cryst_t * , char *);
   /* show all parameters; file linpshowbop.c */
int leed_inp_show_beam_op(leed_cryst_t *, leed_cryst_t *, leed_phs_t *);
   /* read and write parameters */
int leed_write_par(leed_cryst_t *, leed_phs_t *, leed_var_t *,
              leed_energy_t *, leed_beam_t *, FILE * );
int leed_read_par(leed_cryst_t **, leed_phs_t **, leed_var_t **,
             leed_energy_t **, leed_beam_t **, FILE * );

int leed_check_rotation_sym(leed_cryst_t *);
int leed_check_mirror_sym(leed_cryst_t *);

/*********************************************************************
 beams (bm) and parameter control (pc) and output (out)
*********************************************************************/
    /* Free rotation matrices (lbmrotmat.c) */
int leed_beam_rotation_matrix_free(real ** );
    /* Find the beams to be included (lbmgen.c) */
int leed_beam_gen(leed_beam_t **, leed_cryst_t *,
             leed_var_t *, real);
    /* Find the beams to be included (lbmgenrot.c) */
int leed_beam_gen_sym(leed_beam_t **, leed_cryst_t *, leed_cryst_t *,
             leed_var_t *, real);

    /* create rotation matrices */
real **leed_beam_get_rotation_matrices(int );
    /* Find the beams to be included at the current energy (lbmselect.c) */
int leed_beam_get_selection(leed_beam_t **, leed_beam_t *,
             leed_var_t *, real);
    /* Find the beams of a particular beam set (lbmset.c) */
int leed_beam_set(leed_beam_t **, leed_beam_t *, int);

/*********************************************************************
 Parameter control
*********************************************************************/
    /* reset parameter structure (lpcreset.c) */
int leed_par_reset(leed_var_t *, leed_cryst_t *);

    /* update energy (lpcupdate.c) and tl (lpcmktl.c) */
int leed_par_update(leed_var_t *, leed_phs_t *, real);
int leed_par_update_nd(leed_var_t *, leed_phs_t *, real);
mat *leed_par_mktl(mat *, leed_phs_t *, int, real);
mat *leed_par_mktl_nd(mat *, leed_phs_t *, int, real);

    /* temperature dependent scattering factors */
mat leed_par_temp_tl(mat , mat , real , real , int , int );
mat leed_par_cumulative_tl(mat , mat , real , real , real , real , int , int );
int pc_mk_ms(mat * , mat *, mat *, mat *, mat *, mat *, int );

/*********************************************************************
 Output
*********************************************************************/
int leed_out_head(FILE *);
int leed_out_head_2(const char *, const char *, FILE *);
int leed_output_beam_list(leed_beam_t **, leed_beam_t *, leed_energy_t *, FILE *);
int leed_output_int(mat , leed_beam_t *, leed_beam_t *, leed_var_t *, FILE * );
int leed_output_iint_sym(mat , leed_beam_t *, leed_beam_t *, leed_var_t *, FILE * );

    /* check cpu time */
double leed_cpu_time(FILE *, const char *);

/*********************************************************************
 Layer doubling (ld)
*********************************************************************/
   /* LD for 2 layers */
int leed_ld_2lay (mat *, mat *, mat *, mat *, 
             mat, mat, mat, mat, mat, mat, mat, mat, 
             leed_beam_t *, real *);
mat leed_ld_2lay_rpm (mat, mat, mat, mat, mat, mat,
             leed_beam_t *, real *);
   /* LD for periodic layers */
mat leed_ld_2n (mat, mat, mat, mat, mat, leed_beam_t *, real *);
   /* LD for potential step */
mat leed_ld_potstep ( mat , mat , leed_beam_t *, real , real *);
mat leed_ld_potstep0 ( mat , mat , leed_beam_t *, real , real *);

/*********************************************************************
 Multiple scattering (ms)
*********************************************************************/

   /* Don't know yet */
int leed_ms ( mat *, mat *,
               leed_var_t *, leed_layer_t *, leed_beam_t *);
int leed_ms_nd ( mat *, mat *, mat *, mat *,
               leed_var_t *, leed_layer_t *, leed_beam_t *);
int leed_ms_sym ( mat *, mat *,
               leed_var_t *, leed_layer_t *, leed_beam_t *);
int leed_ms_compl ( mat *, mat *, mat *, mat *, 
               leed_var_t *, leed_layer_t *, leed_beam_t *);
int leed_ms_compl_nd ( mat *, mat *, mat *, mat *, 
               leed_var_t *, leed_layer_t *, leed_beam_t *);
int leed_ms_compl_sym ( mat *, mat *, mat *, mat *,  
               leed_var_t *, leed_layer_t * ,leed_beam_t *);

   /* lattice sum for one layer (lmslsumii.c) */
mat leed_ms_lsum_ii (mat , real , real , real * , real * , int , real );

   /* lattice sum for two layers (lmslsumij(sym).c) */
int leed_ms_lsum_ij (mat *, mat *, real , real , real * , real * , real *, int , real );
mat leed_ms_lsum_ij_sym (mat, real , real , real * , real * , real *, int , real, int );

    /* partial inversion */
mat ms_partinv ( mat , mat , int , int );

   /* Green's function (lmstmatii/ij/ijsym.c, lmsgmatijsym.c) */
mat leed_ms_tmat_ii (mat , mat, mat, int );
mat leed_ms_tmat_nd_ii (mat , mat, mat, int );
mat leed_ms_tmat_ij (mat , mat, mat, int );
mat leed_ms_tmat_ij_sym (mat, mat, mat, int, int );

   /* Transformation L -> k (lmsymat.c/lmsymmat.c) */
mat leed_ms_ymat  (mat , int , leed_beam_t *, int );
mat leed_ms_ymat_set  (mat , int , leed_beam_t *, int );
mat leed_ms_ymmat (mat , int , leed_beam_t *, int );
mat leed_ms_ymat_r (mat , int , leed_beam_t *, int );

   /* Transformations of Ylm (lmsypy.c) */
mat leed_ms_yp_ym  (mat , mat);
mat leed_ms_yp_yxp (mat , mat);
mat leed_ms_yp_yxm (mat , mat);

   /* Summation over all beams for composite layer in case of symmetry */
mat leed_ms_comp_k_sum(mat ,leed_beam_t * ,leed_atom_t * ,int, int, int );

/*********************************************************************
lower level functions
*********************************************************************/

#endif /* LEED_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif