#include "patt.h"
#include "spots.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <math.h>

spots *spots_alloc(size_t n_spots)
{
  spots *_spots = (spots*) malloc(sizeof(spots));
  if (_spots == NULL) {
    return(NULL);
  }
  if (n_spots > 0)
  {
    _spots->spots = (spot *) malloc(sizeof(spot) * n_spots+1);
    if (_spots->spots == NULL)
    {
      free(_spots);
      return(NULL);
    }
  }
  else {
    _spots->spots = NULL;
  }
  _spots->n_spots = n_spots;
  _spots->allocated_size = n_spots+1;
  return(_spots);
}

spots *spots_init(size_t n_spots)
{
  spots *_spots = spots_alloc(n_spots);
  
  _spots->fill = true;
  _spots->visible = false;
  _spots->color = PATT_BLACK;
  _spots->shape = PATT_CIRCLE;
  _spots->stroke_width = RADIUS_GS/10.;
  _spots->stroke_style = PATT_SOLID_STROKE;
  _spots->font_size = 12.;
  _spots->font_name = "Times";
  _spots->radius = RADIUS_GS;
  
  return(_spots);
  
}

int spots_list_realloc(spots *spots, size_t size)
{
  spot *temp = spots->spots;
  
  if ((spot*) realloc(spots->spots, sizeof(spot)*size) == NULL)
  {
    spots->spots = temp;
    return(-3);
  }
  
  if (size < spots->n_spots)
  {
    spots->n_spots = size;
  }

  spots->allocated_size = size;
    
  return(0);
}

void spots_free(spots *spots)
{
  free(spots->spots);
  free(spots);
}

void spots_append(spots *spots, spot spot)
{
  if (spots->n_spots >= spots->allocated_size)
  {
    spots_list_realloc(spots, spots->allocated_size * 2);
  }
  spots->spots[spots->n_spots] = spot;
  spots->n_spots += 1;
}

void spots_set_list(spots *spots, spot *spot, size_t n_spots)
{
  while (spots->n_spots > spots->allocated_size)
  {
    spots_list_realloc(spots, spots->allocated_size * 2);
  }
  
  if (spot == NULL)
    n_spots = 0;
    
   spots->spots = spot;
   spots->n_spots = n_spots;

}

void spots_set_color(spots *spots, patt_color_rgb color)
{
  spots->color = color;
}

void spots_set_fill(spots *spots, bool fill)
{
  spots->fill = fill;
}
void spots_set_shape(spots *spots, patt_shape shape)
{
  spots->shape = shape;
}

void spots_set_stroke_width(spots *spots, double stroke_width)
{
  spots->stroke_width = stroke_width;
}

void spots_set_stroke_style(spots *spots, int stroke_style)
{
  spots->stroke_style = stroke_style;
}

void spots_set_font_size(spots *spots, double font_size)
{
  spots->font_size = abs(font_size);
}

void spots_set_font_name(spots *spots, char *font_name)
{
  strcpy(spots->font_name, font_name);
}

void spots_set_radius(spots *spots, double radius)
{
  spots->radius = abs(radius);
}

void spots_set_visible(spots *spots, bool visible)
{
  spots->visible = visible;
}

patt_color_rgb spots_get_color(const spots *spots)
{
  return (spots->color);
}

bool spots_get_fill(const spots *spots)
{
  return (spots->fill);
}

bool spots_get_visible(const spots *spots)
{
  return (spots->visible);
}

patt_shape spots_get_shape(const spots *spots)
{
  return (spots->shape);
}

patt_stroke spots_get_stroke_style(const spots *spots)
{
  return (spots->stroke_style);
}

double spots_get_stroke_width(const spots *spots)
{
  return (spots->stroke_width);
}

double spots_get_font_size(const spots *spots)
{
  return (spots->font_size);
}

const char* spots_get_font_name(const spots *spots)
{
  char *name = (char*)malloc(sizeof(char) * strlen(spots->font_name));
  strcpy(name, spots->font_name);
  return ((const char*)name);
}

double spots_get_radius(const spots *spots)
{
  return (spots->radius);
}

size_t spots_get_n_spots(const spots *spots)
{
  return (spots->n_spots);
}

const spot *spots_get_list(const spots *spots)
{
  const spot* list = spots->spots;
  return (list);
}
