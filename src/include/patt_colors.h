/*********************************************************************
 *                           PATT_COLORS.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.08.01 - creation
 *********************************************************************/

/*!
 * \file
 * \brief Header for colors used in \c pattern. File contains
 *  - \c typedef \c struct #patt_color_rgb
 *  - \c extern color constants
 */

#ifndef PATT_COLORS_H
#define PATT_COLORS_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


#define NUM_COLORS 6    /*!< number of colors to cycle through */
#define NUM_GRAYS  9    /*!< number of grays to cycle through */

/*! \typedef patt_color_rgb
 *  \brief RGB color structure.
 */
typedef struct
{
  double red;           /*!< fractional amount of red color */
  double green;         /*!< fractional amount of green color */
  double blue;          /*!< fractional amount of blue color */
} patt_color_rgb;

extern const char colors[NUM_COLORS][2][30];
extern const patt_color_rgb grays[NUM_GRAYS];

/* named colors */
extern const patt_color_rgb PATT_BLACK;
extern const patt_color_rgb PATT_RED;
extern const patt_color_rgb PATT_BLUE;
extern const patt_color_rgb PATT_GREEN;
extern const patt_color_rgb PATT_MAGENTA;
extern const patt_color_rgb PATT_LIGHT_BLUE;
extern const patt_color_rgb PATT_ORANGE;
extern const patt_color_rgb PATT_CYAN;
extern const patt_color_rgb PATT_YELLOW;
extern const patt_color_rgb PATT_PURPLE;
extern const patt_color_rgb PATT_GRAY;
extern const patt_color_rgb PATT_DARK_RED;
extern const patt_color_rgb PATT_DARK_CYAN;
extern const patt_color_rgb PATT_DARK_GRAY;
extern const patt_color_rgb PATT_DARK_GREEN;
extern const patt_color_rgb PATT_DARK_ORANGE;
extern const patt_color_rgb PATT_GOLD;
extern const patt_color_rgb PATT_BROWN;
extern const patt_color_rgb PATT_WHITE;

/* special colors */
extern const patt_color_rgb PATT_EDGE_COLOR;
extern const patt_color_rgb PATT_SCREEN_COLOR;
extern const patt_color_rgb PATT_GUN_COLOR;


/*!
 *  \brief Retrieve a color from a given enum value.
 *  \param color The color from an enum value.
 *  \return patt_color_rgb* Pointer to color struct.
 */
patt_color_rgb *patt_get_enum_color(int color);

/*!
 *  \brief retrieve a color from a given color name string.
 *  \param *color the color name string.
 *  \return patt_color_rgb* pointer to color struct.
 */
patt_color_rgb *patt_color_from_name(const char *color);


/*!
 * \brief copies #patt_colo_rgb color data from \p src to \p dst .
 * \param dst Pointer to copy color information into.
 * \param src Pointer to copy color information from.
 */
void patt_color_copy(patt_color_rgb *dst, const patt_color_rgb *src);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C"  */
#endif

#endif
