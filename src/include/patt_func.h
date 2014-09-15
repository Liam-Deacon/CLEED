/* patt_func.h */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef PATT_FUNC_H
#define PATT_FUNC_H

#include "patt_def.h"
#include <stdbool.h>

/* declare function prototypes & constants */

char *fget_nocomm(char *buffer, FILE *in_stream, FILE *out_stream);

int ps_print_substitute(FILE *output, char *str);

void ps_add_vectors(FILE *out_stream);

int decode_vectors(int spot_type, int *do_vectors, char *argv, int str_length);

char *ps_color(int colour_flag, int ifile, int nfiles, int spot_type, int i_dom);

int ps_draw_finalize(FILE *out_stream, int gun_flag, float gun_thickness,
	float gun_radians, float screen_thickness, int screen_flag,
	int vectors_flag, char *vectors_str, int ifiles, int ev_flag,
	float eV, char *title_str);

int ps_draw_gun(FILE *out_stream, float gun_thickness, float gun_radius, 
	float position_radians, float edge_thickness, int screen_flag);

int ps_draw_init(FILE *out_stream, int ifiles, char *title_str,
	float *pos_title, char *footnote, float *pos_footnote, int screen_flag,
	float screen_thickness, int fill_screen, int ev_flag,
	float eV, float *pos_ev, int clip_flag, int vectors_flag);

int ps_draw_title(FILE *in_stream, FILE *out_stream, int ii, int ifiles,
		char *colour, int *i_line_offset);
	
int ps_draw_screen(FILE *out_stream, float edge_thickness, int fill);

int ps_draw_spot(FILE *out_stream, float x, float y,
	float spot_size, int shape, char *colour, char *fill);

int ps_draw_vectors(FILE *out_stream, int i_dom, int ii, char *colour, float *a1, float *a2,
		float spot_radius, char dummystr[][STRSZ], int spot, char *vectors_str);	
	
void ps_set_linewidth(FILE* file, double width);
    
int nice_frac(int *zahler, int *nenner);

int magick_ps2png(char *filename_in, char *filename_out);


/* patt specific functions */
int parse_patt_args(int argc, char *argv[], drawing_t *drawing, 
                    int *format, char *output_filename);
void patt_usage(FILE *output);
void patt_info();
                    
#endif /* PATT_FUNC_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif