/*********************************************************************
 *                       PATT_DEFAULTS.C
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *
 * Provides defaults for the structs used in the \c patt program.
 */

#include "patt.h"
#include <stdbool.h>

patt_text text_default = {.x=0., .y=0., .size=12., .visible=false};

patt_gun gun_default = {.x=0., .y=0., .radius=0., .angle=0.,
                        .fill=true, .visible=false};

patt_screen screen_default = {.radius=MAX_RADIUS, .stroke_width=RADIUS_GS*0.5,
                              .clip=false, .fill=true, .visible=false};

patt_vector vector_default = {.x1=0., .y1=0., .color={0, 0, 0},
                              .linewidth=1., .head_size=5.};

spot spot_default = {.index = {.h=0., .k=0.}, .x=0., .y=0.};

spots spots_default = {.radius=RADIUS_GS, .stroke_style=0, .stroke_width=1.,
                         .fill=true, .visible=true,
                         .shape=PATT_CIRCLE,
                         .color={0, 0, 0},
                        };
                  
patt_drawing drawing_default = {
        .title = {.x = -(MAX_RADIUS), .y = (MAX_RADIUS+150.), .size = 26., },
        .footnote = {.x = -(MAX_RADIUS), .y = -(MAX_RADIUS+50.), .size = 10.,},
        .color_scheme=PATT_COLOR_SCHEME,
        .show_overlap=false,
        .show_vectors=false,
        .show_indices=true,
        .symbols=true,
        .energy=0.,
        .fill_gs=true,
        .fill_ss=true};
        
matrix_2x2 matrix_2x2_default = {0., 0., 0., 0.};

pattern pattern_default = {
    .a1={0., 0., 0.},
    .a2={0., 0., 0.},
    .n_domains=0,
    .radius=1.};

