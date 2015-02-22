/*********************************************************************
 *                        MKIV_DEF.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   CS/08.09.93 - creation.
 *   CS/28.7.93   PYTH2, PYTH, MIN, MAX
 *********************************************************************/

/*! \file
 *
 * Header file for \c mkiv definitions, macros and structures.
 */

#ifndef MKIV_DEF_H
#define MKIV_DEF_H

#include <tiffio.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "gh_stddef.h"

#ifdef __cplusplus__ /* add header compatibility with C++ */
namespace cleed {
#endif

/* macros */
#ifdef __cplusplus

template <typename T> inline T pythag_sq(T x, T y)
{
  return (pow(x, 2) + pow(y, 2));
}

template <typename T> inline T pythag(T x, T y)
{
  return (sqrt(pythag_sq(x, y)));
}

#else /* do not use type-generic macros */

#define PYTH2(x,y) (pow((x), 2) + pow((y), 2))
#define PYTH(x, y) (sqrt(PYTH2(x, y)))
#define NAT(x) ( fabs((x) - int_round((x))) < DBL_EPSILON )

#define pythag_sq PYTH2
#define pythag PYTH

#endif

/* enums, constants, etc. for mkiv */
enum {
  VERBOSE = 1,
  QUICK = 2
};
static inline bool QQ(int verb) { return ( !(verb & QUICK) ); }

/* array dimensions */
#define    STRSZ          2048     /*!< Max. string size                  */
#define    I_MAX          2048     /*!< Max. number of IV curves (spots)  */
#define    D_MAX            20     /*!< Max. number of domains            */
#define    E_MAX          2048     /*!< Max. number of Energy points      */

/* adjustable values */
#define    KMAX            5.      /*!< Max. k-value                      */
#define    THICK           1.      /*!< Thickness                         */
#define    WHITE           255     /*!< Greylevel of circle line          */
#define    TOLERANCE       0.05    /*!< Tolerance for 2 identical float values */

/*  constants  */
static const double H2M  = 1.951939668076095; /*!< \f$ \frac{h}{sqrt{2 m}} \f$ in eV^-0.5*A */
static const double K10 = 2.6771;             /*!< kp_len of (1,0) beam in A^-1 */
static const double ROOT2 = 1.4142136;        /*!< \f$ \sqrt{2} \f$ */
enum { INT_OUT = -999 };                      /*!< Default value if sum not defined */

/* control bits/values */
enum { BG_YES = 1 };
enum { DEV_BIG = 1 };
enum { DEV_SMALL = 0 };

/* spot->control bits */
typedef enum {
  SPOT_EXCL = 1,
  SPOT_DESI = 2,
  SPOT_REF = 4,
  SPOT_GOOD_S2N = 0x20,
  SPOT_OUT = 0x40
} mkiv_spot_control_bit;

/* structures */
#ifdef __cplusplus__ /* add header compatibility with C++ */
extern "C" {
#endif
/*!
 * \struct mkiv_reflex
 *
 * Structure for containing diffraction spot information used in \c mkiv .
 */
typedef struct mkiv_reflex
{
  double kp_len;          /*!< Length of \f$ \vec{k} \f$ parallel to surface */
  double lind1;           /*!< First index of reflex */
  double lind2;           /*!< Second index of reflex */
  double y0;              /*!< Calculated vertical reflex position  */
  double x0;              /*!< Calculated horizontal reflex position */
  double yy;              /*!< Vertical component of reflex position */
  double xx;              /*!< Horizontal component of reflex position */
  double cos_th;          /*!< Cosine of reflex */
  double intensity;       /*!< Intensity of reflex */
  double s2n;             /*!< Signal to noise ratio of reflex */
  double s2u;		          /*!< Signal to underground ratio */
  int64_t control;        /*!< Control byte */
} mkiv_reflex;

typedef struct mkiv_index
{
  double lind1;        /*!< First index of reflex */
  double lind2;        /*!< Second index of reflex */
  int64_t control;     /*!< Control byte */
} mkiv_index;

/*!
 * \struct mkiv_domain
 *
 * Contains the superstructure matrix elements describing the domain.
 */
typedef struct mkiv_domain
{
  double lind11;        /*!< m11 */
  double lind12;        /*!< m12 */
  double lind21;        /*!< m21 */
  double lind22;        /*!< m22 */
} mkiv_domain;

/*!
 * \struct mkiv_position
 *
 * Structure for indicating an integer position for x and y.
 * This is primarily used for storing height and width
 * information for the #mkiv_image structure.
 */
typedef struct mkiv_pos
{
  int xx;
  int yy;
} mkiv_position;

/*!
 * \struct mkiv_vector
 *
 * Position vector structure used in \c mkiv .
 */
typedef struct mkiv_vector
{
  double len;
  double xx;
  double yy;
} mkiv_vector;

/*!
 * \struct mkiv_image
 *
 * Contains an internal format for storing 2D (matrix) image data.
 */
typedef struct mkiv_image
{
  uint32 rows;        /*!< Number of rows in image (height) */
  uint32 cols;        /*!< Number of columns in image (width) */
  uint32 *imagedata;  /*!< Pointer to the image data. The 2D data is stored as
                       * a flattened matrix, where
                       * \code data[i][j] = j*columns + i \endcode */
} mkiv_image;

typedef struct mkiv_tif_values_str
{
  TIFF *tif_in;       /*!< Pointer to TIFF object for input */
  TIFF *tif_out;      /*!< Pointer to TIFF object for output */
  uint64_t imagewidth;
  uint64_t imagelength;
  uint16_t bitspersample;
  uint16_t compression;
  uint16_t photometricinterpretation;
  uint64_t stripoffsets;
  uint64_t rowsperstrip;
  uint64_t stripbytecounts;
  double xresolution;
  double yresolution;
  uint16_t planarconfiguration;
  uint16_t resolutionunit;

  char *buf;

  int energy;
  int current;
} mkiv_tif_values;

typedef struct {
  char ref_name[FILENAME_MAX];
  size_t n_start;         /*!< Number of first LEED image in sequence */
  size_t n_stop;          /*!< Number of last LEED image in sequence */
  double e_step;          /*!< Energy step between images */
  mkiv_position center;   /*!< Middle position of LEED screen */
  double r_outer;         /*!< Outer radius of screen */
  double r_inner;         /*!< Inner radius of screen */
  int bkgnd;
  int smooth;
  double kpl_10;          /*!< Parallel length of (1,0) beam */

  size_t n_dom;                 /*!< Number of domains */
  mkiv_domain superlat[D_MAX];  /*!< Basis superlattice vectors in units of a1,a2 */
  size_t n_desi;                /*!< Number of desired reflexes */
  mkiv_index desi[I_MAX];       /*!< Array of desired reflexes */
  size_t n_ref;                 /*!< Number of reference reflexes */
  mkiv_index ref[I_MAX];        /*!< Array of reference reflexes */
  size_t n_excl;                /*!< Number of excluded reflexes */
  mkiv_index excl[I_MAX];       /*!< Array of excluded reflexes */
} mkiv_input;

typedef struct {
  double cos_min;     /*!< Minimum cosine for reference spots    */
  double cos_max;     /*!< Minimum cosine for reference spots    */
  double verh;        /*!< Vertical/horizontal integration ratio */
  double acci;        /*!< Integration ratio 'a' */
  double accb;        /*!< Integration ratio 'b' */
  int distance;       /*!< Minimum distance for reference spots */
  size_t update_freq; /*!< Update step for outputting images */
  int step;           /*!< Step for mkiv_find_imax() */
  size_t trip_max;    /*!< Number of triples for recalibration of spot positions */
  double scale_min;   /*!< Minimum scaling */
  double range_min;   /*!< Minimum range */
  size_t ref_min;     /*!< Minimum number of predefined spots for recalibration */
  double sec_range;   /*!< Range (2nd loop) = range(1st)/sec_range */

  double s2n_good;    /*!< Threshold for good signal-noise */
  double s2n_bad;     /*!< Threshold for bas signal-noise */
  double s2n_ref;     /*!< Reference value minimum for basis recalibration */

  double ref_dev;     /*!< Maximum deviation of new base from origin */
  double bas_rat;     /*!< Ratio of basis... ?? */
  double range;
  mkiv_vector scale;
  double angle;
  double ratio;       /*!< Ratio of (screen radius)/(camera distance)    */
  int modify_current; /*!< 0: use stored beam current
                       *   1: smooth stored beam current
                       *  -1: use mod_cur as beam current       */
  size_t repetitions;
} mkiv_params;

typedef struct {

} mkiv_integration;

typedef struct {

} mkiv_screen;

typedef struct {
    double s2n_good;    /*!< Threshold for good signal-noise */
    double s2n_bad;     /*!< Threshold for bas signal-noise */
    double s2n_ref;     /*!< Reference value minimum for basis recalibration */
} mkiv_s2n;

typedef struct {
  bool verbose; /*!< verbose&VERBOSE -> more info about run
                 *   verbose&QUICK -> quick and quiet mode   */
  bool make_mask;
  bool save_intermediates;
  bool show_bounds;

  int beam_current;

  /* IO paths */
  char inp_path[FILENAME_MAX];
  char mask_path[FILENAME_MAX];
  char param_path[FILENAME_MAX];
  char pos_path[FILENAME_MAX];
  char var_path[FILENAME_MAX];
  char beam_raw_path[FILENAME_MAX];
  char beam_smoothed_path[FILENAME_MAX];
  char ivdat_path[FILENAME_MAX];
} mkiv_args;

#ifdef __cplusplus__ /* add header compatibility with C++ */
} /* extern "C" */
} /* namespace cleed */
#endif

#endif /* MKIV_DEF_H */
