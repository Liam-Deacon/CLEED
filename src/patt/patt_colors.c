#include "patt_colors.h"
#include "strings.h"

const char colors[NUM_COLORS][2][30] = {
      {"black", "dark gray"}, {"dark red", "red"},
      {"blue", "light blue"}, {"dark green", "green"},
      {"dark orange", "orange"}, {"gold", "yellow"}
};
  
const patt_color_rgb_t grays[NUM_GRAYS] = {
    {0.1, 0.1, 0.1}, {0.2, 0.2, 0.2}, {0.3, 0.3, 0.3},
    {0.4, 0.4, 0.4}, {0.5, 0.5, 0.5}, {0.6, 0.6, 0.6},
    {0.7, 0.7, 0.7}, {0.8, 0.8, 0.8}, {0.9, 0.9, 0.9}
};

patt_color_rgb_t *patt_get_named_color(char *color)
{
  patt_color_rgb_t *patt_color;
  if (strcasecmp(color, "red") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_RED;
  }
  else if (strcasecmp(color, "blue") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_BLUE;
  }
  else if (strcasecmp(color, "green") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_RED;
  }
  else if (strcasecmp(color, "magenta") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_MAGENTA;
  }
  else if (strcasecmp(color, "light blue") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_LIGHT_BLUE;
  }
  else if (strcasecmp(color, "orange") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_ORANGE;
  }
  else if (strcasecmp(color, "cyan") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_CYAN;
  }
  else if (strcasecmp(color, "yellow") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_YELLOW;
  }
  else if (strcasecmp(color, "purple") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_PURPLE;
  }
  else if (strcasecmp(color, "gray") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_GRAY;
  }
  else if (strcasecmp(color, "dark cyan") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_DARK_CYAN;
  }
  else if (strcasecmp(color, "dark gray") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_DARK_GRAY;
  }
  else if (strcasecmp(color, "dark green") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_DARK_GREEN;
  }
  else if (strcasecmp(color, "dark orange") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_DARK_ORANGE;
  }
  else if (strcasecmp(color, "dark red") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_DARK_RED;
  }
  else if (strcasecmp(color, "gold") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_GOLD;
  }
  else if (strcasecmp(color, "brown") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_BROWN;
  }
  else if (strcasecmp(color, "white") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_WHITE;
  }
  else if (strcasecmp(color, "black") == 0) {
    patt_color = (patt_color_rgb_t*) &PATT_BLACK;
  }
  else {
    patt_color = (patt_color_rgb_t*) &PATT_BLACK;
  }
  return patt_color;
}
