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
 *   LD/2014.06.15 - Added conditional defines if compiling with 'USE_GSL'
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
namespace cleed {
#endif

#include <stdbool.h>
#include <stddef.h>
#include "cleed_real.h"
#include "cleed_vector.h"
#include "cleed_matrix.h"
#include "gh_stddef.h"

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
#if USE_GSL /* set search functions to GNU Scientific Library */
# define SR_SX_FACTORY     sr_sx_gsl
# define SR_SA_FACTORY     sr_sa_gsl
# define SR_PO_FACTORY     sr_po_gsl
# define SR_GA_FACTORY     sr_ga_gsl
# define SR_RDINP_FACTORY  sr_rdinp
# define SR_EVALRF_FACTORY sr_evalrfac_gsl
# define I_PAR_0   0         /*!< start index for parameters */
#elif (USE_CBLAS || USE_MKL || USE_LAPACK)

#else /* use old search functions (not open source) */
# define SR_SX_FACTORY     sr_sx
# define SR_SA_FACTORY     sr_sa
# define SR_PO_FACTORY     sr_po
# define SR_GA_FACTORY     sr_ga
# define SR_RDINP_FACTORY  sr_rdinp
# define SR_EVALRF_FACTORY sr_evalrf
# define I_PAR_0   1         /*!< start index for parameters */
#endif

static const real R_TOLERANCE = 5.0e-4;  /*!< Tolerance of R-Factors for termination */
static const real BRENT_TOLERANCE = 2.0e-2; /*!< Tolerance criterion in brent()
                                             * (used in linmin() ) */

static const real DPOS = 0.10; /*!< initial displacement of parameters from
                                * input geometry (used to set up the vertex
                                * for sr_amoeba) */

enum { MAX_ITER_AMOEBA = 2000 };  /*!< Maximum number of iterations in sr_amoeba() */
enum { MAX_ITER_POWELL = 100 };  /*!< Maximum number of iterations in sr_powell() */

static const real FAC_THETA = 5.; /*!< Factor for displacement in \f$ \theta \f$ */
static const real FAC_PHI = 50.;  /*!< Factor for displacement in \f$ \phi \f$ */

/* R-factor parameters  (used in sr_evalrf() ) */
static const char RFAC_TYP[] = "rp"; /*!< R factor type used for minimisation */
static const real RFAC_SHIFT_STEP = 0.25;  /*!< step in the search for minimum
                                            *   shift (in eV) */
static const real RFAC_SHIFT_RANGE = 10.;  /*!< half of the search range for
                                            *   minimum shift (in eV) */

/*********************************************************************
 * Preprocessor Macros
 *********************************************************************/

/*********************************************************************
 * enums, structures and typedefs
 *********************************************************************/

/*!
 * \enum search_method
 * \brief enumeration for search methodology.
 */
typedef enum {
  SR_SIMPLEX=1,     /*!< Use down hill simplex or amoeba (sx) method */
  SR_POWELL,        /*!< Use Powell's method (po). */
  SR_SIM_ANNEALING, /*!< Use simulated annealing (sa) algorithm. */
  SR_GENETIC,       /*!< Use genetic algorithm (ga) method. */
  SR_UNKNOWN        /*!< Unknown search method. */

} search_method;

/*!
 * \enum search_error
 * \brief Indicates error code for function success.
 */
typedef enum {
  SR_FAILURE=-1,              /*!< indicates general failure. */
  SR_SUCCESS,                 /*!< indicates general success. */
  SR_SEARCH_NOT_IMPLEMENTED,  /*!< indicates search method is not yet implemented.
                               * This is particularly relevant if compiling
                               * when defining \c USE_GSL as not all the
                               * search methods have been ported to use the
                               * GNU Scientific Library API.
                               */
  SR_MAXIMUM_ITERATIONS,      /*!< Indicates maximum number of iterations reached */
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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*!
 * \struct search_atom
 *
 * This structure contains all properties of a single atom needed when
 * performing a geometry search.
 */

typedef struct search_atom
{
  int type;         /*!< Type reference, terminator */
  size_t ref;       /*!< Reference to symmetry equivalent atom */
  size_t n_ref;     /*!< Number of equivalent atoms */
  char name[STRSZ]; /*!< Atomic symbol (reference to phase shifts) */
  real r_min;       /*!< Minimum radius e.g. the muffin tin radius MTR */

  real x;           /*!< Offset for x position */
  real y;           /*!< Offset for y position */
  real z;           /*!< Offset for z position */
  real dr;          /*!< Offset for thermal vibrations */

  real *x_par;      /*!< Coefficients used to determine shifts in x */
  real *y_par;      /*!< Coefficients used to determine shifts in y */
  real *z_par;      /*!< Coefficients used to determine shifts in z */
  real *dr_par;     /*!< Coefficients used to determine shifts in dr */

} search_atom;

/*!
 * \struct search
 *
 * Structure for containing all the parameters of a given search.
 */
typedef struct search
{
  size_t n_par;       /*!< Number of independent parameters */

  /* coordinate search: */
  size_t n_par_geo;   /*!< Number of geometrical parameters in search */

  real x_min;         /*!< Minimum relative x shift */
  real y_min;         /*!< Minimum relative y shift */
  real z_min;         /*!< Minimum z value */

  real x_max;         /*!< Maximum relative x shift */
  real y_max;         /*!< Maximum relative y shift */
  real z_max;         /*!< Maximum z value */

  real b_lat[5];      /*!< Basis vectors of the real 2-dim unit cell
                       * stored as standard matrix \f$(b1,b2)\f$:
                       * \f[ b1_x = b_lat[1], b2_x = b_lat[2] \f]
                       * \f[ b1_y = b_lat[3], b2_y = b_lat[4] \f]
                       */
  /* angle search */
  bool sr_angle;      /*!< Flag for the angle search */
  size_t i_par_theta; /*!< Number of search parameters for \f$ \theta \f$
                       * when using multiple data sets */
  size_t i_par_phi;   /*!< Number of search parameters for \f$ \phi \f$ when
                       * using multiple data sets */
  real theta_0;       /*!< Start value for \f$ \theta \f$ when optimising */
  real phi_0;         /*!< Start value for \f$ \phi \f$ when optimising */


  /* symmetries of search */
  bool z_only;        /*!< \f$ (x,y,z) \f$ search or \f$ z \f$ only search */
  size_t rot_deg;     /*!< Degree of rotational symmetry */
  real rot_axis[3];   /*!< Rotational axis */
  real mir_point[3];  /*!< Point in mirror plane */
  real mir_dir[3];    /*!< Direction of mirror plane */

  /* R factor */
  char rfac_type[16];   /*!< R factor type */
  real rfac_range;      /*!< Shift range for R factor */

} search;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */
} /* namespace cleed */
#endif

#endif /* SEARCH_DEF_H */
