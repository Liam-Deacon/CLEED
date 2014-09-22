/****************************************************************************
                        PATT_VER.H 
                        
    version information for patt

Changes:
         
****************************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef PATT_DEF_H
#define PATT_DEF_H

#include <limits.h>
#include <stdbool.h>
#include "matrix_2x2.h"
#include "spots.h"
#include "coord.h"
#include "patt_colors.h"
#include "pattern.h"

#ifndef STRSZ
  #define STRSZ 128
#endif

/* formats */
/*! \enum patt_format_t
 *  \brief Output format of generated LEED pattern.
 */
typedef enum {
  PATT_PS_OLD,
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
  PATT_READ_ERROR,
  PATT_WRITE_ERROR,
  PATT_COLOR_ERROR,
  PATT_FORMAT_ERROR,
  PATT_ALLOC_ERROR,
  PATT_DEALLOC_ERROR,
  PATT_SHAPE_ERROR,
  PATT_UNKNOWN_ERROR
} patt_error_t;

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

/* define postscript colours */
#define PSRED "1 0 0 setrgbcolor"			    /* red */
#define PSDKRED "0.7 0.1 0 setrgbcolor"		    /* dark red */
#define PSGREEN "0 1 0 setrgbcolor"			    /* green */
#define PSBLUE "0 0 1 setrgbcolor"			    /* blue */
#define PSDKBLUE "0 0 0.5 setrgbcolor"		    /* dark blue */
#define PSLTBLUE "0 1 1 setrgbcolor"		    /* light blue */
#define PSMAGENTA "1 0 1 setrgbcolor"		    /* magenta */
#define PSYELLOW "1 1 0 setrgbcolor"		    /* yellow */
#define PSWHITE "1 1 1 setrgbcolor"			    /* white */
#define PSBLACK "0 0 0 setrgbcolor"			    /* black */
#define PSORANGE "1.0 0.7 0.0 setrgbcolor"	    /* orange */
#define PSDKORANGE "1 0.33 0 setrgbcolor"	    /* dark orange */
#define PSPURPLE "0.7 0.3 1.0 setrgbcolor"	    /* purple */
#define PSBROWN "0.7 0.3 0.0 setrgbcolor"	    /* brown */
#define PSDKGREEN "0.0 0.5 0.0 setrgbcolor"	    /* dark green */
#define PSGREY "0.5 setgray"				    /* grey */
#define PSDKGREY "0.25 setgray"				    /* dark grey */
#define PSLTGREY "0.75 setgray"				    /* light grey */
#define PSGOLD "0.83 0.83 0.17 setrgbcolor"     /* 'gold' */
#define PSDKCYAN "0 0.67 0.67 setrgbcolor"      /* dark cyan */
#define PSCYAN "0.08 0.92 0.92 setrgbcolor"     /* cyan */

#define EDGE_COLOR "0.55 1 0.55 setrgbcolor"
#define SCREEN_COLOR "0.85 1 0.85 setrgbcolor"
#define GUN_COLOR "0 0 0 setrgbcolor"

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
  char *label;
  double x;
  double y;
  double size;
  bool visible;
  patt_color_rgb_t color;
} text_t;
extern text_t text_default;

typedef struct screen_t 
{
  double radius;        /* screen radius (default is MAX_RADIUS)*/
  double stroke_width;  /* screen edge thickness */
  bool clip;            /* clip pattern to radius for screen */
  bool fill;            /* background fill of screen */
  bool visible;         /* show screen */
} screen_t;
extern screen_t screen_default;

typedef struct gun_t 
{
  double x;
  double y;
  double radius;
  double angle;      /* in radians */
  bool fill;
  bool visible;
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
} vector_t;
extern vector_t vector_default; 

typedef struct drawing_t 
{
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
  bool show_ss_vectors[MAX_INPUT_FILES][STRSZ];
  bool show_gs_indices[MAX_INPUT_FILES];
  bool show_ss_indices[MAX_INPUT_FILES][STRSZ];
  size_t n_files;
  char input_files[MAX_INPUT_FILES][PATH_MAX];
} drawing_t;
extern drawing_t drawing_default;




#endif /* PATT_DEF_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
