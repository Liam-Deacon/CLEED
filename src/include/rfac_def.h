/**
 * @file rfac_def.h
 * @brief Type definitions and constants for R-factor calculations.
 *
 * This file provides:
 * - Structures for I-V curve storage and comparison
 * - R-factor result containers
 * - Physical constants and tolerance values
 *
 * @ingroup rfac_core
 *
 * Changes:
 * GH/25.09.92
 * GH/03.03.93
 * GH/10.08.95 - Create (copy from rfdefines.h and rftypes.h)
 */

#ifndef RFAC_DEF_H
#define RFAC_DEF_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "real.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/**
 * @defgroup rfac_types R-Factor Data Types
 * @ingroup rfac_core
 * @brief Structures for I-V curves and R-factor results.
 * @{
 */

/*********************************************************************
 structures and types 
*********************************************************************/

/**
 * @brief Flags indicating which R-factors to calculate.
 *
 * If this structure is changed, the functions rfrsw, rfcmpr,
 * rfoutput must also be updated.
 */
struct rfrswitch
{
 int ek;            /**< Energy axis: I(E): 0; I(k): 1 */
 int r_1;           /**< Calculate R1-factor */
 int r_2;           /**< Calculate R2-factor */
 int r_b;           /**< Calculate Rb-factor */
 int r_g;           /**< Calculate Rg-factor */
 int r_p;           /**< Calculate Pendry's R-factor */
};

/**
 * @brief Mapping between experimental and theoretical energy lists.
 */
struct rfref
{
 int i_min;         /**< Index of smallest energy in theoretical list */
 int i_max;         /**< Index of largest energy in theoretical list */
 int *pindex;       /**< List of indices in experimental list corresponding
                         to theoretical energies */
};

/**
 * @brief Single data point in an experimental I-V curve.
 */
struct rfelist 
{
 float energy;      /**< Energy value in experimental I-V curve (eV) */
 float intens;      /**< Intensity value in experimental I-V curve */
};

/**
 * @brief Single data point in a theoretical I-V curve.
 */
struct rftlist 
{
 float energy;      /**< Energy value in theoretical I-V curve (eV) */
 float *pintens;    /**< List of intensity values for different geometries */
};

/**
 * @brief Container for all R-factor values for a single beam.
 */
struct rfrfac
{
 int group_id;      /**< Either AVERAGE_ID or group flag */
 float r_1;         /**< R1-factor (absolute difference integral) */
 float r_2;         /**< R2-factor (squared difference integral) */
 float r_b1;        /**< Rb1-factor */
 float r_b2;        /**< Rb2-factor */
 float r_g;         /**< Rg-factor (goodness of fit) */
 float r_p;         /**< Pendry's R-factor (logarithmic derivative) */
 float rr;          /**< RR factor (confidence level) */
 float i_ratio;     /**< Ratio of theoretical/experimental curve integral */
 float e_range;     /**< Total energy range (eV) */
};

/**
 * @brief Spot identification for a diffraction beam.
 */
struct rfspot 
{
 float index1;      /**< First Miller index (h) */
 float index2;      /**< Second Miller index (k) */
 float f_val1;      /**< Can be used for length in k space */
 float f_val2;      /**< Can be used for intensity */
 int i_val1;        /**< Arbitrary integer value 1 */
 int i_val2;        /**< Arbitrary integer value 2 */
};

typedef struct rfspot rfac_spot;
typedef struct rfrfac rfac;

typedef enum {
  RP_FACTOR = 1,
  R1_FACTOR = 2,
  R2_FACTOR = 3,
  RB_FACTOR = 4
} rfactor_type;

#ifndef END_OF_GROUP_ID
#define END_OF_GROUP_ID -1
#endif

typedef struct rfac_iv_data
{
  real energy;
  real intens;
  real deriv2;
} rfac_iv_data;

typedef struct rfac_iv
{
  rfac_iv_data *data;
  size_t n_eng;
  bool equidist;
  bool sort;
  bool smooth;
  bool spline;
  real first_eng;
  real last_eng;
  real max_int;
} rfac_iv;

typedef struct rfac_ivcur
{
  int group_id;
  real eng_0;
  rfac_spot spot_id;
  rfac_iv *theory;
  rfac_iv *experimental;
  real overlap;
  rfac rfac;
  real weight;
} rfac_ivcur;

typedef struct rfac_args
{
  char ctr_file[FILENAME_MAX];
  char the_file[FILENAME_MAX];
  char out_file[FILENAME_MAX];
  char iv_file[FILENAME_MAX];
  rfactor_type r_type;
  real s_ini;
  real s_fin;
  real s_step;
  real vi;
  bool all_groups;
} rfac_args;

/**
 * @brief Complete I-V curve data for a single beam.
 *
 * Contains both theoretical and experimental I-V data,
 * along with metadata and computed R-factors.
 */
struct rfivcur 
{
 int     group_id;           /**< Assignment to a curve group 
                                  (integral/superstructure) */
 float   eng_0;              /**< Energy of beam appearance (eV) */
 struct rfspot  spot_id;     /**< Spot indices identifying this beam */

 struct rftlist *the_list;   /**< Theoretical I-V curve data */
 int     the_leng;           /**< Number of data pairs in theoretical list */
 int     the_ngeo;           /**< Number of different theoretical geometries */
 int     the_equidist;       /**< Flag: theoretical energies equidistant */
 int     the_sort;           /**< Flag: theoretical energies sorted */
 int     the_smooth;         /**< Flag: smoothing has been applied */
 float   the_first_eng;      /**< First energy in theoretical list (eV) */
 float   the_last_eng;       /**< Last energy in theoretical list (eV) */
 float   *pthe_max_int;      /**< Maximum intensity per geometry */

 struct rfelist *exp_list;   /**< Experimental I-V curve data */
 int     exp_leng;           /**< Number of data pairs in experimental list */
 int     exp_equidist;       /**< Flag: experimental energies equidistant */
 int     exp_sort;           /**< Flag: experimental energies sorted */
 int     exp_smooth;         /**< Flag: smoothing has been applied */
 float   exp_first_eng;      /**< First energy in experimental list (eV) */
 float   exp_last_eng;       /**< Last energy in experimental list (eV) */
 float   exp_max_int;        /**< Maximum intensity in experimental curve */

 float   overlap;            /**< Overlapping energy range (eV) */
 struct rfrfac  rfac;        /**< Computed R-factors for this beam */
};

/**
 * @brief Command-line arguments for R-factor program.
 */
struct rfargs 
{
 char  *ctrfile;             /**< Input control file path */
 char  *thefile;             /**< Input theory file path */
 int   *p_geo;               /**< List of trial geometries to test */
 float *p_shift;             /**< Energy axis shift values (eV) */
 struct rfrswitch r_switch;  /**< Which R-factors to calculate */
 int   all_groups;           /**< Flag: print R-factors for all group IDs */
 char  *outfile;             /**< Output file path */
 float vi;                   /**< Imaginary part of optical potential (eV) */
 float ignore_max;           /**< Ignore features larger than this in Rg */
};

/**
 * @brief Single R-factor result with geometry/shift info.
 */
struct rfpar
{
 float rf;                   /**< R-factor value */
 int geo;                    /**< Geometry index */
 int shift;                  /**< Energy shift index */
};

/**
 * @brief Minimum R-factor values across all geometries/shifts.
 */
struct rfmin
{
 int group_id;               /**< Group identifier */
 struct rfpar r_1;           /**< Best R1 value */
 struct rfpar r_2;           /**< Best R2 value */
 struct rfpar r_b1;          /**< Best Rb1 value */
 struct rfpar r_b2;          /**< Best Rb2 value */
 struct rfpar r_g;           /**< Best Rg value */
 struct rfpar r_p;           /**< Best Rp value */
};

/** @} */ /* end of rfac_types group */

/**
 * @defgroup rfac_constants R-Factor Constants
 * @ingroup rfac_core
 * @brief Physical constants and tolerance values.
 * @{
 */

/*********************************************************************
 general definitions / constants
*********************************************************************/

#define M2_H      0.2631894506957162   /**< 2*m/h [eV^-1 A^-2] */
#define SQRT_M2_H 0.5130199320647456   /**< sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 special definitions
*********************************************************************/

#define N_RFACTORS     7       /**< Number of possible R-factors */

#define ENG_TOLERANCE  0.1     /**< Accuracy in comparing energies (eV) */
#define IND_TOLERANCE  0.02    /**< Accuracy in comparing indices */
#define ZERO_TOLERANCE 1.e-10  /**< Intensities below this are zero */

#define I_FAIL       -1        /**< Integer return value if failed */
#define F_FAIL       -1.       /**< Float return value if failed */

#define SM_LORENTZ    1        /**< Flag for Lorentzian smoothing */
#define ALL_CURVES   NULL      /**< Flag for rf_cmpr: use all I-V curves */
#define DEFAULT_GROUP_ID  1    /**< Default group ID */
#define AVERAGE_GROUP_ID -1    /**< Group ID for average over all curves */

/*
 Version 1.1
*/

#define E_AXIS  0    /**< Use I(E) curve for R-factor calculations */
#define K_AXIS  1    /**< Use I(k) curve for R-factor calculations */

#define RG_IGNORE_MAX  400.    /**< Ignore features larger than 400eV
                                    (lower limit in Fourier space for Rg) */

/** @} */ /* end of rfac_constants group */

/*********************************************************************
 End of include file
*********************************************************************/
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_DEF_H */
