#include "patt.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <math.h>

spots_t *spots_alloc(size_t n_spots)
{
  spots_t *spots = (spots_t*) malloc(sizeof(spots_t));
  if (spots == NULL) {
    return(NULL);
  }
  if (n_spots > 0)
  {
    spots->spots = (spot_t *) malloc(sizeof(spot_t) * n_spots+1);
    if (spots->spots == NULL)
    {
      free(spots);
      return(NULL);
    }
  }
  else {
    spots->spots = NULL;
  }
  spots->n_spots = n_spots;
  spots->allocated_size = n_spots+1;
  return(spots);
}

spots_t *spots_init(size_t n_spots)
{
  spots_t *spots = spots_alloc(n_spots);
  
  spots->fill = true;
  spots->visible = false;
  spots->color = PATT_BLACK;
  spots->shape = PATT_CIRCLE;
  spots->stroke_width = RADIUS_GS/10.;
  spots->stroke_style = PATT_SOLID_STROKE;
  spots->font_size = 12.;
  spots->font_name = "Times";
  spots->radius = RADIUS_GS;
  
  return(spots);
  
}

int spots_list_realloc(spots_t *spots, size_t size)
{
  spot_t *temp = spots->spots;
  
  if ((spot_t*) realloc(spots->spots, sizeof(spot_t)*size) == NULL)
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

void spots_free(spots_t *spots)
{
  free(spots->spots);
  free(spots);
}

void spots_append(spots_t *spots, spot_t spot)
{
  if (spots->n_spots >= spots->allocated_size)
  {
    spots_list_realloc(spots, spots->allocated_size * 2);
  }
  spots->spots[spots->n_spots] = spot;
  spots->n_spots += 1;
}

void spots_set_list(spots_t *spots, spot_t *spot, size_t n_spots)
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

void spots_set_color(spots_t *spots, patt_color_rgb_t color)
{
  spots->color = color;
}

void spots_set_fill(spots_t *spots, bool fill)
{
  spots->fill = fill;
}
void spots_set_shape(spots_t *spots, patt_shape_t shape)
{
  spots->shape = shape;
}

void spots_set_stroke_width(spots_t *spots, double stroke_width)
{
  spots->stroke_width = stroke_width;
}

void spots_set_stroke_style(spots_t *spots, int stroke_style)
{
  spots->stroke_style = stroke_style;
}

void spots_set_font_size(spots_t *spots, double font_size)
{
  spots->font_size = abs(font_size);
}

void spots_set_font_name(spots_t *spots, char *font_name)
{
  strcpy(spots->font_name, font_name);
}

void spots_set_radius(spots_t *spots, double radius)
{
  spots->radius = abs(radius);
}

void spots_set_visible(spots_t *spots, bool visible)
{
  spots->visible = visible;
}

patt_color_rgb_t spots_get_color(const spots_t *spots)
{
  return (spots->color);
}

bool spots_get_fill(const spots_t *spots)
{
  return (spots->fill);
}

bool spots_get_visible(const spots_t *spots)
{
  return (spots->visible);
}

patt_shape_t spots_get_shape(const spots_t *spots)
{
  return (spots->shape);
}

patt_stroke_t spots_get_stroke_style(const spots_t *spots)
{
  return (spots->stroke_style);
}

double spots_get_stroke_width(const spots_t *spots)
{
  return (spots->stroke_width);
}

double spots_get_font_size(const spots_t *spots)
{
  return (spots->font_size);
}

const char* spots_get_font_name(const spots_t *spots)
{
  char *name = (char*)malloc(sizeof(char) * strlen(spots->font_name));
  strcpy(name, spots->font_name);
  return ((const char*)name);
}

double spots_get_radius(const spots_t *spots)
{
  return (spots->radius);
}

size_t spots_get_n_spots(const spots_t *spots)
{
  return (spots->n_spots);
}

const spot_t *spots_get_list(const spots_t *spots)
{
  const spot_t* list = spots->spots;
  return (list);
}
