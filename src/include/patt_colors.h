#ifndef PATT_COLORS_H
#define PATT_COLORS_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif


#define NUM_COLORS 6
#define NUM_GRAYS 9

/*! \struct patt_color_rgb_t
 *  \brief RGB color structure.
 *
 *
 *
 *
 *
 */
typedef struct patt_color_rgb_t
{
  double red;           /*!< fractional amount of red color */
  double green;         /*!< fractional amount of green color */
  double blue;          /*!< fractional amount of blue color */
} patt_color_rgb_t;

extern const char colors[NUM_COLORS][2][30];
extern const patt_color_rgb_t grays[NUM_GRAYS];

const patt_color_rgb_t PATT_BLACK = {0, 0, 0};         
const patt_color_rgb_t PATT_RED = {1, 0, 0};
const patt_color_rgb_t PATT_BLUE = {0, 1, 0}; 
const patt_color_rgb_t PATT_GREEN = {0, 1, 0}; 
const patt_color_rgb_t PATT_MAGENTA = {1, 0, 1};
const patt_color_rgb_t PATT_LIGHT_BLUE = {0, 1, 1}; 
const patt_color_rgb_t PATT_ORANGE = {1, 0.7, 0};
const patt_color_rgb_t PATT_CYAN = {0.08, 0.92, 0.92};
const patt_color_rgb_t PATT_YELLOW = {1, 1, 0};
const patt_color_rgb_t PATT_PURPLE = {0.7, 0.3, 1};
const patt_color_rgb_t PATT_GRAY = {0.5, 0.5, 0.5};
const patt_color_rgb_t PATT_DARK_RED = {0.7, 0.1, 0};
const patt_color_rgb_t PATT_DARK_CYAN = {0, 0.67, 0.67};
const patt_color_rgb_t PATT_DARK_GRAY = {0.25, 0.25, 0.25};
const patt_color_rgb_t PATT_DARK_GREEN = {0, 0.5, 0};
const patt_color_rgb_t PATT_DARK_ORANGE = {1, 0.7, 0};
const patt_color_rgb_t PATT_GOLD = {0.83, 0.83, 0.17};
const patt_color_rgb_t PATT_BROWN = {0.7, 0.3, 0};
const patt_color_rgb_t PATT_WHITE = {1, 1, 1};

/*! \fn patt_color_rgb_t *patt_get_enum_color(int color);
 *  \brief Retrieve a color from a given enum value.
 *  \param color The color from an enum value.
 *  \return patt_color_rgb_t* Pointer to color struct.
 */
patt_color_rgb_t *patt_get_enum_color(int color);

/*! \fn patt_color_rgb_t *patt_get_named_color(char *color)
 *  \brief retrieve a color from a given color name string.
 *  \param *color the color name string.
 *  \return patt_color_rgb_t* pointer to color struct.
 */
patt_color_rgb_t *patt_get_named_color(char *color);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C"  */
#endif

#endif