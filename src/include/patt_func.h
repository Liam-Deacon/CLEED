/*********************************************************************
 *                           PATT_FUNC.H
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
 * \brief File contains function prototypes for the \c patt program.
 */

#ifndef PATT_FUNC_H
#define PATT_FUNC_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "patt_def.h"
#include <stdbool.h>

int patt_main(int, char **); /* prototype for python entry into main */

#if USE_CAIRO
#include <cairo.h>
#endif

#if (!__GNUC__)
# define __attribute__(x) /* empty to disable GCC attribute extension */
#endif

/* declare function prototypes & constants */
__attribute__((nonnull, returns_nonnull))
char *fget_nocomm(char *buffer, FILE *in_stream, FILE *out_stream);

__attribute__((nonnull))
int decode_vectors(int spotype, bool *do_vectors, char *argv,
                   size_t str_length);

__attribute__((nonnull(2)))
int ps_print_substitute(FILE *output, char *str);

void ps_add_vectors(FILE *out_stream);

__attribute__((returns_nonnull))
char *ps_color(int colour_flag, int ifile, int nfiles,
               int spotype, int i_dom);

__attribute__((nonnull))
int ps_draw_finalize(FILE *out_stream, int gun_flag, float patt_gunhickness,
	float gun_radians, float screenhickness, int screen_flag,
	int vectors_flag, char *vectors_str, int ifiles, int ev_flag,
	float eV, char *title_str);

int ps_draw_gun(FILE *out_stream, float patt_gunhickness, float gun_radius, 
	float position_radians, float edge_thickness, int screen_flag);

__attribute__((nonnull))
int ps_draw_init(FILE *out_stream, int ifiles, char *title_str,
	float *pos_title, char *footnote, float *pos_footnote, int screen_flag,
	float screenhickness, int fill_screen, int ev_flag,
	float eV, float *pos_ev, int clip_flag, int vectors_flag);

__attribute__((nonnull))
int ps_draw_title(FILE *in_stream, FILE *out_stream, int ii, int ifiles,
		char *colour, int *i_line_offset);
	
int ps_draw_screen(FILE *out_stream, float edge_thickness, int fill);

int ps_draw_spot(FILE *out_stream, float x, float y,
	float spot_size, int shape, char *colour, char *fill);

int ps_draw_vectors(FILE *out_stream, int i_dom, int ii, char *colour,
                    float *a1, float *a2, float spot_radius,
                    char dummystr[][STRSZ], int spot, char *vectors_str);
	
void ps_set_linewidth(FILE* file, double width);

__attribute__((nonnull)) int nice_frac(int *numerator, int *denominator);


/* patt specific functions */
__attribute__((nonnull)) int patt_session(const patt_drawing *drawing);

__attribute__((nonnull(2)))
int patt_args(int argc, char *argv[], patt_drawing *drawing);

__attribute__((nonnull, returns_nonnull))
const char *patt_color_get_name(const patt_color_rgb *color);

__attribute__((nonnull, returns_nonnull))
const char *patt_color_get_ps_string(const patt_color_rgb *color);

__attribute__((nonnull))
bool patt_color_is_equal(const patt_color_rgb *, const patt_color_rgb *);

__attribute__((nonnull)) int patt_draw(const patt_drawing *drawing);


__attribute__((nonnull)) int patt_draw_ps(const patt_drawing *drawing);

__attribute__((nonnull))
void patt_draw_ps_finish(FILE *file_ptr, const patt_drawing *drawing);

__attribute__((nonnull(2)))
void patt_draw_ps_gun(FILE *file_ptr, const patt_gun *gun);

__attribute__((nonnull(2)))
void patt_draw_ps_init(FILE *file_ptr, const patt_drawing *drawing);

__attribute__((nonnull(2,3)))
void patt_draw_ps_label(FILE *file_ptr, const spots *group, const spot *spot);

__attribute__((nonnull(2)))
void patt_draw_ps_screen(FILE *file_ptr, const patt_screen *screen);

__attribute__((nonnull))
void patt_draw_ps_spot(FILE *file_ptr, spot *spot, double spot_size,
                      patt_shape shape, const patt_color_rgb *color,
                      bool fill);

__attribute__((nonnull(2)))
void patt_draw_ps_vectors(FILE *file_ptr, const patt_drawing *drawing);

void patt_draw_ps_vector_funcs(FILE *file_ptr);

__attribute__((nonnull, returns_nonnull))
patt_color_rgb *patt_get_named_color(const char *color);

void patt_info(void);
void patt_usage(FILE *output);


#if USE_CAIRO
__attribute__((nonnull))
int patt_draw_cairo(const patt_drawing *drawing);

__attribute__((nonnull))
void patt_draw_cairo_arrow(cairo_t *cr, double x1, double y1, double x2,
                           double y2, double stroke_width, double head_size);

__attribute__((nonnull))
void patt_draw_cairo_gun(cairo_t *cr, const patt_gun *gun);

__attribute__((nonnull))
void patt_draw_cairo_label(cairo_t *cr, const spot *spot);

__attribute__((nonnull))
void patt_draw_cairo_screen(cairo_t *cr, const patt_screen *screen);

__attribute__((nonnull))
void patt_draw_cairo_spot(cairo_t *cr, spot *spot,
                          double spot_size, patt_shape shape);

__attribute__((nonnull))
void patt_draw_cairo_text(cairo_t *cr, const patt_text *text,
                          cairo_font_slant_t slant, cairo_font_weight_t weight);

#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#ifdef __attribute__
# undef __attribute__
#endif

#endif /* PATT_FUNC_H */
