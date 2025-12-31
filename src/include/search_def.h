/*********************************************************************
GH/20.09.95

 Include file for SEARCH

  - additional data structures and type definitions
  - constant values

Changes:
GH/16.08.95 - Create (copy from srdefines.h )
GH/20.09.95 - Include parameters for R factor program here
GH/20.06.06 - Change number of iterations in amoeba to 2000.
LD/29.04.14 - Removed RFAC_PRG & LEED_PRG from defines as no longer used.

*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_DEF_H
#define SEARCH_DEF_H

/*********************************************************************
 structures and types 
*********************************************************************/

/*
  struct sr_atom_str contains all properties of a single atom
  (part of file leed_def.h, define only if this file is not included)
*/

struct sratom_str
{
 int type;         /* type reference, terminator */
 int  ref;         /* reference to symmetry equivalent atom */
 int nref;         /* number of equivalent atoms */
 char name[STRSZ]; /* atomic symbol (reference to phase shifts) */
 real r_min;       /* min. radius */

 real x;          /* offset for x position */
 real y;          /* offset for y position */
 real z;          /* offset for z position */
 real dr;         /* offset for thermal vibrations */

 real *x_par;     /* coefficients used to determine shifts in x */
 real *y_par;     /* coefficients used to determine shifts in y */
 real *z_par;     /* coefficients used to determine shifts in z */
 real *dr_par;    /* coefficients used to determine shifts in dr */
};

struct search_str
{
 int n_par;       /* number of independent parameters */

/* coordinate search: */
 int n_par_geo;   /* number of geometrical parameters in search */ 


 real x_min;      /* minimum rel. x shift */
 real y_min;      /* minimum rel. y shift */
 real z_min;      /* minimum z value */

 real x_max;      /* maximum rel. x shift */
 real y_max;      /* maximum rel. y shift */
 real z_max;      /* maximum z value */

 real b_lat[5];   /* basis vectors of the real 2-dim unit cell stored as
                     standard matrix (b1,b2): b1x = b_lat[1], b2x = b_lat[2]
                                              b1y = b_lat[3], b2y = b_lat[4] 
                  */
/* angle search */
 int sr_angle;    /* flag for the angle search */
 int i_par_theta; /* number of search parameter for theta */ 
 int i_par_phi;   /* number of search parameter for phi */ 
 real theta_0;    /* theta start value */
 real phi_0;      /* phi start value */


/* symmeties of search */
 int z_only;           /* xyz search or z search only */
 int rot_deg;          /* degree of rot. symmetry */
 real rot_axis[3];     /* rotational axis */
 real mir_point[3];    /* point in mirror plane */
 real mir_dir[3];      /* direction of mirror plane */

/* R factor */
 char rf_type[16];     /* R factor type */
 real rf_range;        /* shift range for R factor */
};

/*********************************************************************
 special definitions
*********************************************************************/

/*
  search 
*/

/*!
    \def SR_SIMPLEX
    Search code for the Downhill Simplex or amoeba (sx) method.
    
    \def SR_POWELL
    Search code for Powell's method (po).
    
    \def SR_SIM_ANNEALING
    Search code for the simulated annealing (sa) algorithm.
    
    \def SR_GENETIC
    Search code for the genetic algorithm (ga) method.

    \def SR_PSO
    Search code for the particle swarm optimisation (ps) method.

    \def SR_DIFFERENTIAL_EVOLUTION
    Search code for the differential evolution (de) method.
*/
#define SR_SIMPLEX        1     /* enumeration of search algorithm types */
#define SR_POWELL         2
#define SR_SIM_ANNEALING  3
#define SR_GENETIC        4
#define SR_PSO            5
#define SR_DIFFERENTIAL_EVOLUTION 6

/*!
    \def SR_SX
    Entry into downhill simplex/amoeba search.
    
    \def SR_SA
    Entry into simulated annealing search.
    
    \def SR_PO
    Entry into Powell's method search.
    
    \def SR_GA
    Entry into genetic algorithm search.

    \def SR_PS
    Entry into particle swarm optimisation search.

    \def SR_DE
    Entry into differential evolution search.
*/  
#if defined(USE_GSL) || defined(_USE_GSL)
    /* set search functions to GNU Scientific Library */
    #define SR_SX    sr_sx_gsl     
    #define SR_SA    sr_sa_gsl     
    #define SR_PO    sr_po_gsl
    #define SR_GA    sr_ga_gsl
    #define SR_PS    sr_pso
    #define SR_DE    sr_de
    #define SR_RDINP sr_rdinp
    #define I_PAR_0  0          /* start index for parameters */
# else
    /* use old search functions (not open source) */
    #define SR_SX    sr_sx
    #define SR_SA    sr_sa
    #define SR_PO    sr_po
    #define SR_GA    sr_ga    
    #define SR_PS    sr_pso
    #define SR_DE    sr_de
    #define SR_RDINP sr_rdinp
    #define I_PAR_0  1          /* start index for parameters */
#endif

/*!
    \def R_TOLERANCE
    Tolerance of R factors for termination.
    
    \def DPOS
    Initial displacement of parameters from input geometry 
    (used to set up the vertex for sr_amoeba)
    
    \def MAX_ITER_AMOEBA
    Maximum number of iterations in sr_amoeba().
     
    \def MAX_ITER_POWELL
    Maximum number of iterations in sr_powell().
    
    \def BRENT_TOLERANCE
    Tolerance criterion in function brent().
     
    \def FAC_THETA
    Factor for displacement in theta.
    
    \def FAC_PHI
    Factor for displacement in phi.
*/    
#define R_TOLERANCE     5.0e-4  /* tolerance of R factors for termination */

#define DPOS            0.10    /* initial displacement of parameters from 
                                   input geometry (used to set up the vertex
                                   for sr_amoeba) */
#define MAX_ITER_AMOEBA 2000    /* max. number of iterations in sr_amoeba */
#define MAX_ITER_SA     200     /* max. iterations per temperature in sr_sa */
#define MAX_ITER_PSO    200     /* max. number of iterations in sr_pso */
#define MAX_EVAL_PSO    2000    /* max. number of evaluations in sr_pso */
#define MAX_ITER_DE     200     /* max. number of iterations in sr_de */
#define MAX_EVAL_DE     2000    /* max. number of evaluations in sr_de */

#define MAX_ITER_POWELL 100     /* max. number of iterations in sr_powell */
#define BRENT_TOLERANCE 2.0e-2  /* tolerance criterion in function brent 
                                   (used in linmin) */

#define FAC_THETA       5.      /* factor for displacement in theta */
#define FAC_PHI         50.     /* factor for displacement in phi */

/* 
  R-factor parameters  (used in sr_evalrf)
*/
/*!
    \def RFAC_TYP
    R factor type used for minimisation.
    
    \def RFAC_SHIFT_STEP
    Step size in the search for minimum shift (eV).
    
    \def RFAC_SHIFT_RANGE
    Half of the search range for minimum shift (eV).
    
    \def SR_EVAL_DEF
    Indicates that the above parameters have been defined.
*/  
#define RFAC_TYP    "rp"        /* R factor type used for minimisation */
#define RFAC_SHIFT_STEP   0.25  /* step in the search for min. shift (eV) */
#define RFAC_SHIFT_RANGE  10.   /* half of the search range for min. 
                                   shift (eV) */
#define SR_EVAL_DEF             /* indicated that the above parameters
                                   have been defined */

/*
  current version 
*/

#define SR_VERSION "1.0 (test version GH,SRP/02.04.03)"

/*
  Preprocessor Macros
*/

/*!
    \def OPEN_ERROR(x)
    Convenience function for output
*/
    
#ifdef EXIT_ON_ERROR
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x); \
        exit(1)
#else
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x)
#endif

#define SR_NOT_IMPLEMENTED_ERROR(x)                             \
    fprintf(STDERR, "***error (SEARCH): "                       \
            "'%s' search is not yet implemented.\n", x);        \
    exit(-1);

/*********************************************************************
 End of include file
*********************************************************************/
#endif /* SEARCH_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
