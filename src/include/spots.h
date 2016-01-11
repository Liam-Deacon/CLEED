/*********************************************************************
 *                       SPOTS.H
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *********************************************************************/

/*!
 * \file
 * \brief header for #spot \c struct and #spots \c struct.
 */

#ifndef SPOTS_H
#define SPOTS_H

#include "miller_index.h"
#include <stdbool.h>
#include "patt_colors.h"

enum {
  SPOT_GS=0,   /*!< Indexing of substrate spots. */
  SPOT_SS      /*!< Indexing of superstructure spots. */
};

#define RADIUS_GS 10.   /*!< Default radius of substrate spots */
#define RADIUS_SS  5.   /*!< Default radius of superstructure spots */


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
#include <string>
#include <list>
  extern "C" {
#endif

/* shapes */
/*! \enum patt_shape 
 */
typedef enum {
  PATT_CIRCLE,          /*!< circle shape */
  PATT_TRIANGLE_UP,     /*!< up triangle shape */
  PATT_TRIANGLE_DOWN,   /*!< down triangle shape */
  PATT_SQUARE,          /*!< square shape */
  PATT_DIAMOND,         /*!< diamond shape */
  PATT_HEXAGON,         /*!< hexagon shape */
  PATT_SHAPE_UNKNOWN    /*!< unknown shape */
} patt_shape;

/* stroke styles */
/*! \enum patt_stroke
 */
typedef enum {
  PATT_SOLID_STROKE,    /*!< solid stroke */
  PATT_DASHED_STROKE,   /*!< dashed stroke */
  PATT_DOTTED_STROKE,   /*!< dotted stroke */
  PATT_DASHDOT_STROKE,  /*!< dashdot stroke */
  PATT_NO_STROKE        /*!< no stroke */
} patt_stroke;

/*! \struct spot
 *  \brief LEED spot structure.
 */
typedef struct spot
{
  char* label;          /*!< spot label */
  miller_hkl index;   /*!< spot index */
  double x;             /*!< x position of spot */
  double y;             /*!< y position of spot */
} spot;

/*! \typedef spots
 *  \brief LEED spot list structure.
 *
 * List structure for LEED spots. Contains members to control drawing 
 * of spots group.
 */
typedef struct _spots
{
  bool fill;                    /*!< flag to determine if spots are filled */
  bool visible;                 /*!< flag to determine if spots are shown */
  bool indexing;                /*!< flag to determine is labels are shown */
  patt_color_rgb color;         /*!< color of drawn spots */
  patt_shape shape;             /*!< shape of drawn spots */
  double stroke_width;          /*!< stroke width of drawn spots */
  patt_stroke stroke_style;     /*!< stroke type of drawn spots */
  double font_size;             /*!< font size of labels */
  char *font_name;              /*!< font face for label text */
  double radius;                /*!< radius of spots */
  int flag;                     /*!< extra switch */
  spot *spots;                  /*!< array of spots */
  size_t n_spots;               /*!< maximum number of spots in array */
  size_t allocated_size;        /*!< allocated number of spots for array */
} spots;

extern spot spot_default; 

/* spots functions */
/*!
 *  \brief Allocate memory for spots. 
 *  \param n_spots Size of array to allocate to memory.
 *  \return \sa spots Pointer to #spots structure.
 *
 * Allocate memory for array of spots of size \a n_spots.
 */
spots *spots_alloc(size_t n_spots);

/*!
 *  \brief Initialise a \a spots structure. 
 *  \param n_spots Size of array to allocate to memory.
 *  \return \sa spots* Pointer to spots structure.
 *
 * Allocate memory for array of spots of size \a n_spots.
 * Set structure members to default valves.
 */
spots *spots_init(size_t n_spots);

/*!
 *  \brief Free memory of a \a spots structure. 
 *  \param *spots Pointer to spots object to free from memory.
 *
 * Free memory of a \a spots structure.
 */
void spots_free(spots *spots);

/*! \fn int spots_list_realloc(spots *spots, size_t size)
 *  \brief Resize a \a spots structure. 
 *  \param *spots Pointer to spots structure. 
 *  \param size Size of array to allocate to memory.
 *  \return success of realloc.
 *
 */
int spots_list_realloc(spots *spots, size_t size);

/*!
 *  \brief Add a \a spot to the \a spots list.
 *  \param *spots Pointer to spots structure. 
 *  \param spot spot struct.
 *
 */
void spots_append(spots *_spots, const spot _spot);

/*!
 *  \brief Assign a \a spot array to the \a spots struct.
 */
void spots_set_list(spots *spots, spot *spot, size_t n_spots);

/*!
 * \brief Set \p color of the \p spots struct.
 * \param *spots Pointer to spots structure.
 * \param *color A patt_color_rgb struct.
 *
 */
void spots_set_color(spots *spots, patt_color_rgb color);

/*!
 * \brief Set color of the \a spots struct using individual RGB channels.
 * \param *spots Pointer to spots structure.
 * \param red Fraction of red color.
 * \param green Fraction of green color.
 * \param blue Fraction of blue color.
 *
 */
void spots_set_color_rgb(spots *spots, 
                         double red, double green, double blue);

void spots_set_fill(spots *spots, bool fill);
void spots_set_shape(spots *spots, patt_shape shape);
void spots_set_stroke_width(spots *spots, double stroke_width);
void spots_set_stroke_style(spots *spots, int stroke_style);
void spots_set_font_size(spots *spots, double font_size);
void spots_set_font_name(spots *spots, char *font_name);
void spots_set_radius(spots *spots, double radius);
void spots_set_visible(spots *spots, bool visible);
void spots_set_indexing(spots *spots, bool indexing);

patt_color_rgb spots_get_color(const spots *spots);
patt_shape spots_get_shape(const spots *spots);
patt_stroke spots_get_stroke_style(const spots *spots);
bool spots_get_fill(const spots *spots); 
bool spots_get_visible(const spots *spots);
double spots_get_stroke_width(const spots *spots);
double spots_get_font_size(const spots *spots);
const char* spots_get_font_name(const spots *spots);  
double spots_get_radius(const spots *spots);
size_t spots_get_n_spots(const spots *spots);
const spot *spots_get_list(const spots *spots);
bool spots_show_indexing(const spots *spots);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

/*! \class Spot
 *  \brief LEED spot class
 */
class Spot {
  public:
    /*! \brief Default Spot class constructor. */ 
    Spot();
    /*! \brief Spot class destructor. */ 
    ~Spot();

    /* setter functions */    
    /*! \brief Miller index h setter function. 
     *  \sa setH()
     *  \param h Value of Miller index h.
     */ 
    void setH(double h);

    /*! \brief Miller index k setter function. 
     *  \sa setK()
     *  \param k Value of Miller index k.
     */ 
    void setK(double k);

    /*! \brief Miller index l setter function. 
     *  \sa setK()
     *  \param k Value of Miller index .
     */ 
    void setL(double l);
    
    /*! \brief Setter function for position of Spot. 
     *  \sa setPoint()
     *  \param x Value of x position.
     *  \param y Value of y position.
     */ 
    void setPoint(double x, double y);

    /* getter functions */
    /*! \brief Getter function for Miller h index of of Spot. 
     *  \sa getH()
     *  \return Value of Miller h index.
     */ 
    const double getH();
    
    /*! \brief Getter function for Miller k index of of Spot. 
     *  \sa getK()
     *  \return Value of Miller k index.
     */ 
    const double getK();

	/*! \brief Getter function for Miller l index of of Spot.
	*  \sa getL()
	*  \return Value of Miller l index.
	*/
	const double getL();

    /*! \brief Getter function for x position of Spot. 
     *  \sa getPointX()
     *  \return Value of x position.
     */ 
    const double getPointX();
    
    /*! \brief Getter function for y position of Spot. 
     *  \sa getPointY()
     *  \return Value of y position.
     */ 
    const double getPointY();
    
  protected:
    /*!
     * Wrapped spot C structure. 
     */
    spot spot;  
};

/*! \class
 *  \brief
 */
class Spots {
  public:
    Spots();
    ~Spots();
    
    /* setters */
    void setColor(int color);           
    void setFill(bool fill);
    void setShape(int shape);
    void setStrokeWidth(double width);
    void setStrokeStyle(int style);
    void setFontSize(double size);
    void setFontName(std::string name);
    void setRadius(double radius);
    void setVisible(bool visible);
    void setIndexing(bool indexing);
    void setSpotList(std::list<Spot> spots);
    
    /* getters */
    const bool isFilled();
    const bool isVisible();
    const bool isIndexed();
    const int getColor();
    const int getShape();
    const double getStrokeWidth();
    const int getStrokeStyle();
    const double getFontSize();
    const std::string getFontName();
    const double getRadius();
    const std::list<Spot> getSpotList();    
    
  protected:
    spots spots;
};

} /* namespace cleed */

#endif

#endif /* SPOTS_H */
