/*********************************************************************
 *                       SEARCH_DEF.H
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.16 - Create (copy from srdefines.h )
 *   GH/1995.09.20 - Include parameters for R factor program here
 *   GH/2006.06.20 - Change number of iterations in amoeba to 2000.
 *   LD/2014.04.29 - Removed RFAC_PRG & LEED_PRG from defines as no longer used.
 *   LD/2014.06.15 - Added conditional defines if compiling with '_USE_GSL'
 *   LD/2014.10.09 - Added typedefs to structs.
 *                 - Added some Doxygen markup.
 *   LD/2014.10.16 - Added enumerations: search_error & search_method
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 * This include file defines all the additional data structures, type
 * declarations and constant values needed for the search program.
 */

#ifndef SEARCH_DEF_H
#define SEARCH_DEF_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdbool.h>

/*********************************************************************
 * definitions
 *********************************************************************/

/*!
 * \def SR_SX
 * \brief Entry into downhill simplex/amoeba search.
 *
 * \def SR_SA
 * \brief Entry into simulated annealing search.
 *
 * \def SR_PO
 * \brief Entry into Powell's method search.
 *
 * \def SR_GA
 * \brief Entry into genetic algorithm search.
 *
 * \def SR_EVALRF
 * \brief Entry into R factor evaluation function.
 */
#if defined(USE_GSL) || defined(_USE_GSL)
    /* set search functions to GNU Scientific Library */
    #define SR_SX     sr_sx_gsl
    #define SR_SA     sr_sa_gsl
    #define SR_PO     sr_po_gsl
    #define SR_GA     sr_ga_gsl
    #define SR_RDINP  sr_rdinp
    #define SR_EVALRF sr_evalrf_gsl
    #define I_PAR_0   0         /*!< start index for parameters */
# else
    /* use old search functions (not open source) */
    #define SR_SX     sr_sx
    #define SR_SA     sr_sa
    #define SR_PO     sr_po
    #define SR_GA     sr_ga
    #define SR_RDINP  sr_rdinp
    #define SR_EVALRF sr_evalrf
    #define I_PAR_0   1         /*!< start index for parameters */
#endif

#define R_TOLERANCE     5.0e-4  /*!< tolerance of R factors for termination */

#define DPOS            0.10    /*!< initial displacement of parameters from
                                 * input geometry (used to set up the vertex
                                 * for sr_amoeba) */

#define MAX_ITER_AMOEBA 2000    /*!< max. number of iterations in sr_amoeba */

#define MAX_ITER_POWELL 100     /*!< max. number of iterations in sr_powell */
#define BRENT_TOLERANCE 2.0e-2  /*!< tolerance criterion in function brent()
                                 * (used in linmin() )
                                 */

#define FAC_THETA       5.      /*!< factor for displacement in \f$ \theta \f$ */
#define FAC_PHI         50.     /*!< factor for displacement in \f$ \phi \f$ */

/* R-factor parameters  (used in sr_evalrf() ) */
#define RFAC_TYP    "rp"        /*!< R factor type used for minimisation */
#define RFAC_SHIFT_STEP   0.25  /*!< step in the search for min. shift (in eV) */
#define RFAC_SHIFT_RANGE  10.   /*!< half of the search range for min.
                                   shift (in eV) */
#define SR_EVAL_DEF             /*!< indicated that the above parameters
                                   have been defined */

/*!
 * current version
 */
#define SR_VERSION "1.0 (test version GH,SRP/02.04.03)"

/*********************************************************************
 * Preprocessor Macros
 *********************************************************************/

/*!
 * \def OPEN_ERROR(x
 * \brief Convenience function for output.
 */
#ifdef EXIT_ON_ERROR
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x); \
        exit(SR_FILE_IO_ERROR)
#else
#define OPEN_ERROR(x)      fprintf(STDERR,                      \
        "*** error (SEARCH): could not open file \"%s\"\n", x)
#endif

#define SR_NOT_IMPLEMENTED_ERROR(x)                             \
    fprintf(STDERR, "***error (SEARCH): "                       \
            "'%s' search is not yet implemented.\n", x);        \
    exit(SR_SEARCH_NOT_IMPLEMENTED);

/*********************************************************************
 * enums, structures and typedefs
 *********************************************************************/

/*!
 * \typedef search_method
 * \brief enumeration for search methodology.
 */
typedef enum {
  SR_SIMPLEX=1,     /*!< Use downhill simplex or amoeba (sx) method */
  SR_POWELL,        /*!< Use Powell's method (po). */
  SR_SIM_ANNEALING, /*!< Use simulated annealing (sa) algorithm. */
  SR_GENETIC,       /*!< Use genetic algorithm (ga) method. */
  SR_UNKNOWN        /*!< Unknown search method. */

} search_method;

/*!
 * \typedef search_error
 * \brief Indicates error code for function success.
 */
typedef enum {
  SR_FAILURE=-1,              /*!< indicates general failure. */
  SR_SUCCESS,                 /*!< indicates general success. */
  SR_SEARCH_NOT_IMPLEMENTED,  /*!< indicates search method is not yet implemented.
                               * This is particularly relevant if compiling
                               * when defining \c _USE_GSL as not all the
                               * search methods have been ported to use the
                               * GNU Scientific Library API.
                               */

  SR_INVALID_SEARCH_TYPE,     /*!< Indicates search method is invalid. */
  SR_INVALID_INPUT_FILE,      /*!< Indicates the input file format is invalid
                               * or file not specified. */
  SR_INVALID_VERTEX_FILE,     /*!< Indicates the vertex backup file format is
                               * not correct or file not specified. */
  SR_FILE_IO_ERROR,           /*!< Indicates general file IO error. */
  SR_MISSING_ARGUMENT,        /*!< Command line argument is missing. */
  SR_SYSTEM_CALL_FAILED,      /*!< Indicates that system call returned non-zero */
  SR_ENVIRONMENT_VARIABLE_ERROR, /*!< Indicates invalid environment variable */
  SR_SYMMETRY_ERROR,          /*!< Indicates an issue with the specified
                               * symmetry for the given input */
  SR_ALLOC_ERROR,             /*!< Indicates that memory could not be allocated */
} search_error;


/*!
 * \struct sr_atom_str
 *
 * This structure contains all properties of a single atom.
 */

typedef struct sratom_str
{
  int type;         /*!< type reference, terminator */
  size_t ref;       /*!< reference to symmetry equivalent atom */
  int n_ref;        /*!< number of equivalent atoms */
  char name[STRSZ]; /*!< atomic symbol (reference to phase shifts) */
  real r_min;       /*!< min. radius */

  real x;           /*!< offset for x position */
  real y;           /*!< offset for y position */
  real z;           /*!< offset for z position */
  real dr;          /*!< offset for thermal vibrations */

  real *x_par;      /*!< coefficients used to determine shifts in x */
  real *y_par;      /*!< coefficients used to determine shifts in y */
  real *z_par;      /*!< coefficients used to determine shifts in z */
  real *dr_par;     /*!< coefficients used to determine shifts in dr */

} search_atom;

/*!
 * Structure for containing all the parameters of a given search.
 */
typedef struct search_str
{
  size_t n_par;       /*!< number of independent parameters */

  /* coordinate search: */
  size_t n_par_geo;   /*!< number of geometrical parameters in search */

  real x_min;         /*!< minimum rel. x shift */
  real y_min;         /*!< minimum rel. y shift */
  real z_min;         /*!< minimum z value */

  real x_max;         /*!< maximum rel. x shift */
  real y_max;         /*!< maximum rel. y shift */
  real z_max;         /*!< maximum z value */

  real b_lat[5];      /*!< basis vectors of the real 2-dim unit cell
                       * stored as standard matrix \f$(b1,b2)\f$:
                       * \f[ b1_x = b_lat[1], b2_x = b_lat[2] \f]
                       * \f[ b1_y = b_lat[3], b2_y = b_lat[4] \f]
                       */
  /* angle search */
  bool sr_angle;      /*!< flag for the angle search */
  size_t i_par_theta; /*!< number of search parameter for theta */
  size_t i_par_phi;   /*!< number of search parameter for phi */
  real theta_0;       /*!< theta start value */
  real phi_0;         /*!< phi start value */


  /* symmetries of search */
  bool z_only;        /*!< xyz search or z search only */
  size_t rot_deg;     /*!< degree of rot. symmetry */
  real rot_axis[3];   /*!< rotational axis */
  real mir_point[3];  /*!< point in mirror plane */
  real mir_dir[3];    /*!< direction of mirror plane */

  /* R factor */
  char rf_type[16];   /*!< R factor type */
  real rf_range;      /*!< shift range for R factor */

} search;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* SEARCH_DEF_H */
