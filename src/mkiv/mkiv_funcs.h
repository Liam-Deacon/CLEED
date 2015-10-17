/*********************************************************************
 *                         MKIV_FUNCS.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

#ifndef MKIV_FUNCS_H
#define MKIV_FUNCS_H

#ifdef __cplusplus__ /* add header compatibility with C++ */
extern "C" {
#endif

#include "mkiv_def.h"
#include "file_functs.h"

mkiv_args *mkiv_init();
mkiv_args *mkiv_get_args(int argc, char *argv[]);
void mkiv_args_free(mkiv_args *args);

/* Calculates background smoothing */
double mkiv_bkgnd_smooth(double *int_norm, int numb, int n_start, int n_step, int width);

int mkiv_calc_area(double *kappa, double *en_old, double energy,
      mkiv_vector a[], double *range, double range_min, double rel_range,
      mkiv_vector *scale, double scale_min, mkiv_vector rel_scale);

/* Recalculates the center of each spot using the center of gravity method */
int mkiv_calc_spot_disc(size_t nspot, mkiv_reflex spot[], double range, mkiv_image *image);

/* Determines the origin and basis vectors from at least 3 spots. */
size_t mkiv_calc_basis(size_t naux, mkiv_reflex aux[], mkiv_vector a[],
                       double ratio, int distance, size_t trip_max, double cor);

/* Calculates the positions of all reasonable reciprocal lattice points */
int mkiv_calc_spot_positions(mkiv_vector a[], size_t n_dom, mkiv_domain superl[],
                size_t spot_max, size_t *nspot, mkiv_reflex spot[],
                size_t n_excl, mkiv_index excl[], mkiv_image *imask,
                double r_outer, double r_inner, double *radius,
                double energy, double ratio, double k_10);
  
/* Converts TIFF to an matrix object. */
int mkiv_tif2mat(mkiv_tif_values *tifval, mkiv_image *mat_image);

/* Converts an image matrix to a TIFF object */
int mkiv_mat2tif(const mkiv_image *mat_image, mkiv_tif_values *tif_image);

/* Draws the boundaries of the LEED-screen. */
int mkiv_draw_bounds(mkiv_image *image, mkiv_image *imask, mkiv_position *center,
                   double r_outer, double r_inner, int write, char* fname);

/* Draws the elliptical integration area boundaries into image. */
int mkiv_draw_ellipse(size_t n_spot, mkiv_reflex spot[], mkiv_image *image,
                      mkiv_vector *scale, double angle, double verh);

/* Draws a grid overlay on top of the LEED image */
int mkiv_draw_grid(mkiv_image *image, int write, const char *fname);

char *filename(char *fname, int n);

void mkiv_find_imax(size_t n_spot, mkiv_reflex spot[], int step, double range,
                    const mkiv_image *image);

/* Reads superstructure domain information from a string */
int mkiv_read_domain(const char *buffer, mkiv_domain *superlat);

int mkiv_read_indices(FILE *fp, char *buffer, size_t *n_desi, mkiv_index *desi,
               size_t *n_ref, mkiv_index *ref, size_t *n_excl,
               mkiv_index *excl);

int mkiv_calc_intensities(size_t nspot, mkiv_reflex *spot, mkiv_image *image,
                          mkiv_image *imask, mkiv_vector *scale,
                          double angle, double use_cur, int bg, double mins2n,
                          int verb, double verh, double acci, double accb);

int mkiv_mark_reflex(size_t nspot, mkiv_reflex spot[], mkiv_image *image,
                double thick, double radius, int color, int ind, char *fname);

int mkiv_mk_mask(mkiv_image *image, const mkiv_position *center,
           double r_outer, double r_inner, int write, const char *mask_path);

int mkiv_output_tif(const mkiv_image *image, const char *filename);

int mkiv_plot_indices(mkiv_image *image, size_t nspot, const mkiv_reflex *spot);

void mkiv_quicksort(mkiv_reflex *low_ptr, mkiv_reflex *up_ptr);

void mkiv_input_free();
mkiv_input *mkiv_input_init();
mkiv_input *mkiv_input_read(mkiv_args *args);

void mkiv_params_free();
mkiv_params *mkiv_params_init();
mkiv_params *mkiv_params_read(const mkiv_args *args);

int mkiv_read_tif(mkiv_tif_values *tifimage, const char *input);

int mkiv_ref_inp(size_t *naux, mkiv_reflex *aux, int verb, const char *pos_file);

int mkiv_set_control(size_t nspot, mkiv_reflex *spot, size_t n_desi, mkiv_index *desi,
               size_t n_ref, mkiv_index *ref);

int mkiv_sign(mkiv_image *image, uint8_t chr, int xx, int yy, uint16_t value);

mkiv_reflex *mkiv_reflex_alloc(size_t *spot_max, const mkiv_vector a[], size_t n_dom,
                  const mkiv_domain *superlat, const mkiv_image *image);

/* writes TIFF to file */
int mkiv_tif_write(mkiv_tif_values *tifimage, const char *filename);

/******************************************************************
 * UTILITY FUNCTIONS
 ******************************************************************/

/* reverses a string */
char *reverse(char *s);

/* gets current timestamp */
const char *timestamp();

/******************************************************************
 * HELP FUNCTIONS
 ******************************************************************/

/* entry into mkiv program if running as a library */
int mkiv_main(int argc, char *argv[]);

/* print syntax for mkiv command line input */
void mkiv_usage(FILE *output);

/* print mkiv program versioning info */
void mkiv_info();

#ifdef __cplusplus__ /* add header compatibility with C++ */
}
#endif

#endif /* MKIV_FUNCS_H */
