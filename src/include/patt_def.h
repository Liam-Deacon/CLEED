/****************************************************************************
                        PATT_VER.H 
                        
    version information for patt

Changes:
         
****************************************************************************/

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
  #define STRSZ 128
#endif

/* formats */
/*! \enum patt_format_t
 *  \brief Output format of generated LEED pattern.
 */
typedef enum {
  PATT_PS_OLD,
  PATT_EPS,
  PATT_PS,
  PATT_PNG,
  PATT_PDF,
  PATT_SVG,
  PATT_UNKNOWN_FORMAT
} patt_format_t;

/*! \enum patt_color_scheme_t
 *  \brief Color scheme for LEED pattern.
 */
typedef enum {
  PATT_BLACK_SCHEME,
  PATT_GRAYSCALE_SCHEME,
  PATT_COLOR_SCHEME,
  PATT_UNKNOWN_SCHEME
} patt_color_scheme_t;

typedef enum {
  PATT_SUCCESS=0,
  PATT_ARGUMENT_ERROR,
  PATT_READ_ERROR,
  PATT_WRITE_ERROR,
  PATT_COLOR_ERROR,
  PATT_FORMAT_ERROR,
  PATT_ALLOC_ERROR,
  PATT_DEALLOC_ERROR,
  PATT_SHAPE_ERROR,
  PATT_UNKNOWN_ERROR
} patt_error_t;


#define PATT_PAGE_HEIGHT 800.
#define PATT_PAGE_WIDTH 600.

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
 *  \brief Default linewidth for drawing 
 */
#define LINE_WIDTH 2.
#define MAX_RADIUS 200.

#define MAX_INPUT_FILES 6

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

typedef struct text_t 
{
  patt_color_rgb_t color;
  double alpha;
  char label[BUFSIZ];
  double x;
  double y;
  double size;
  bool visible;
} text_t;
extern text_t text_default;

typedef struct screen_t 
{
  double alpha;             /* % alpha transparency of screen (if supported) */
  patt_color_rgb_t stroke_color;
  patt_color_rgb_t fill_color;
  double radius;            /* screen radius (default is MAX_RADIUS)*/
  double stroke_width;      /* screen edge thickness */
  bool clip;                /* clip pattern to radius for screen */
  bool fill;                /* background fill of screen */
  bool visible;             /* show screen */
} screen_t;
extern screen_t screen_default;

typedef struct gun_t 
{
  double x;
  double y;
  double stroke_width;
  patt_color_rgb_t color;
  double radius;
  double angle;      /* in radians */
  bool fill;
  bool visible;
  double alpha;
} gun_t;
extern gun_t gun_default; 

typedef struct vector_t
{
  double x1;
  double y1;
  double x2;
  double y2;
  patt_color_rgb_t color;
  double linewidth;
  double head_size;
  double alpha;
} vector_t;
extern vector_t vector_default; 

typedef struct drawing_t 
{
  basis_t basis_gs[MAX_INPUT_FILES];
  basis_t basis_ss[MAX_INPUT_FILES][MAX_DOMAINS];
  text_t eV;
  text_t title;
  text_t footnote;
  text_t legend[MAX_INPUT_FILES];
  gun_t gun;
  screen_t screen;
  patt_color_scheme_t color_scheme;
  patt_format_t format;
  spots_t spots[2];
  int energy;
  bool show_overlap;
  bool show_vectors;
  bool show_indices;             /* show indices in output */
  bool symbols;                  /* use different superstructures symbols */
  bool fill_gs;
  bool fill_ss;
  bool show_gs_vectors[MAX_INPUT_FILES];
  bool show_ss_vectors[MAX_INPUT_FILES][MAX_DOMAINS];
  bool show_gs_indices[MAX_INPUT_FILES];
  bool show_ss_indices[MAX_INPUT_FILES][MAX_DOMAINS];
  size_t n_files;
  char input_files[MAX_INPUT_FILES][PATH_MAX];
  char output_filename[PATH_MAX];
  bool std_in;
  bool std_out;
  bool interactive;
} drawing_t;
extern drawing_t drawing_default;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* PATT_DEF_H */


