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
