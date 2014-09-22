#include <cairo.h>

cairo_surface_t *surface;
cairo_t *cr;
cairo_text_extents_t te;

surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 500, 500);
cr = cairo_create(surface);

/* setup canvas */
cairo_set_line_width(cr, 0.1);
cairo_set_source_rgb(cr, 0., 0., 0.);
cairo_select_font_face(cr, "Times", 
                       CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
cairo_set_font_size(cr, 1.2);


/* show text */
cairo_text_extents (cr, "a", &te);
cairo_move_to (cr, 0.5 - te.width / 2 - te.x_bearing,
    0.5 - te.height / 2 - te.y_bearing);
cairo_show_text(cr, "text");

/* draw path */
cairo_move_to(cr, 0.25, 0.25);


cairo_set_line_width (cr, 0.1);

cairo_save (cr);
cairo_scale (cr, 0.5, 1);
cairo_arc (cr, 0.5, 0.5, 0.40, 0, 2 * M_PI);
cairo_stroke (cr);

cairo_translate (cr, 1, 0);
cairo_arc (cr, 0.5, 0.5, 0.40, 0, 2 * M_PI);
cairo_restore (cr);
cairo_stroke (cr);

cairo_arc(cr, 0.5, 0.5, 0.4, 0, 2*M_PI);
