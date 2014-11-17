/*********************************************************************
 *                         MKIV_FUNCS.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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

/* Calculates background smoothing */
float bkgnd_smooth(float *int_norm, int numb, int nstart, int nstep, int width);

int calca(float *kappa, float *en_old, float energy,
      mkiv_vector a[], float *range, float range_min, float rel_range, 
      mkiv_vector *scale, float scale_min, mkiv_vector rel_scale);

/* Recalculates the center of each spot using the center of gravity method */
int calcoi(size_t nspot, mkiv_reflex spot[], float range, mkiv_image *image);

/* Determines the origin and basis vectors from at least 3 spots. */
short calcbase(size_t naux, mkiv_reflex aux[], mkiv_vector a[],
               float ratio, int distance, int trip_max, float cor);

/* Calculates the positions of all reasonable reciprocal lattice points */
int calcspotpos(mkiv_vector a[], size_t ndom, mkiv_domain superl[],
                size_t spot_max, size_t *nspot, mkiv_reflex spot[],
                size_t nexcl, mkiv_index excl[], mkiv_image *imask,
                float router, float rinner, float *radius, 
                float energy, float ratio, float k_10);   
  
/* Converts TIFF to an matrix object. */
int conv_tif2mat(mkiv_tif_values *tifval, mkiv_image *mat_image);

/* Converts an image matrix to a TIFF object */
int conv_mat2tif(const mkiv_image *mat_image, mkiv_tif_values *tif_image);

/* Draws the boundaries of the LEED-screen. */
int drawbound(mkiv_image *image, mkiv_image *imask, mkiv_position *center,
              float router, float rinner, int write, char* fname);

/* Draws the elliptical integration area boundaries into image. */
int drawell(int nspot, mkiv_reflex spot[], mkiv_image *image,
            mkiv_vector *scale, float angle, float verh);

/* Draws a grid overlay on top of the LEED image */
int drawgrid(mkiv_image *image, int write, const char *fname);

char *filename(char *fname, int n);

void fimax4(size_t nspot, mkiv_reflex spot[], int step, float range,
            const mkiv_image *image);

/* Reads superstructure domain information from a string */
int getdomain(const char *buffer, mkiv_domain *superlat);

int getindices(FILE *fp, char *buffer, size_t *ndesi, mkiv_index *desi,
               size_t *nref, mkiv_index *ref, size_t *nexcl,
               mkiv_index *excl);

int get_int(size_t nspot, mkiv_reflex spot[], mkiv_image *image,
            mkiv_image *imask, mkiv_vector *scale,
            float angle, float use_cur, int bg, float mins2n, int verb,
            float verh, float acci, float accb);
               
int ito3a(int n, char *s);

int mark_reflex(size_t nspot, mkiv_reflex spot[], mkiv_image *image,
                float thick, float radius, int color, int ind, char *fname);

int mkmask(mkiv_image *image, const mkiv_position *center,
           float router, float rinner, int write, const char *mask_path);

int out_tif(const mkiv_image *image, const char *filename);

int plot_indices(mkiv_image *image, size_t nspot, const mkiv_reflex *spot);

void quicksort(mkiv_reflex *low_ptr, mkiv_reflex *up_ptr);

int readinp(int verb, char *inp_path, char *param_path, char *ref_name, 
            char *mask_name, size_t *nstart, size_t *nstop, float *e_step,
            mkiv_position *cen, float *router, float *rinner, int *bg,
            int *smooth, float *kpl_10, size_t *ndom, mkiv_domain *superlat,
            size_t *ndesi, mkiv_index *desi, size_t *nref, mkiv_index *ref,
            size_t *nexcl, mkiv_index *excl);

int readtif(mkiv_tif_values *tifimage, const char *input);

int readvar(int verb, char *var_path, char *param_path,
            float *cos_min, float *cos_max, float *verh, 
            float *acci, float *accb, int *distance, int *update, int *step, 
            int *trip_max, float *scale_min, float *range_min, float *s2n_good, 
            float *s2n_bad, int *ref_min, float *sec_range, float *s2n_ref, 
            float *ref_dev, float *bas_rat, float *range, mkiv_vector *scale,
            float *angle, float *ratio);

int ref_inp(size_t *naux, mkiv_reflex *aux, int verb, const char *pos_file);

int setcontrol(size_t nspot, mkiv_reflex *spot, size_t ndesi, mkiv_index *desi,
               size_t nref, mkiv_index *ref);

int sign(mkiv_image *image, unsigned char chr,
         size_t xx, size_t yy, unsigned short value);

/******************************************************************
 * UTILITY FUNCTIONS
 ******************************************************************/

/* reverses a string */
char *reverse(char *s);

/* gets current timestamp */
const char *timestamp();

/* writes TIFF to file */
int writetif(mkiv_tif_values *tifimage, const char *filename);


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
