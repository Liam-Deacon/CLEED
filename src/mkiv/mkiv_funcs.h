#ifndef MKIV_FUNCS_H
#define MKIV_FUNCS_H

#include "mkiv_def.h"
#include "file_functs.h"

float b_smooth(float *int_norm, int numb, int nstart, int nstep, int width);

int calca(float *kappa, float *en_old, float energy,
      Vector a[], float *range, float range_min, float rel_range, 
      Vector *scale, float scale_min, Vector rel_scale);
               
int calcoi(int nspot, Spot spot[], float range, ImageMatrix *image);

short calcbase(int naux, Spot aux[], Vector a[],
               float ratio, int distance, int trip_max, float cor);
               
int calcspotpos(Vector a[], int ndom, Domain superl[], 
                int spot_max, int *nspot, Spot spot[], 
                int nexcl, Lindex excl[], ImageMatrix *imask, 
                float router, float rinner, float *radius, 
                float energy, float ratio, float k_10);   
  
int conv_tif2mat(tifvalues *tifval, ImageMatrix *mat_image);

int conv_mat2tif(ImageMatrix *mat_image, tifvalues *tif_image);

int drawbound(ImageMatrix *image, ImageMatrix *imask, Coord *center,
              float router, float rinner, int write, char* fname);

int drawell(int nspot, Spot spot[], ImageMatrix *image, 
            Vector *scale, float angle, float verh);

int drawgrid(ImageMatrix *image, int write, char *fname);

char *filename(char *fname, int n);

int fimax4(int nspot, Spot spot[], int step, float range, ImageMatrix *image);

int getdomain (char *buffer, struct domain *superlat);

int getindices(FILE *fp, char *buffer, int *ndesi, struct lindex desi[], 
               int *nref, struct lindex ref[], int *nexcl, 
               struct lindex excl[]);

int get_int(int nspot, Spot spot[], ImageMatrix *image, ImageMatrix *imask, Vector *scale,
            float angle, float use_cur, int bg, float mins2n, int verb,
            float verh, float acci, float accb);
               
int ito3a(int n, char *s);

int mark_reflex(int nspot, Spot spot[], ImageMatrix *image, float thick, float radius, 
                int color, int ind, char *fname);

int mkmask(ImageMatrix *image, Coord *center, 
           float router, float rinner, int write, char *mask_path);

int out_tif(ImageMatrix *image, char *filename);

int plot_indices(ImageMatrix *image, int nspot, struct spot * spot);

void quicksort(struct spot *low_ptr, struct spot *up_ptr);

int readinp(int verb, char *inp_path, char *param_path, char *ref_name, 
            char *mask_name, int *nstart, int *nstop, float *e_step, 
            struct coord *cen, float *router, float *rinner, int *bg, 
            int *smooth, float *kpl_10, int *ndom, struct domain superlat[], 
            int *ndesi, struct lindex desi[], int *nref, struct lindex ref[], 
            int *nexcl, struct lindex excl[] );

int readtif (tifvalues *tifimage, char *v_input);

int readvar(int verb, char *var_path, char *param_path,
            float *cos_min, float *cos_max, float *verh, 
            float *acci, float *accb, int *distance, int *update, int *step, 
            int *trip_max, float *scale_min, float *range_min, float *s2n_good, 
            float *s2n_bad, int *ref_min, float *sec_range, float *s2n_ref, 
            float *ref_dev, float *bas_rat, float *range, struct vector *scale, 
            float *angle, float *ratio);

int ref_inp(int *naux, struct spot *aux, int verb);

char *reverse(char *s);

int setcontrol(int nspot, Spot spot[], int ndesi, Lindex desi[], 
               int nref, Lindex ref[]);

int sign(ImageMatrix *image, unsigned char chr, int xx, int yy, unsigned short value);
               
const char *timestamp();

int writetif (tifvalues *tifimage, char *v_output);
            
#endif /* MKIV_FUNCS_H */