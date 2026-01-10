/*********************************************************************
GH/12.09.95

 Include file for

  - additional data structures and type definitions
  - constant values

Changes:
GH/25.09.92
GH/03.03.93
GH/10.08.95 - Create (copy from rfdefines.h and rftypes.h)
GH/10.08.95 - modify structure crargs (output)

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CRFAC_DEF_H
#define CRFAC_DEF_H

/*********************************************************************
 structures and types 
*********************************************************************/

struct crelist 
{
 real energy;      /* energy value in expt. IV curve */
 real intens;      /* intensity value in expt. IV curve */
 real deriv2;      /* second derivative used in cubic spline */
};

struct rfspot 
{
 real index1;      /* 1st index */
 real index2;      /* 2nd index */
 real f_val1;      /* can be used for length in k space e.g. */
 real f_val2;      /* can be used for intensity e.g. */
 int i_val1;        /* arbitrary use */
 int i_val2;        /* arbitrary use */
};

struct crivcur 
{
/* general */
 int     group_id;           /* assignment to a certain group of 
                                curves e.g. integral/superstructure */
 struct rfspot  spot_id;     /* set of spot indices (label) */
 real   eng_0;              /* energy of beam appearance (1.1) */

/* theroretical data */
 struct crelist *the_list;   /* theo. IV curve */
 int     the_leng;           /* number of data pairs in IV list */
 int     the_equidist;       /* indicates equidistant energies */
 int     the_sort;           /* indicates sorted energies */
 int     the_smooth;         /* indicates that smoothing has been done */
 int     the_spline;         /* indicates that cubic spline has been prepared */
 real    the_first_eng;      /* first energy in theo. list */
 real    the_last_eng;       /* last energy in theo. list */
 real    the_max_int;        /* max. intensity value in list */

/* experimental data */
 struct crelist *exp_list;   /* expt. IV curve */
 int     exp_leng;           /* number of data pairs in IV list */
 int     exp_equidist;       /* indicates equidistant energies */
 int     exp_sort;           /* indicates sorted energies */
 int     exp_smooth;         /* indicates that smoothing has been done */
 int     exp_spline;         /* indicates that cubic spline has been prepared */
 real    exp_first_eng;      /* first energy in expt. list */
 real    exp_last_eng;       /* last energy in expt. list */
 real    exp_max_int;        /* max. intensity value in list */

/* R factor */
 real    overlap;            /* Energy overlap */
 real    weight;             /* relative weight in average */
 real    rfac;               /* R-factor */
};

struct crargs 
{
 char  *ctrfile;             /* input control file */
 char  *thefile;             /* input theory file */
 char  *outfile;             /* output file */
 char  *iv_file;             /* file name for IV curves */
 int   iv_out;               /* flag for output of IV curves */
 int   r_type;               /* R factor type */
 real  s_ini;                /* shift of energy axes */
 real  s_fin;                /* shift of energy axes */
 real  s_step;               /* shift of energy axes */
 real  vi;                   /* imaginary part of optical potential */
 int   all_groups;           /* flag: print R-factors of all group ID's */
};

/*********************************************************************
 general definitions / constants
*********************************************************************/

#define M2_H      0.2631894506957162   /* 2*m/h       [eV^-1   A^-2] */
#define SQRT_M2_H 0.5130199320647456   /* sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 special definitions
*********************************************************************/

#define N_RFACTORS     7       /* Number of possible R-factors */

#define ENG_TOLERANCE  0.1     /* accuracy in comparing energies */
#define IND_TOLERANCE  0.02    /* accuracy in comparing indices */
#define ZERO_TOLERANCE 1.e-10  /* intensities smaller than this value
				  are considered to be zero */

#define I_FAIL       -1        /* integer return value if failed */
#define F_FAIL       -1.       /* real return value if failed */

#define SM_LORENTZ    1        /* flag for Lorentzian smooth */
#define ALL_CURVES   NULL      /* flag for rf_cmpr: use all IV curves
				  for R-factor calculation */
#define DEFAULT_GROUP_ID  1    /* default group ID */
#define AVERAGE_GROUP_ID -1    /* group ID for average over all curves */

/*
 Version 1.1
*/

#define E_AXIS  0    /* use an I(E) curve for R factor calculations */
#define K_AXIS  1    /* use an I(k) curve for R factor calculations */

/* 
 CRFAC
*/

#define RP_FACTOR  1  /* Pendry's R factor */
#define R1_FACTOR  2  /* R1 factor */
#define R2_FACTOR  3  /* R2 factor */
#define RB_FACTOR  4  /* Rb factor */
#define RS_FACTOR  5  /* R_S factor */

/*********************************************************************
 End of include file
*********************************************************************/
#endif /* CRFAC_DEF_H */


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
