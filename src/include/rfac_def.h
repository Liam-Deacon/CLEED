/*********************************************************************
GH/10.08.95

 Include file for

  - additional data structures and type definitions
  - constant values

Changes:
GH/25.09.92
GH/03.03.93
GH/10.08.95 - Create (copy from rfdefines.h and rftypes.h)

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef RFAC_DEF_H
#define RFAC_DEF_H

/*********************************************************************
 structures and types 
*********************************************************************/

struct rfrswitch    /* indicates which R-factors shall be calculated */
                    /* If this structure is changed, the functions
                       rfrsw, rfcmpr, rfoutput have also to be changed */
{
 int ek;            /* Energy axis: I(E): 0; I(k): 1 (1.1) */
 int r_1;           /* R1-factor */
 int r_2;           /* R2-factor */
 int r_b;           /* Rb-factor */
 int r_g;           /* Rg-factor */
 int r_p;           /* Pendry's R-factor */
};

struct rfref        /* relation between expt. and theo. lists */
{
 int i_min;         /* index of smallest energy in theor. list */
 int i_max;         /* index of largest energy in theor. list */
 int *pindex;       /* list of indices in exper. list corresponding to
                       theoretical energies */
};
struct rfelist 
{
 float energy;      /* energy value in expt. IV curve */
 float intens;      /* intensity value in expt. IV curve */
};

struct rftlist 
{
 float energy;      /* energy value in theor. IV curve */
 float *pintens;    /* list of intensity values in theor. IV curve */
};

struct rfrfac
{
 int group_id;      /* either AVERAGE_ID or group flag */
 float r_1;         /* R1-factor */
 float r_2;         /* R2-factor */
 float r_b1;        /* Rb1-factor */
 float r_b2;        /* Rb2-factor */
 float r_g;         /* Rg-factor */
 float r_p;         /* Pendry's R-factor */
 float rr;          /* RR factor (confidence level) */
 float i_ratio;     /* ratio of theoretical/experimental curve integral */
 float e_range;     /* total energy range */
};

struct rfspot 
{
 float index1;      /* 1st index */
 float index2;      /* 2nd index */
 float f_val1;      /* can be used for length in k space e.g. */
 float f_val2;      /* can be used for intensity e.g. */
 int i_val1;        /* arbitrary use */
 int i_val2;        /* arbitrary use */
};

struct rfivcur 
{
 int     group_id;           /* assignment to a certain group of 
                                curves e.g. integral/superstructure */
 float   eng_0;              /* energy of beam appearance (1.1) */
 struct rfspot  spot_id;     /* one labeling set of spot indices */

 struct rftlist *the_list;   /* theo. IV curve */
 int     the_leng;           /* number of data pairs in IV list */
 int     the_ngeo;           /* number of different theoretical IV curves */
 int     the_equidist;       /* indicates equidistant energies */
 int     the_sort;           /* indicates sorted energies */
 int     the_smooth;         /* indicates that smoothing has been done */
 float   the_first_eng;      /* first energy in theo. list */
 float   the_last_eng;       /* last energy in theo. list */
 float   *pthe_max_int;      /* max. intensity value in list */

 struct rfelist *exp_list;   /* expt. IV curve */
 int     exp_leng;           /* number of data pairs in IV list */
 int     exp_equidist;       /* indicates equidistant energies */
 int     exp_sort;           /* indicates sorted energies */
 int     exp_smooth;         /* indicates that smoothing has been done */
 float   exp_first_eng;      /* first energy in expt. list */
 float   exp_last_eng;       /* last energy in expt. list */
 float   exp_max_int;        /* max. intensity value in list */

 float   overlap;            /* Energy range */
 struct rfrfac  rfac;        /* R-factors */
};

struct rfargs 
{
 char  *ctrfile;             /* input control file */
 char  *thefile;             /* input theory file */
 int   *p_geo;               /* list of trial geometries to test */
 float *p_shift;             /* shift of energy axes */
 struct rfrswitch r_switch;  /* calculate an r-factor or not */
 int   all_groups;           /* flag: print R-factors of all group ID's */
 char  *outfile;             /* file for output */
 float vi;                   /* imaginary part of optical potential */
 float ignore_max;           /* ignore features larger than ignore_max
                                in Rg */
};

struct rfpar
{
 float rf;
 int geo;
 int shift;
};

struct rfmin
{
 int group_id;
 struct rfpar r_1;
 struct rfpar r_2;
 struct rfpar r_b1;
 struct rfpar r_b2;
 struct rfpar r_g;
 struct rfpar r_p;
};

/*********************************************************************
 general definitions / constants
*********************************************************************/

#define M2_H      0.2631894506957162   /* 2*m/h       [eV^-1   A^-2] */
#define SQRT_M2_H 0.5130199320647456   /* sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 special definitions
*********************************************************************/

#define N_RFACTORS     7       /* Number of possible R-factors      */

#define ENG_TOLERANCE  0.1     /* accuracy in comparing energies    */
#define IND_TOLERANCE  0.02    /* accuracy in comparing indices     */
#define ZERO_TOLERANCE 1.e-10  /* intensities smaller than this value
                                  are considered to be zero         */

#define I_FAIL       -1        /* integer return value if failed    */
#define F_FAIL       -1.       /* float return value if failed      */

#define SM_LORENTZ    1        /* flag for Lorentzian smooth        */
#define ALL_CURVES   NULL      /* flag for rf_cmpr: use all IV curves
                                  for R-factor calculation          */
#define DEFAULT_GROUP_ID  1    /* default group ID                  */
#define AVERAGE_GROUP_ID -1    /* group ID for average over all 
                                  curves                            */

/*
 Version 1.1
*/

#define E_AXIS  0    /* use an I(E) curve for R factor calculations */
#define K_AXIS  1    /* use an I(k) curve for R factor calculations */

#define RG_IGNORE_MAX  400.    /* Ignore Features larger than 100eV (defines
				  the lower limit in Fourier space,
				  when calculating Rg-Factor */

/*********************************************************************
 End of include file
*********************************************************************/
#endif /* RFAC_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
