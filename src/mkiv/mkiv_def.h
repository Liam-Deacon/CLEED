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

#ifdef __cplusplus__ /* add header compatibility with C++ */
extern "C" {
#endif

#include <tiffio.h>
#include <float.h>
#include <math.h>

/* macros */
#define    ARG_IS(str)     (strcmp(argv[i], str) == 0)
#define    SQUARE(x)       (x)*(x)   /* the brackets are necessary 
                                        because of PYTH and the    
                                        priority of multiplication        */
#define    PYTH2(x,y)      SQUARE(x) + SQUARE(y)
#define    PYTH(x,y)       sqrt( PYTH2(x,y) )
#define    RND(x)          (int)( (x) + .5 )
#define    RNDUP(x)        (int)( (x) + 1. )
#define    NAT(x)          ( fabs((x) - RND(x)) < DBL_EPSILON )
#ifndef    MIN
#define    MIN(x,y)        ((x) < (y)) ? (x) : (y) /*!< determines the minimum */
#endif
#ifndef    MAX
#define    MAX(x,y)        ((x) > (y)) ? (x) : (y) /*!< determines the maximum */
#endif
#ifndef    YES
#define    YES             1
#define    NO              0
#endif

#define  ERR_EXIT(text)                                                     \
{                                                                           \
    fprintf(stderr, "***error " #text "\n");                                \
    exit(1);                                                                \
}

#define  ERR_EXIT_X(text, x)                                                \
{                                                                           \
    fprintf(stderr, "*** error " #text "\n", x);                            \
    exit(1);                                                                \
}

#define STRCPY_ARG(x)                                                       \
if (i+1 < argc) {i++; strcpy(x, argv[i]);}                                  \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}

#define FLOAT_ARG(x)                                                        \
if (i+1 < argc) {i++; x = atof(argv[i]);}                                   \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}

#define INT_ARG(x)                                                          \
if (i+1 < argc) {i++; x = atoi(argv[i]);}                                   \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}


/* macros for mkiv.c */
#define  VERBOSE           1
#define  QUICK             2
#define  VPRINT            if ( verb & VERBOSE ) printf
#define  QQ                if ( !(verb & QUICK) )

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
#define    PI     3.141592653589793  /*!< \f$ \pi \f$ */
#define    H2M    1.951939668076095  /*!< \f$ \frac{h}{sqrt{2 m}} \f$ in eV^-0.5*A */
#define    K10    2.6771             /*!< kp_len of (1,0) beam in A^-1 */
#define    ROOT2  1.4142136          /*!< \f$ \sqrt{2} \f$ */
#define    INT_OUT  -999             /*!< Default value if sum not defined */

/* control bits/values */
#define    BG_YES           1
#define    DEV_BIG          1
#define    DEV_SMALL        0

/* spot->control bits */
#define    SPOT_EXCL        1
#define    SPOT_DESI        2
#define    SPOT_REF         4
#define    SPOT_GOOD_S2N    0x20
#define    SPOT_OUT         0x40

/* structures */

/*!
 * \struct mkiv_reflex
 *
 * Structure for containing diffraction spot information used in \c mkiv .
 */
typedef struct mkiv_reflex
{
  float kp_len;          /*!< Length of \f$ \vec{k} \f$ parallel to surface */
  float lind1;           /*!< First index of reflex */
  float lind2;           /*!< Second index of reflex */
  float y0;              /*!< Calculated vertical reflex position  */
  float x0;              /*!< Calculated horizontal reflex position */
  float yy;              /*!< Vertical component of reflex position */
  float xx;              /*!< Horizontal component of reflex position */
  float cos_th;          /*!< Cosine of reflex */
  float intensity;       /*!< Intensity of reflex */
  float s2n;             /*!< Signal to noise ratio of reflex */
  float s2u;		         /*!< Signal to underground ratio */
  long int control;      /*!< Control byte */
} mkiv_reflex;

typedef struct mkiv_index
{
  float lind1;        /*!< First index of reflex */
  float lind2;        /*!< Second index of reflex */
  long int control;   /*!< Control byte */
} mkiv_index;

/*!
 * \struct mkiv_domain
 *
 * Contains the superstructure matrix elements describing the domain.
 */
typedef struct mkiv_domain
{
  float lind11;        /*!< m11 */
  float lind12;        /*!< m12 */
  float lind21;        /*!< m21 */
  float lind22;        /*!< m22 */
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
  float len;
  float xx;
  float yy;
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
  unsigned long imagewidth;
  unsigned long imagelength;
  unsigned short bitspersample;
  unsigned short compression;
  unsigned short photometricinterpretation;
  unsigned long stripoffsets;
  unsigned long rowsperstrip;
  unsigned long stripbytecounts;
  float xresolution;
  float yresolution;
  unsigned short planarconfiguration;
  unsigned short resolutionunit;

  char *buf;

  int ENERGY;
  int CURRENT;
} mkiv_tif_values;

#ifdef __cplusplus__ /* add header compatibility with C++ */
}
#endif

#endif /* MKIV_DEF_H */
