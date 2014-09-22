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

extern const patt_color_rgb_t PATT_BLACK;
extern const patt_color_rgb_t PATT_RED;
extern const patt_color_rgb_t PATT_BLUE;
extern const patt_color_rgb_t PATT_GREEN;
extern const patt_color_rgb_t PATT_MAGENTA;
extern const patt_color_rgb_t PATT_LIGHT_BLUE;
extern const patt_color_rgb_t PATT_ORANGE;
extern const patt_color_rgb_t PATT_CYAN;
extern const patt_color_rgb_t PATT_YELLOW;
extern const patt_color_rgb_t PATT_PURPLE;
extern const patt_color_rgb_t PATT_GRAY;
extern const patt_color_rgb_t PATT_DARK_RED;
extern const patt_color_rgb_t PATT_DARK_CYAN;
extern const patt_color_rgb_t PATT_DARK_GRAY;
extern const patt_color_rgb_t PATT_DARK_GREEN;
extern const patt_color_rgb_t PATT_DARK_ORANGE;
extern const patt_color_rgb_t PATT_GOLD;
extern const patt_color_rgb_t PATT_BROWN;
extern const patt_color_rgb_t PATT_WHITE;

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
