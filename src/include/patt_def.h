/*********************************************************************
 *                           PATT_DEF.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief File contains \c enum, \c struct and \code #define \endcode declarations
 * for \c patt.
 */

#ifndef PATT_DEF_H
#define PATT_DEF_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  extern "C" {
#endif

#include <limits.h>
#include <stdbool.h>
#include "matrix_2x2.h"
#include "spots.h"
#include "coord.h"
#include "patt_colors.h"
#include "pattern.h"
#include "basis.h"

#ifndef STRSZ
  #define STRSZ 128   /*! maximum length of string */
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
  PATT_SUCCESS=0,         /*!< function success */
  PATT_ARGUMENT_ERROR,    /*!< invalid argument error */
  PATT_READ_ERROR,        /*!< error reading from file */
  PATT_WRITE_ERROR,       /*!< error writing to file */
  PATT_COLOR_ERROR,       /*!< invalid color error */
  PATT_FORMAT_ERROR,      /*!< invalid format error */
  PATT_ALLOC_ERROR,       /*!< unable to allocate memory error */
  PATT_DEALLOC_ERROR,     /*!< unable to free memory error */
  PATT_SHAPE_ERROR,       /*!< invalid shape error */
  PATT_UNKNOWN_ERROR      /*!< unknown error */
} patt_error;

/*! \def PATT_PAGE_HEIGHT
 *  \brief Height of bounding box for drawing.
 */
#define PATT_PAGE_HEIGHT 800.

/*! \def PATT_PAGE_WIDTH
 *  \brief Width of bounding box for drawing.
 */
#define PATT_PAGE_WIDTH 600.

/*! \def ARROW_ANGLE
 *  \brief Default angle for drawing arrow heads.
 */
#define ARROW_ANGLE 0.5

/*! \def OFF_H
 *  \brief Height offset for drawing.
 */

#define OFF_H      400.
/*! \def OFF_W
 *  \brief Width offset for drawing. 
 */
#define OFF_W      300.

/*! \def LINE_WIDTH
 *  \brief Default line width for drawing.
 */
#define LINE_WIDTH 2.

/*! \def MAX_RADIUS
 *  \brief Maximum radius for pattern (analogous to the Ewald sphere).
 */
#define MAX_RADIUS 200.

/*! \def MAX_INPUT_FILES
 *  \brief Number of possible input files to be simultaneously rendered
 *         in the drawing.
 */
#define MAX_INPUT_FILES 6

/*! \def MAX_DOMAINS
 *  \brief The maximum number of allowed super-structure domains
 *   per substrate pattern.
 */
#define MAX_DOMAINS 128

#define GREY_GS    0.

/* alter the following list for order of preference */
#define NUM_SUBS 54

/* map to postscript symbol font */
extern const char substitutes[NUM_SUBS][2][STRSZ];

char line_buffer[STRSZ];

/*******************************************************
Pattern Structures
*******************************************************/

/*! \struct patt_text
 *  \brief Struct for displaying text label.
 */
typedef struct patt_text 
{
  patt_color_rgb color;   /*!< color of text label */
  double alpha;           /*!< alpha transparency (0. to 1. is 0 to 100%) */
  char label[BUFSIZ];     /*!< text string to be displayed */
  double x;               /*!< x-position for text */
  double y;               /*!< y-position for text */
  double size;            /*!< text size */
  bool visible;           /*!< show text */
} patt_text;
extern patt_text text_default;


/*! \struct screen
 *  \brief Struct for containing LEED screen drawing parameters.
 */
typedef struct patt_screen
{
  double alpha;           /*!< % alpha transparency of screen (if supported) */
  patt_color_rgb stroke_color;  /*!< line color of screen */
  patt_color_rgb fill_color;    /*!< fill color of screen */
  double radius;          /*!< screen radius (default is MAX_RADIUS)*/
  double stroke_width;    /*!< screen edge thickness */
  bool clip;              /*!< clip pattern to radius for screen */
  bool fill;              /*!< background fill of screen */
  bool visible;           /*!< show screen */
} patt_screen;
extern patt_screen screen_default;


/*! \struct patt_gun
 *  \brief Struct for containing electron gun drawing parameters.
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
 *  \brief Struct for containing basis vector drawing details.
 */
typedef struct patt_vector
{
  double x1;                /*!< initial x-coordinate for vector */
  double y1;                /*!< initial y-coordinate for vector */
  double x2;                /*!< final x-coordinate for vector */
  double y2;                /*!< final y-coordinate for vector */
  double alpha;             /*!< fractional alpha transparency from 0. to 1. */
  patt_color_rgb color;     /*!< color of basis vector */
  double linewidth;         /*!< line width of basis vector */
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
  patt_text eV;                      /*!< label for energy if used */
  patt_text title;                   /*!< label for drawing title */
  patt_text footnote;                /*!< label for drawing footnote */
  patt_text legend[MAX_INPUT_FILES]; /*!< legend for drawing */
  patt_gun gun;                      /*!< electron gun drawing details */
  patt_screen screen;                /*!< LEED screen drawing details */
  patt_color_scheme color_scheme;    /*!< color scheme for drawing */
  patt_format format;         /*!< format for output file e.g. svg or pdf */
  spots spots[2];             /*!< container for drawing style for both
                               *   GS_SPOTS (subtrate) and
                               *   SS_SPOTS (superstructure) */
  size_t energy;              /*!< energy for simulating Ewald sphere */
  bool show_overlap;          /*!< show spots on top of gun and edge of screen */
  bool show_vectors;          /*!< show basis vectors in output drawing */
  bool show_indices;          /*!< show Miller indices in output drawing */
  bool symbols;               /*!< use different symbols for each domain */
  bool fill_gs;               /*!< fill in substrate spots with spot color */
  bool fill_ss;               /*!< fill in superstructure spots */
  bool show_gs_vectors[MAX_INPUT_FILES];
  bool show_ss_vectors[MAX_INPUT_FILES][MAX_DOMAINS];
  bool show_gs_indices[MAX_INPUT_FILES];
  bool show_ss_indices[MAX_INPUT_FILES][MAX_DOMAINS];
  size_t n_files;
  char input_files[MAX_INPUT_FILES][FILENAME_MAX];
  char output_filename[FILENAME_MAX];
  bool std_in;              /*!< indicates whether to use stdin as input */
  bool std_out;             /*!< indicates whether to use stdout as output */
  bool interactive;         /*!< indicates whether to run patt interactively */
} patt_drawing;
extern patt_drawing drawing_default;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* PATT_DEF_H */
