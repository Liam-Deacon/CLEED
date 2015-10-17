/************************************************************************
 *                           RFAC_DEF.H
 *
 *  Copyright 2006-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.09.25 - initial work
 *   GH/1993.03.03 - refactor
 *   GH/1995.08.10 - creation (copy from rfdefines.h and rftypes.h)
 *   LD/2014.10.09 - added typedefs to rfac structs, rfac_axis enum
 *                   for E_AXIS and K_AXIS, size_t whereever appropriate
 *                   & initial Doxygen markup.
 *********************************************************************/

/*! \file
 *  \brief Header file for rfac data structures, definitions and constants.
 */

#ifndef RFAC_DEF_H
#define RFAC_DEF_H

#include <stdbool.h>
#include "cleed_real.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

/*********************************************************************
 * general definitions / constants
 *********************************************************************/

/*********************************************************************
 * special definitions
 *********************************************************************/

/*!
 *  \brief The accuracy in comparing energies using floating point.
 */
static const double ENG_TOLERANCE = 0.1;     /* accuracy in comparing energies */

/*!
 *  \brief The accuracy in comparing indices using floating point.
 */
static const double IND_TOLERANCE = 0.02;    /* accuracy in comparing indices */

/*!
 *  \brief Any intensities smaller than this value are considered to be zero
 */
static const double ZERO_TOLERANCE = 1.e-10;  /*!< intensities smaller than this
                                               * value are considered to be zero */

static const real F_FAIL = -1.;      /*!< float return value if failed */

enum { I_FAIL = -1 };                /*!< integer return value if failed */
enum { SM_LORENTZ = 1 };             /*!< flag for Lorentzian smooth */

#define ALL_CURVES   NULL      /*!< flag for rfac_cmpr: use all IV curves
                                * for R-factor calculation */
/*!
 * \brief Ignore Features larger than 100eV (defines the lower limit in Fourier
 * space when calculating Rg-Factor)
 */
static const real RG_IGNORE_MAX = 400.;

/*!
 *  \brief The maximum number of possible R factors
 */
enum {
  N_RFACTORS = 6       /* Number of possible R-factors */
};

typedef enum
{
  END_OF_GROUP_ID = -1,
  AVERAGE_GROUP_ID = 0,  /*!< default group ID */
  NO_GROUP_ID = 1,
  DEFAULT_GROUP_ID = 2   /*!< group ID for average over all curves */
} rfac_group_id;

/*!
 * Indicates the return/exit status of a function.
 */
typedef enum {
  RFAC_FAILURE=-1,              /*!< Indicates general failure */
  RFAC_SUCCESS,                 /*!< Indicates general success */
  RFAC_INVALID_ARGUMENT,        /*!< Indicates invalid command line argument */
  RFAC_IV_NOT_EQUIDISTANT,      /*!< Indicates IV is not equally spaced in energy */
  RFAC_ALLOCATION_ERROR,        /*!< Indicates failure to allocate memory */
  RFAC_ESTEP_TOO_SMALL,         /*!< Indicates energy step size is too small */
  RFAC_VI_TOO_SMALL,            /*!< Indicates the \f$ V_i \f$ value is too small */
  RFAC_INVALID_NUMBER_OF_ENERGIES,  /*!< Indicates an invalid number of energies */
  RFAC_INVALID_NUMBER_OF_BEAMS,     /*!< Indicates an invalid number of spots */
  RFAC_BAD_IV_DATA                  /*!< Indicates bad input of IV data */
} rfac_error;

/*! \typedef rfactor_type
 *  \brief indicates the R-factor type.
 */
typedef enum {
  RP_FACTOR=1,    /*!< Pendry's R-factor */
  R1_FACTOR,      /*!< R1 factor */
  R2_FACTOR,      /*!< R2 factor */
  RB_FACTOR       /*!< Rb factor */
} rfactor_type;

/*! \typedef rfac_axis
 *  \brief Enumeration representing IV axis 
 */
typedef enum {
  E_AXIS=0,     /*!< Use an I(E) curve for R factor calculations */
  K_AXIS        /*!< Use an I(k) curve for R factor calculations */
} rfac_axis;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* namespace cleed */

using cleed::rfac_group_id;
using cleed::rfac_error;
using cleed::rfactor_type;
using cleed::rfac_axis;

using cleed::rfac_iv_data;
using cleed::rfac_iv;
using cleed::rfac_ivcur;

#endif

/*********************************************************************
 * structures and types
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/*!
 * \brief Contains I(V) data pair at a single energy.
 */
typedef struct      /* Structure for a single I(V) data point entry */
{
  real energy;      /*!< Energy value for IV data point */
  real intens;      /*!< Intensity value for IV data point */
  real deriv2;      /*!< Second derivative used in cubic spline */
} rfac_iv_data;

/*!
 * Holds all the properties and data for an entire I(V) dataset. This
 * includes the energy range, flags for data operations and the actual I(V)
 * data as an array with @rfac_iv::n_eng number of elements.
 */
typedef struct            /* Holds all properties for an entire IV dataset */
{
  rfac_iv_data *data;     /*!< Pointer to the list of IV values */
  size_t n_eng;           /*!< Number of data pairs in IV list */
  bool equidist;          /*!< Indicates equidistant energies */
  bool sort;              /*!< Indicates sorted energies */
  bool smooth;            /*!< Indicates that smoothing has been done */
  bool spline;            /*!< Indicates that cubic spline has been prepared */
  real first_eng;         /*!< First energy in theo. list */
  real last_eng;          /*!< Last energy in theo. list */
  real max_int;           /*!< Max. intensity value in list */
} rfac_iv;

/*!
 * Contains R-factor values for evaluating the experimental/theoretical fit.
 */
typedef struct        /* Holds R-factor values for evaluating fits */
{
  int group_id;       /*!< either AVERAGE_ID or group flag */
  float r_1;          /*!< R1-factor */
  float r_2;          /*!< R2-factor */
  float r_b1;         /*!< Rb1-factor */
  float r_b2;         /*!< Rb2-factor */
  float r_g;          /*!< Rg-factor */
  float r_p;          /*!< Pendry's R-factor */
  float rr;           /*!< RR factor (confidence level) */
  float i_ratio;      /*!< Ratio of theoretical/experimental curve integral */
  float e_range;      /*!< Total energy range */
} rfac;

/*!
 * Contains contains diffraction spot data.
 */
typedef struct          /* Holds diffraction spot data. */
{
  real index1;          /*!< First index ( \f$ h \f$ Miller index) */
  real index2;          /*!< Second index ( \f$ h \f$ Miller index) */
  real f_val1;          /*!< Floating point variable which can be used for
                         * length in \f$ k \f$ -space */
  real f_val2;          /*!< Floating point variable which can be used for
                         * spot intensity */
  int i_val1;           /*!< Arbitrary use */
  int i_val2;           /*!< Arbitrary use */
} rfac_spot;

/*!
 * Structure to contain the theoretical and experimental I(V) curves and
 * any supporting properties needed for the R-factor calculations.
 */
typedef struct            /* Struct for comparing two I(V) curves */
{
  /* general */
  rfac_group_id group_id;/*!< Assignment to a certain group of
                           * curves e.g. integral/superstructure */
  real eng_0;             /*!< Energy of beam appearance (1.1) */
  rfac_spot spot_id;      /*!< One labeling set of spot indices */
  
  /* theoretical data */
  rfac_iv *theory;        /*!< Pointer to theoretical IV curve data */

  /* experimental data */
  rfac_iv *experimental;  /*!< Pointer experimental IV curve data*/

  /* R factor */
  real overlap;           /*!< Energy range */
  rfac rfac;              /*!< R-factors */
  real weight;            /*!< Relative weight in average R-factor between
                           * different IV curves */
} rfac_ivcur;

/*!
 * Stores all the command line arguments as properties ready for use in
 * the \c rfac program.
 */
typedef struct                /* Holds CLI arguments as program properties */
{
  char ctr_file[FILENAME_MAX];  /*!< Input control file string or path */
  char the_file[FILENAME_MAX];  /*!< Input theory file string or path */
  char out_file[FILENAME_MAX];  /*!< Output file string or path */
  char iv_file[FILENAME_MAX];   /*!< File name or path for IV curves */
  rfactor_type r_type;          /*!< R-factor type */
  real s_ini;                   /*!< Shift of initial energy */
  real s_fin;                   /*!< Shift of final energy */
  real s_step;                  /*!< Shift of energy step */
  real vi;                      /*!< Imaginary part of optical potential */
  bool all_groups;              /*!< Flag: print R-factors of all group ID's */

} rfac_args;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {
  typedef ::rfac rfac;
  typedef ::rfac_args rfac_args;
  typedef ::rfac_iv rfac_iv;
  typedef ::rfac_ivcur rfac_ivcur;
  typedef ::rfac_iv_data rfac_iv_data;
  typedef ::rfac_spot rfac_spot;
} /* namespace cleed */
#endif

#endif /* RFAC_DEF_H */
