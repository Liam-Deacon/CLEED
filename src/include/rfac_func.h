/*********************************************************************
GH/11.08.95

functions used for the r-factor program
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef RFAC_FUNC_H
#define RFAC_FUNC_H

int    bgets( char *, long, long, char *); /* get string from buffer */
char * file2buffer( char *);               /* copy file to buffer */ 
struct rfargs rf_rdargs (int, char * *);   /* read argument list */

/* 
 count lines (all in file rflines.c)
*/
int   rf_lines( char *);                   /* count lines */
int   rf_clines( char *);                  /* count comment lines */
int   rf_nclines( char *);                 /* count data lines */

/*
 read data from files
*/
char * rf_version( char * );               /* current program version */
void rf_help(FILE *);                            /* help function */

struct rfivcur *rf_input( char * );        /* general input routine */
struct rfelist *rf_rdexpt( struct rfivcur *, char *);       
					   /* input of expt. data */
struct rftlist *rf_rdthe( struct rfivcur *, char *);       
					   /* input of the. data */
struct rftlist *rf_rdvh_beams( struct rfivcur *, char *, char *);  
					   /* input of the. data */
int rf_intindl( char *, struct rfspot *, int);   
					   /* line interpreter */
/*
 data output
*/

int rf_output(struct rfargs * ,  struct rfrfac * , int , int);
					   /* general output function */
int rf_ctr2out(char *, FILE * );           /* copy control file to output */

/*
 r_factors
*/
/*
struct rfrfac *rf_cmpr( struct rfivcur *, struct rfargs *, int *, int, float);
*/

int rf_cmpr( struct rfrfac **, struct rfivcur *, 
                        struct rfargs *, int *, int, float);
					   /* compute all R-factors */
double rf_r1( struct rfivcur *, struct rfref *, int);
					   /* R1 factor */
double rf_r2( struct rfivcur *, struct rfref *, int);
					   /* R2 factor */
double rf_rb( struct rfivcur *, struct rfref *, int);
					   /* Rb1/2 factors */
double rf_rg( struct rfivcur *, struct rfref *, float, int);
					   /* Rg factor */
double rf_rp( struct rfivcur *, struct rfref *, float, int);
					   /* Pendry's R factor */

double rf_rk1( struct rfivcur *, struct rfref *, int);
					   /* R1 factor I(k) */
double rf_rk2( struct rfivcur *, struct rfref *, int);
					   /* R2 factor  I(k) */
double rf_rkb( struct rfivcur *, struct rfref *, int);
					   /* Rb1/2 factors  I(k) */
double rf_rkg( struct rfivcur *, struct rfref *, int);
					   /* Rg factor  I(k) */
double rf_rkp( struct rfivcur *, struct rfref *, float, int);
					   /* Pendry's R factor  I(k) */
void rf_rres( struct rfrfac * );           /* reset rfrfac */
int rf_rsw( char *, struct rfrswitch * );  /* set rf switches */
void rf_rswall ( int, struct rfrswitch * );/* set all rf switches (in
					      file "rfrsw.c" */
struct rfmin *rf_fdmin(struct rfrfac *, struct rfmin *, int, int);
					   /* find min. R factor */
int rf_prmin(struct rfmin *, struct rfargs *);

/* 
 smoothing etc.
*/
struct rfref rf_reflist( struct rfivcur *, float, float);
					   /* set up reference */
int rf_tsort( struct rfivcur *);           /* sort theoretical IV curve */
int rf_lorentz( struct rfivcur * , float, char *);
                                           /* smooth IV curves */

/************************/

/* new */
int cr_cmpr( struct rfrfac **, struct rfivcur *, 
                        struct rfargs *, int *, int, float);
struct rfivcur *cr_input( char *, char *);
struct rftlist *cr_rdcleed( struct rfivcur *, char *, char *);
int cr_output(struct rfargs * ,  struct rfrfac * , int , int);
					   /* general output function */

#endif /* RFAC_FUNC_H */
                       
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif