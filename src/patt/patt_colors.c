/*********************************************************************
 *                       PATT_COLORS.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *  \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Provides color constants and functions for the \c patt program.
 */

#if _MSC_VER
#define strcasecmp _stricmp
#include <string.h>
#else
#include <strings.h>
#endif

#include "patt.h"
#include "patt_colors.h"
#include <stdbool.h>
#include <malloc.h>

/*!
 * Array of named colors to cycle between
 */
const char colors[NUM_COLORS][2][30] = {
      {"black", "dark gray"}, {"dark red", "red"},
      {"blue", "light blue"}, {"dark green", "green"},
      {"dark orange", "orange"}, {"gold", "yellow"}
};

/*!
 * Array of #patt_color_rgb grayscale colors to cycle through.
 */
const patt_color_rgb grays[NUM_GRAYS] = {
    {0.1, 0.1, 0.1}, {0.2, 0.2, 0.2}, {0.3, 0.3, 0.3},
    {0.4, 0.4, 0.4}, {0.5, 0.5, 0.5}, {0.6, 0.6, 0.6},
    {0.7, 0.7, 0.7}, {0.8, 0.8, 0.8}, {0.9, 0.9, 0.9}
};

const patt_color_rgb PATT_BLACK = {0, 0, 0};
const patt_color_rgb PATT_RED = {1, 0, 0};
const patt_color_rgb PATT_BLUE = {0, 1, 0};
const patt_color_rgb PATT_GREEN = {0, 1, 0};
const patt_color_rgb PATT_MAGENTA = {1, 0, 1};
const patt_color_rgb PATT_LIGHT_BLUE = {0, 1, 1};
const patt_color_rgb PATT_ORANGE = {1, 0.7, 0};
const patt_color_rgb PATT_CYAN = {0.08, 0.92, 0.92};
const patt_color_rgb PATT_YELLOW = {1, 1, 0};
const patt_color_rgb PATT_PURPLE = {0.7, 0.3, 1};
const patt_color_rgb PATT_GRAY = {0.5, 0.5, 0.5};
const patt_color_rgb PATT_DARK_RED = {0.7, 0.1, 0};
const patt_color_rgb PATT_DARK_CYAN = {0, 0.67, 0.67};
const patt_color_rgb PATT_DARK_GRAY = {0.25, 0.25, 0.25};
const patt_color_rgb PATT_DARK_GREEN = {0, 0.5, 0};
const patt_color_rgb PATT_DARK_ORANGE = {1, 0.7, 0};
const patt_color_rgb PATT_GOLD = {0.83, 0.83, 0.17};
const patt_color_rgb PATT_BROWN = {0.7, 0.3, 0};
const patt_color_rgb PATT_WHITE = {1, 1, 1};

const patt_color_rgb PATT_EDGE_COLOR = {0.55, 1., 0.55};
const patt_color_rgb PATT_SCREEN_COLOR = {0.85, 1., 0.85};
const patt_color_rgb PATT_GUN_COLOR = {0., 0., 0.};

/*!
 * Copies the RGB color values of \p src into \p dst
 *
 * \param[in] dst Pointer to the source color.
 * \param[out] src Pointer to the destination color.
 * \note if \p dst is \c NULL upon entering the function then the pointer to \p
 * src is returned.
 */
void patt_color_copy(patt_color_rgb *dst, const patt_color_rgb *src)
{
  if (dst == NULL) dst = (patt_color_rgb *) malloc(sizeof(patt_color_rgb));
  if (dst == NULL) return;

  dst->red = src->red;
  dst->blue = src->blue;
  dst->green = src->green;
}

/*!
 * Tests whether the two #patt_color_rgb instances match
 *
 * \param dst
 * \param src
 * \return
 */
bool patt_color_is_equal(const patt_color_rgb *col1,
                         const patt_color_rgb *col2)
{
  if ((col1->red == col2->red) &&
      (col1->blue == col2->blue) &&
      (col1->green == col2->green)) return(true);

  return(false);
}

/*!
 * Returns a pointer to a #patt_color_rgb color which matches the \p color name.
 *
 * \param color String name of the color.
 * \return Pointer to a #patt_color_rgb object.
 * \retval \c NULL will be returned if \p color is unknown.
 */
patt_color_rgb *patt_color_from_name(const char *color)
{
  patt_color_rgb *patt_color;
  if (strcasecmp(color, "red") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_RED;
  }
  else if (strcasecmp(color, "blue") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_BLUE;
  }
  else if (strcasecmp(color, "green") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_GREEN;
  }
  else if (strcasecmp(color, "magenta") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_MAGENTA;
  }
  else if (strcasecmp(color, "light blue") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_LIGHT_BLUE;
  }
  else if (strcasecmp(color, "orange") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_ORANGE;
  }
  else if (strcasecmp(color, "cyan") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_CYAN;
  }
  else if (strcasecmp(color, "yellow") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_YELLOW;
  }
  else if (strcasecmp(color, "purple") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_PURPLE;
  }
  else if (strcasecmp(color, "gray") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_GRAY;
  }
  else if (strcasecmp(color, "dark cyan") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_DARK_CYAN;
  }
  else if (strcasecmp(color, "dark gray") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_DARK_GRAY;
  }
  else if (strcasecmp(color, "dark green") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_DARK_GREEN;
  }
  else if (strcasecmp(color, "dark orange") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_DARK_ORANGE;
  }
  else if (strcasecmp(color, "dark red") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_DARK_RED;
  }
  else if (strcasecmp(color, "gold") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_GOLD;
  }
  else if (strcasecmp(color, "brown") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_BROWN;
  }
  else if (strcasecmp(color, "white") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_WHITE;
  }
  else if (strcasecmp(color, "black") == 0)
  {
    patt_color = (patt_color_rgb*) &PATT_BLACK;
  }
  else
  {
    patt_color = NULL;
  }
  return(patt_color);
}

/*!
 * Returns a string name of the #patt_color_rgb instance \p color
 *
 * \param color Pointer to #patt_color_rgb instance in which to query.
 * \return A name string of the color.
 * \note If the color is unknown then the function defaults to 'black'
 */
const char *patt_color_get_name(const patt_color_rgb *color)
{
  if (patt_color_is_equal(color, &PATT_RED))
  {
    return("red");
  }
  else if (patt_color_is_equal(color, &PATT_BLUE))
  {
    return("blue");
  }
  else if (patt_color_is_equal(color, &PATT_GREEN))
  {
    return("green");
  }
  else if (patt_color_is_equal(color, &PATT_MAGENTA))
  {
    return("magenta");
  }
  else if (patt_color_is_equal(color, &PATT_LIGHT_BLUE))
  {
    return("light blue");
  }
  else if (patt_color_is_equal(color, &PATT_ORANGE))
  {
    return("orange");
  }
  else if (patt_color_is_equal(color, &PATT_CYAN))
  {
    return("cyan");
  }
  else if (patt_color_is_equal(color, &PATT_YELLOW))
  {
    return("yellow");
  }
  else if (patt_color_is_equal(color, &PATT_PURPLE))
  {
    return("purple");
  }
  else if (patt_color_is_equal(color, &PATT_GRAY))
  {
    return("gray");
  }
  else if (patt_color_is_equal(color, &PATT_DARK_CYAN))
  {
    return("dark cyan");
  }
  else if (patt_color_is_equal(color, &PATT_DARK_GRAY))
  {
    return("dark gray");
  }
  else if (patt_color_is_equal(color, &PATT_DARK_GREEN))
  {
    return("dark green");
  }
  else if (patt_color_is_equal(color, &PATT_DARK_ORANGE))
  {
    return("dark orange");
  }
  else if (patt_color_is_equal(color, &PATT_DARK_RED))
  {
    return("dark red");
  }
  else if (patt_color_is_equal(color, &PATT_GOLD))
  {
    return("gold");
  }
  else if (patt_color_is_equal(color, &PATT_BROWN))
  {
    return("brown");
  }
  else if (patt_color_is_equal(color, &PATT_WHITE))
  {
    return("white");
  }
  else if (patt_color_is_equal(color, &PATT_BLACK))
  {
    return("black");
  }

  return("black");
}

/*!
 * Converts the #patt_color_rgb instance \p color into postscript form.
 *
 * \param color #patt_color_rgb instance to convert to postscript.
 * \return pointer to color name string.
 */
const char *patt_color_get_ps_string(const patt_color_rgb *color)
{
  char *str = (char*) malloc(sizeof(char)* 1024);
  if (str)
    sprintf(str, "%.6e %.6e %.6e setrgbcolor",
            color->red, color->blue, color->green);

  return(str);
}
