/*********************************************************************
GH/21.08.95

functions used for the SEARCH program
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_FUNC_H
#define SEARCH_FUNC_H

#include <stdio.h>

#if !defined(USE_GSL) && !defined(_USE_GSL)
/********************************************************************
 Original Code Interface
 ********************************************************************/
/* sramoeba.c */
void sr_amoeba(real **, real *, int , real , real (*)(), int *);
real amotry   (real **, real *, real *, int , 
               real (*)(), int , int *, real );

/* sramebsa.c */
void sr_amebsa(real **, real *, int , real *, real *, real , 
               real (*)(), int *, real );
real amotsa   (real **, real *, real *, int , real *, real *,
               real (*)(), int , real *, real );
real fluct( long *);

/* srpowell.c */
void sr_powell(real *, real **, int , real , int *, real *, real (*)() );

/* Drivers */
void sr_sa(int , real, char *, char *);
void sr_sx(int , real, char *, char *);
void sr_po(int , char *, char *);
void sr_er(int , real, char *, char *);

/* file input|output */
real sr_ckgeo(real *);
int  sr_ckrot(struct sratom_str * , struct search_str * );
real sr_evalrf(real *);
int  sr_mkinp(real *, int, char *);
int  sr_rdinp(const char * );
int  sr_rdver(char * , real *, real **, int );

#else
/********************************************************************
 GNU Scientific Interface
 ********************************************************************/
/* sramoeba_gsl.c - downhill simplex */
int sr_amoeba_gsl(gsl_matrix *, gsl_vector *, real , size_t *);
double sr_amoeba_eval(const gsl_vector *, void *);

/* sramebsa_gsl.c - simulated annealing */
void sr_amebsa_gsl(gsl_matrix *, gsl_vector *, gsl_vector *, real *, 
                   real, real (*)(), size_t *, real);
real amotsa(gsl_matrix *, gsl_vector *, gsl_vector *, gsl_vector *, real *, 
            real *, real (*)(), int , real *, real);
real fluct( long *);

/* srpowell_gsl.c - Powell's method */
int sr_powell_gsl(gsl_vector *, gsl_matrix *, real, size_t *, real *);
void linmin_gsl(gsl_vector *, gsl_vector *, real *, real (*)());

/* Drivers */
void sr_sa_gsl(size_t , real, char *, char *);     /* Simulated Annealing  */
void sr_sx_gsl(size_t , real, char *, char *);     /* Simplex method       */
void sr_po_gsl(size_t , char *, char *);           /* Powell's method      */
void sr_er_gsl(size_t , real, char *);             /* error evaluation     */

/* file input|output */
real sr_ckgeo_gsl(const gsl_vector *);
int  sr_ckrot(struct sratom_str * , struct search_str * );
double sr_evalrf_gsl(const gsl_vector *);
int  sr_mkinp_gsl(const gsl_vector *, int, const char *);
int  sr_rdinp(const char * );
int  sr_rdver_gsl(const char *, gsl_vector *, gsl_matrix *);

#endif 

/* debye temperature */
real leed_inp_debye_temp(real , real , real );

/* help */
void search_usage(FILE * );
void search_info();

#endif /* SEARCH_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif