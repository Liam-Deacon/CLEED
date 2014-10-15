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

/*! \file rfac_def.h
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
 *  \brief convenience constant of 2*m_e/h = 0.2631894506957162 [eV^-1   A^-2]
 */
#define M2_H      0.2631894506957162   /* 2*m/h       [eV^-1   A^-2] */

/*! \define SQRT_M2_H
 *  \brief convenience constant of sqrt(2*m_e/h) = 0.5130199320647456 [eV^-0.5 A^-1]
 */
#define SQRT_M2_H 0.5130199320647456   /* sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 * special definitions
 *********************************************************************/

/*! \define N_RFACTORS 6
 *  \brief The maximum number of possible R-factors
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
 * \brief Indicates the return/exit status of a function.
 */
typedef enum {
  RFAC_FAILURE=-1,
  RFAC_SUCCESS,
  RFAC_INVALID_ARGUMENT,
  RFAC_IV_NOT_EQUIDISTANT,
  RFAC_ALLOCATION_ERROR,
  RFAC_ESTEP_TOO_SMALL,
  RFAC_VI_TOO_SMALL,
  RFAC_INVALID_NUMBER_OF_ENERGIES,
  RFAC_INVALID_NUMBER_OF_BEAMS,
  RFAC_BAD_IV_DATA
} rfac_error;

/*! \typedef \enum refactor
 *  \brief indicates the R-factor type.
 */
typedef enum {
  RP_FACTOR=1,    /*!< Pendry's R-factor */
  R1_FACTOR,      /*!< R1 factor */
  R2_FACTOR,      /*!< R2 factor */
  RB_FACTOR       /*!< Rb factor */
} rfactor_type;

typedef enum {
  E_AXIS=0,     /*!< use an I(E) curve for R factor calculations */
  K_AXIS        /*!< use an I(k) curve for R factor calculations */
} rfac_axis;

#define RG_IGNORE_MAX  400.    /*!< Ignore Features larger than 100eV (defines
                                * the lower limit in Fourier space
                                * when calculating Rg-Factor) */

/*********************************************************************
 * structures and types
 *********************************************************************/

/*!
 *
 * Indicates which R-factors shall be calculated.
 * If this structure is changed, the functions \fn rfrsw, \fn rfcmpr,
 * \fn rfoutput have to be changed as well */
typedef struct rfrswitch
{
  rfac_axis ek;       /*!< Energy axis: I(E): 0; I(k): 1 (1.1) */
  bool r_1;           /*!< R1-factor */
  bool r_2;           /*!< R2-factor */
  bool r_b;           /*!< Rb-factor */
  bool r_g;           /*!< Rg-factor */
  bool r_p;           /*!< Pendry's R-factor */
} rfac_rf_switch;

typedef struct rfiv
{
  real energy;      /*!< energy value in expt. IV curve */
  real intens;      /*!< intensity value in expt. IV curve */
  real deriv2;      /*!< second derivative used in cubic spline */
} rfac_iv_data;

typedef struct rfivlist
{
  rfac_iv_data *data;     /*!< pointer to the list of IV values */
  size_t n_eng;           /*!< number of data pairs in IV list */
  bool equidist;          /*!< indicates equidistant energies */
  bool sort;              /*!< indicates sorted energies */
  bool smooth;            /*!< indicates that smoothing has been done */
  bool spline;            /*!< indicates that cubic spline has been prepared */
  real first_eng;         /*!< first energy in theo. list */
  real last_eng;          /*!< last energy in theo. list */
  real max_int;           /*!< max. intensity value in list */
} rfac_iv;

typedef struct rfrfac
{
  int group_id;      /*!< either AVERAGE_ID or group flag */
  float r_1;         /*!< R1-factor */
  float r_2;         /*!< R2-factor */
  float r_b1;        /*!< Rb1-factor */
  float r_b2;        /*!< Rb2-factor */
  float r_g;         /*!< Rg-factor */
  float r_p;         /*!< Pendry's R-factor */
  float rr;          /*!< RR factor (confidence level) */
  float i_ratio;     /*!< ratio of theoretical/experimental curve integral */
  float e_range;     /*!< total energy range */
} rfac;

typedef struct rfspot
{
  real index1;       /*!< 1st index */
  real index2;       /*!< 2nd index */
  real f_val1;       /*!< can be used for length in k space e.g. */
  real f_val2;       /*!< can be used for intensity e.g. */
  int i_val1;        /*!< arbitrary use */
  int i_val2;        /*!< arbitrary use */
} rfac_spot;

typedef struct rfivcur
{
  /* general */
  int group_id;               /*!< assignment to a certain group of
                               * curves e.g. integral/superstructure */
  real eng_0;                 /*!< energy of beam appearance (1.1) */
  rfac_spot spot_id;          /*!< one labeling set of spot indices */
  size_t n_geo;               /*!< number of different theoretical IV curves */
  
  /* theoretical data */
  rfac_iv *theory;             /*!< theoretical IV curve */

  /* experimental data */
  rfac_iv *experimental;       /*!< experimental. IV curve */

  /* R factor */
  real overlap;               /*!< Energy range */
  rfac rfac;                  /*!< R-factors */
  real weight;                /*!< relative weight in average */
} rfac_ivcur;

typedef struct rfargs
{
  rfac_rf_switch r_switch;    /*!< calculate an r-factor or not */
  char *ctr_file;             /*!< input control file */
  char *the_file;             /*!< input theory file */
  char *out_file;             /*!< output file */
  char *iv_file;              /*!< file name for IV curves */
  bool iv_out;                /*!< flag for output of IV curves */
  rfactor_type r_type;        /*!< R-factor type */
  real s_ini;                 /*!< shift of energy axes */
  real s_fin;                 /*!< shift of energy axes */
  real s_step;                /*!< shift of energy axes */
  real vi;                    /*!< imaginary part of optical potential */
  bool all_groups;            /*!< flag: print R-factors of all group ID's */

  /* added for compatability with older routines */
  int *p_geo;
  real *p_shift;

} rfac_args;

typedef struct rfpar
{
 float rf;
 int geo;
 int shift;
} rfac_par;

typedef struct rfmin
{
 int group_id;
 rfac_par r_1;
 rfac_par r_2;
 rfac_par r_b1;
 rfac_par r_b2;
 rfac_par r_g;
 rfac_par r_p;
} rfac_rmin;

typedef struct rfref  /*!< relation between expt. and theo. lists */
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
