/*********************************************************************
 *                           PATT_DEF.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.08.23 - header created by split from patt.c
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * \brief File contains \c enum, \c struct and \code #define \endcode declarations
 * for \c patt.
 */

#ifndef PATT_DEF_H
#define PATT_DEF_H

#include <limits.h>
#include <stdio.h>
#include "matrix_2x2.h"
#include "spots.h"
#include "coord.h"
#include "patt_colors.h"
#include "pattern.h"
#include "basis.h"
#include "gh_stddef.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
namespace cleed {
#endif

/* formats */
/*! \enum patt_format
 *  \brief Indicates output format of generated LEED pattern.
 */
typedef enum {
  PATT_PS_OLD,        /*!< Use old postscript backend */
  PATT_EPS,           /*!< Encapsulated postscript output */
  PATT_PS,            /*!< Postscript output */
  PATT_PNG,           /*!< Portable network graphics output */
  PATT_PDF,           /*!< Portable document format output */
  PATT_SVG,           /*!< Scalable vector graphics output */
  PATT_UNKNOWN_FORMAT /*!< Unknown output format */
} patt_format;

/*! \enum patt_color_scheme
 *  \brief Indicates color scheme for LEED pattern.
 */
typedef enum {
  PATT_MONOCHROME_SCHEME,   /*!< Black and white output only */
  PATT_GRAYSCALE_SCHEME,    /*!< Grayscale output only */
  PATT_COLOR_SCHEME,        /*!< Color output enabled */
  PATT_UNKNOWN_SCHEME       /*!< Invalid color scheme */
} patt_color_scheme;

/*! \enum patt_error
 *  \brief patt return codes.
 */
typedef enum {
  PATT_SUCCESS=0,         /*!< Indicates function success */
  PATT_ARGUMENT_ERROR,    /*!< Indicates invalid argument error */
  PATT_READ_ERROR,        /*!< Indicates error reading from file */
  PATT_WRITE_ERROR,       /*!< Indicates error writing to file */
  PATT_COLOR_ERROR,       /*!< Indicates invalid color error */
  PATT_FORMAT_ERROR,      /*!< Indicates invalid format error */
  PATT_ALLOC_ERROR,       /*!< Indicates unable to allocate memory error */
  PATT_DEALLOC_ERROR,     /*!< Indicates unable to free memory error */
  PATT_SHAPE_ERROR,       /*!< Indicates invalid shape error */
  PATT_UNKNOWN_ERROR      /*!< Indicates an unknown error */
} patt_error;


#ifdef __cplusplus
static const double PATT_PAGE_HEIGHT = 800.; /*!< Height of bounding box for drawing. */
static const double PATT_PAGE_WIDTH = 600.;  /*!< Width of bounding box for drawing. */

static const double ARROW_ANGLE = 0.5; /*!< Default angle for drawing arrow heads. */

static const double OFF_H = 400.;     /*!< Height offset for drawing. */
static const double OFF_W = 300.;     /*!< Width offset for drawing. */

static const double LINE_WIDTH = 2.;  /*!< Default line width for drawing. */

static const double MAX_RADIUS = 200.; /*!< Maximum radius for pattern
                                        * (analogous to the Ewald sphere). */
#else
#define PATT_PAGE_HEIGHT 800. /*!< Height of bounding box for drawing. */
#define PATT_PAGE_WIDTH 600.  /*!< Width of bounding box for drawing. */

#define ARROW_ANGLE 0.5 /*!< Default angle for drawing arrow heads. */

#define OFF_H 400.     /*!< Height offset for drawing. */
#define OFF_W 300.     /*!< Width offset for drawing. */

#define LINE_WIDTH 2.  /*!< Default line width for drawing. */

#define MAX_RADIUS 200. /*!< Maximum radius for pattern
                         * (analogous to the Ewald sphere). */
#endif

enum { MAX_INPUT_FILES = 6 }; /*!< Number of possible input files to be
                               * simultaneously rendered in the drawing. */

enum { MAX_DOMAINS = 128 };   /*!< Maximum number of allowed super-structure
                               *   domains per substrate pattern. */

static const double GREY_GS = 0.;

/* alter the following list for order of preference */
enum { NUM_SUBS = 54 };

/* map to postscript symbol font */
extern const char substitutes[NUM_SUBS][2][STRSZ];

char line_buffer[STRSZ];

/*******************************************************
Pattern Structures
*******************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  extern "C" {
#endif

/*! \struct patt_text
 *  \brief Struct for displaying text label.
 */
typedef struct patt_text 
{
  patt_color_rgb color;   /*!< Color of text label */
  double alpha;           /*!< Alpha transparency (0. to 1. is 0 to 100%) */
  char label[BUFSIZ];     /*!< Text string to be displayed */
  double x;               /*!< x-position for text */
  double y;               /*!< y-position for text */
  double size;            /*!< Text size */
  bool visible;           /*!< Indicates whether to show text */
} patt_text;
extern patt_text text_default;


/*! \struct patt_screen
 *  \brief Structure for containing LEED screen drawing parameters.
 */
typedef struct patt_screen
{
  double alpha;         /*!< % alpha transparency of screen (if supported) */
  patt_color_rgb stroke_color;  /*!< Line color of screen */
  patt_color_rgb fill_color;    /*!< Fill color of screen */
  double radius;        /*!< Screen radius (default is #MAX_RADIUS ) */
  double stroke_width;  /*!< Screen edge thickness */
  bool clip;            /*!< Clip pattern to radius for screen */
  bool fill;            /*!< Indicates whether to apply background fill to screen */
  bool visible;         /*!< Indicates whether to show screen */
} patt_screen;
extern patt_screen screen_default;


/*! \struct patt_gun
 *  \brief Structure for containing electron gun drawing parameters.
 */
typedef struct patt_gun 
{
  double x;                 /*!< x-axis centre for gun */
  double y;                 /*!< y-axis centre for gun */
  double stroke_width;      /*!< line width of drawing strokes */
  patt_color_rgb color;     /*!< fill color of electron gun */
  double radius;            /*!< radius of gun */
  double angle;             /*!< angle, in radians, where gun support to be shown */
  bool fill;                /*!< determines whether to fill in gun */
  bool visible;             /*!< determines gun visibility */
  double alpha;             /*!< fractional alpha transparency (0. to 1.) */
} patt_gun;
extern patt_gun gun_default; 


/*! \struct patt_vector
 *  \brief Structure for containing basis vector drawing details.
 */
typedef struct patt_vector
{
  double x1;                /*!< Initial x-coordinate for vector */
  double y1;                /*!< Initial y-coordinate for vector */
  double x2;                /*!< Final x-coordinate for vector */
  double y2;                /*!< Final y-coordinate for vector */
  double alpha;             /*!< Fractional alpha transparency from 0. to 1. */
  patt_color_rgb color;     /*!< Color of basis vector */
  double linewidth;         /*!< Line width of basis vector */
  double head_size;

} patt_vector;
extern patt_vector vector_default; 


/*! \struct patt_drawing
 *  \brief Master struct for containing all drawing parameters.
 */
typedef struct patt_drawing 
{
  basis basis_gs[MAX_INPUT_FILES];
  basis basis_ss[MAX_INPUT_FILES][MAX_DOMAINS];
  patt_text eV;                      /*!< Label for energy if used */
  patt_text title;                   /*!< Label for drawing title */
  patt_text footnote;                /*!< Label for drawing footnote */
  patt_text legend[MAX_INPUT_FILES]; /*!< Legend for drawing */
  patt_gun gun;                      /*!< Electron gun drawing details */
  patt_screen screen;                /*!< LEED screen drawing details */
  patt_color_scheme color_scheme;    /*!< Color scheme for drawing */
  patt_format format;         /*!< Format for output file e.g. svg or pdf */
  spots spots[2];             /*!< Container for drawing style for both
                               *   GS_SPOTS (subtrate) and
                               *   SS_SPOTS (superstructure) */
  size_t energy;              /*!< Energy for simulating Ewald sphere */
  bool show_overlap;          /*!< Show spots on top of gun and edge of screen */
  bool show_vectors;          /*!< Show basis vectors in output drawing */
  bool show_indices;          /*!< Show Miller indices in output drawing */
  bool symbols;               /*!< Use different symbols for each domain */
  bool fill_gs;               /*!< Fill in substrate spots with spot color */
  bool fill_ss;               /*!< Fill in superstructure spots */
  bool show_gs_vectors[MAX_INPUT_FILES];
  bool show_ss_vectors[MAX_INPUT_FILES][MAX_DOMAINS];
  bool show_gs_indices[MAX_INPUT_FILES];
  bool show_ss_indices[MAX_INPUT_FILES][MAX_DOMAINS];
  size_t n_files;
  char input_files[MAX_INPUT_FILES][FILENAME_MAX];
  char output_filename[FILENAME_MAX];
  bool std_in;              /*!< Indicates whether to use \c stdin as input */
  bool std_out;             /*!< Indicates whether to use \c stdout as output */
  bool interactive;         /*!< Indicates whether to run patt interactively */
  size_t i_dom;
  size_t i_spot;
} patt_drawing;
extern patt_drawing drawing_default;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  } /* extern "C" */
} /* namespace cleed */
#endif

#endif /* PATT_DEF_H */
