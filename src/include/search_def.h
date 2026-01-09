/**
 * @file search_def.h
 * @brief Type definitions and constants for SEARCH.
 *
 * This file provides:
 * - Data structures for atom coordinates and search configuration
 * - Algorithm selection constants
 * - Convergence tolerances and iteration limits
 * - R-factor evaluation parameters
 *
 * @ingroup search_core
 *
 * Changes:
 * GH/16.08.95 - Create (copy from srdefines.h )
 * GH/20.09.95 - Include parameters for R factor program here
 * GH/20.06.06 - Change number of iterations in amoeba to 2000.
 * LD/29.04.14 - Removed RFAC_PRG & LEED_PRG from defines as no longer used.
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_DEF_H
#define SEARCH_DEF_H

/**
 * @defgroup search_config Search Configuration
 * @ingroup search_core
 * @brief Data structures for search parameters and atom positions.
 * @{
 */

#include "gh_stddef.h"
#include "real.h"

/*********************************************************************
 structures and types 
*********************************************************************/

/**
 * @brief Properties of a single atom in the search parameter space.
 *
 * Each sratom_str instance represents an atom whose position may be varied
 * during optimization. The position offsets (x, y, z, dr) are computed from
 * basis coefficients and the current search parameter vector.
 *
 * @note Part of file leed_def.h; only defined if that file is not included.
 */
struct sratom_str
{
 int type;         /**< Type reference, terminator (0 = end of list) */
 int  ref;         /**< Reference to symmetry equivalent atom */
 int nref;         /**< Number of equivalent atoms */
 char name[STRSZ]; /**< Atomic symbol (reference to phase shifts) */
 real r_min;       /**< Minimum radius (in Angstroms) */

 real x;          /**< Offset for x position */
 real y;          /**< Offset for y position */
 real z;          /**< Offset for z position */
 real dr;         /**< Offset for thermal vibrations (Debye-Waller) */

 real *x_par;     /**< Coefficients used to determine shifts in x */
 real *y_par;     /**< Coefficients used to determine shifts in y */
 real *z_par;     /**< Coefficients used to determine shifts in z */
 real *dr_par;    /**< Coefficients used to determine shifts in dr */
};

typedef struct sratom_str search_atom;

/**
 * @brief Global search configuration structure.
 *
 * Contains the parameter bounds, symmetry constraints, lattice vectors,
 * and R-factor settings for a SEARCH optimization run.
 */
struct search_str
{
 int n_par;       /**< Number of independent parameters */

/* coordinate search: */
 int n_par_geo;   /**< Number of geometrical parameters in search */ 


 real x_min;      /**< Minimum relative x shift */
 real y_min;      /**< Minimum relative y shift */
 real z_min;      /**< Minimum z value */

 real x_max;      /**< Maximum relative x shift */
 real y_max;      /**< Maximum relative y shift */
 real z_max;      /**< Maximum z value */

 real b_lat[5];   /**< Basis vectors of the real 2D unit cell stored as
                       standard matrix (b1,b2): b1x = b_lat[1], b2x = b_lat[2]
                                                b1y = b_lat[3], b2y = b_lat[4] */
/* angle search */
 int sr_angle;    /**< Flag for the angle search (0 = disabled) */
 int i_par_theta; /**< Index of search parameter for theta */ 
 int i_par_phi;   /**< Index of search parameter for phi */ 
 real theta_0;    /**< Theta start value (radians) */
 real phi_0;      /**< Phi start value (radians) */


/* symmeties of search */
 int z_only;           /**< xyz search (0) or z search only (1) */
 int rot_deg;          /**< Degree of rotational symmetry */
 real rot_axis[3];     /**< Rotational axis vector */
 real mir_point[3];    /**< Point in mirror plane */
 real mir_dir[3];      /**< Direction of mirror plane normal */

/* R factor */
 char rf_type[16];     /**< R factor type string (e.g., "rp" for Pendry) */
 real rf_range;        /**< Shift range for R factor (eV) */
};

typedef struct search_str search;
typedef int search_method;

/** @} */ /* end of search_config group */

/**
 * @defgroup search_algos Search Algorithms
 * @ingroup search_core
 * @brief Optimization algorithm selection and parameters.
 * @{
 */

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

/** @} */ /* end of search_algos group */

/*********************************************************************
 End of include file
*********************************************************************/
#endif /* SEARCH_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
