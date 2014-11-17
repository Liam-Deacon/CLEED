/************************************************************************
 *                           RFAC_DEF.H
 *
 *  Copyright 2006-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <stdbool.h>
#include <real.h>

/*********************************************************************
 * general definitions / constants
 *********************************************************************/

 /*! \define M2_H
 *  \brief convenience constant of \f$ \frac{2 m_e}{h} = 0.2631894506957162
 *  [eV^{-1} A^{-2}] \f$
 */
#define M2_H      0.2631894506957162   /* 2*m/h       [eV^-1   A^-2] */

/*! \define SQRT_M2_H
 *  \brief convenience constant of \f$ \sqrt{\frac{2 m_e}{h}} =
 *  0.5130199320647456 [eV^{-0.5} A^{-1}] \f$
 */
#define SQRT_M2_H 0.5130199320647456   /* sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 * special definitions
 *********************************************************************/

/*! \define N_RFACTORS 6
 *  \brief The maximum number of possible R factors
 */
#define N_RFACTORS     6       /* Number of possible R-factors */

/*! \define ENG_TOLERANCE
 *  \brief The accuracy in comparing energies using floating point.
 */
#define ENG_TOLERANCE  0.1     /* accuracy in comparing energies */

/*! \define IND_TOLERANCE
 *  \brief The accuracy in comparing indices using floating point.
 */
#define IND_TOLERANCE  0.02    /* accuracy in comparing indices */

/*! \define ZERO_TOLERANCE
 *  \brief Any intensities smaller than this value are considered to be zero
 */
#define ZERO_TOLERANCE 1.e-10  /*!< intensities smaller than this value
                                * are considered to be zero */

#define I_FAIL       -1        /*!< integer return value if failed */
#define F_FAIL       -1.       /*!< float return value if failed */

#define SM_LORENTZ    1        /*!< flag for Lorentzian smooth */
#define ALL_CURVES   NULL      /*!< flag for rf_cmpr: use all IV curves
                                * for R-factor calculation */
#define DEFAULT_GROUP_ID  1    /*!< default group ID */
#define AVERAGE_GROUP_ID -1    /*!< group ID for average over all curves */

/*
 Version 1.1
*/

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

#define RG_IGNORE_MAX  400.    /*!< Ignore Features larger than 100eV (defines
                                * the lower limit in Fourier space
                                * when calculating Rg-Factor) */

/*********************************************************************
 * structures and types
 *********************************************************************/

/*!
 * Indicates which R-factors shall be calculated.
 */
typedef struct
{
  rfac_axis ek;       /*!< Energy axis: I(E): 0; I(k): 1 (1.1) */
  bool r_1;           /*!< R1-factor */
  bool r_2;           /*!< R2-factor */
  bool r_b;           /*!< Rb-factor */
  bool r_g;           /*!< Rg-factor */
  bool r_p;           /*!< Pendry's R-factor */
} rfac_rf_switch;


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
  int group_id;           /*!< Assignment to a certain group of
                           * curves e.g. integral/superstructure */
  real eng_0;             /*!< Energy of beam appearance (1.1) */
  rfac_spot spot_id;      /*!< One labeling set of spot indices */
  size_t n_geo;           /*!< Number of different theoretical IV curves */
  
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
  rfac_rf_switch r_switch;    /*!< Calculate an r-factor or not */
  char *ctr_file;             /*!< Input control file string or path */
  char *the_file;             /*!< Input theory file string or path */
  char *out_file;             /*!< Output file string or path */
  char *iv_file;              /*!< File name or path for IV curves */
  bool iv_out;                /*!< Flag for output of IV curves */
  rfactor_type r_type;        /*!< R-factor type */
  real s_ini;                 /*!< Shift of initial energy */
  real s_fin;                 /*!< Shift of final energy */
  real s_step;                /*!< Shift of energy step */
  real vi;                    /*!< Imaginary part of optical potential */
  bool all_groups;            /*!< Flag: print R-factors of all group ID's */

  /* added for compatability with older routines */
  int *p_geo;
  real *p_shift;

} rfac_args;

typedef struct
{
 float rf;
 int geo;
 int shift;
} rfac_par;

/*!
 * Contains the minimum R-factor using different evaluation methods.
 */
typedef struct
{
 int group_id;
 rfac_par r_1;
 rfac_par r_2;
 rfac_par r_b1;
 rfac_par r_b2;
 rfac_par r_g;
 rfac_par r_p;
} rfac_rmin;

typedef struct rfref  /*! Relation between experimental and theory lists */
{
 int i_min;           /*!< index of smallest energy in theor. list */
 int i_max;           /*!< index of largest energy in theor. list */
 int *pindex;         /*!< list of indices in exper. list corresponding to
                       * theoretical energies */
} rfac_ref;

/*********************************************************************
 * End of include file
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_DEF_H */
