/*********************************************************************
GH/11.08.95

functions used for the r-factor program
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CRFAC_FUNC_H
#define CRFAC_FUNC_H

/* low level input */
int    bgets( char *, long, long, char *); /* get string from buffer */
char * file2buffer( char *);               /* copy file to buffer */ 

/* count lines (all in file rflines.c) */
int   rf_lines( char *);                   /* count lines */
int   rf_clines( char *);                  /* count comment lines */
int   rf_nclines( char *);                 /* count data lines */


/* high level input/output */
struct crargs cr_rdargs (int, char * *);                       /* read argument list */
struct crivcur *cr_input( char *, char *);                     /* read control file */
struct crelist *cr_rdcleed( struct crivcur *, char *, char *); /* input of theor. data */
struct crelist *cr_rdexpt( struct crivcur *, char *);          /* input of expt. data */
void cr_intindl( char *, struct rfspot *, int);                /* line interpreter */

/* data output */
/*
int cr_output(struct crargs * ,  struct rfrfac * , int , int);
*/
                                           /* general output function */
int rf_ctr2out(char *, FILE * );           /* copy control file to output */


/* help etc. */
char * rf_version( char * );               /* current program version */
void rf_help(FILE *output);                /* help function */
void rf_info();                            /* version information, etc. */

/* 
 sort, spline, smooth 
*/
int cr_sort( struct crivcur *);           /* sort theoretical IV curve */
int cr_lorentz( struct crivcur * , real, char *);
                                           /* smooth IV curves */
void cr_spline(struct crelist *, int ); /* prepare cubic spline */
real cr_splint(real, struct crelist *, int ); /* prepare cubic spline */

/*
 r_factors
*/
int cr_mklide(real *, real *, real *, real , real,
              struct crelist *, int , struct crelist *, int );
int cr_mklist(real *, real *, real *, real , 
              struct crelist *, int , struct crelist *, int );
                                                /* prepare comparison */
real cr_r1( real *, real *, real *);            /* R1 factor */
real cr_r2( real *, real *, real *);            /* R2 factor */
real cr_rb( real *, real *, real *);            /* Rb1 factor */
real cr_rp( real *, real *, real *, real );     /* Pendry's R factor */
real cr_rs( real *, real *, real *, real );     /* R_S factor */

real cr_rmin( struct crivcur *, struct crargs *, real *, real *, real *);


#endif /* CRFAC_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
