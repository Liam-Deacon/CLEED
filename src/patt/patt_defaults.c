#include "patt.h"

text_t text_default = {.x=0., .y=0., .size=12., .visible=0};

gun_t gun_default = {.x=0., .y=0., .radius=0., .angle=0., .fill=1, .visible=0};

screen_t screen_default = {.radius=MAX_RADIUS, .edge_width=RADIUS_GS*0.5, 
                            .clip=0, .fill=1, .visible=0};

vector_t vector_default = {.x1=0., .y1=0., .color=0, .linewidth=1., .head_size=5.};

spot_t spot_default = {.h=0., .k=0., .x=0., .y=0., .radius=RADIUS_GS, 
                       .stroke_style=0, .stroke_width=1., 
                       .fill=1., .visible=1, .color=0, .shape=PATT_CIRCLE};
                  
drawing_t drawing_default = {
                            .eV = {.x = (MAX_RADIUS*1.)*0.7071067811865476, 
                                   .y = -(MAX_RADIUS*1.2)*0.7071067811865476,
                                   .size = 20.
                                  },
                            .title = {.x = -(MAX_RADIUS), 
                                      .y = (MAX_RADIUS+150.), 
                                      .size = 26.
                                     },
        .footnote = {.x = -(MAX_RADIUS), .y = -(MAX_RADIUS+50.), .size = 10.},
        .color = PATT_COLOR,
        .show_overlap=0,
        .show_vectors=0,
        .show_indices=1,
        .symbols=1,
        .energy=0,
        .fill_gs=1.,
        .fill_ss=1.};
        
matrix_2x2_t matrix_2x2_default = {0., 0., 0., 0.};

vector_xy_t vector_xy_default = {0., 0.};

pattern_t pattern_default = {.a1={0., 0.}, .a2={0., 0.}, .n_domains=0, .radius=1.};

