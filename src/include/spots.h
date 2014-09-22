#ifndef SPOTS_H
#define SPOTS_H

#include "miller_index.h"
#include <stdbool.h>
#include "patt_colors.h"

/*! \def SPOT_GS
 *  \brief Indexing of substrate spots.
 */
#define SPOT_GS 0
/*! \def SPOT_SS
 *  \brief Indexing of superstructure spots.
 */
#define SPOT_SS 1

#define RADIUS_GS 10.
#define RADIUS_SS  5.


#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  extern "C" {
#endif

/* shapes */
/*! \enum patt_shape_t 
 */
typedef enum {
  PATT_CIRCLE,          /*!< circle shape */
  PATT_TRIANGLE_UP,     /*!< up triangle shape */
  PATT_TRIANGLE_DOWN,   /*!< down triangle shape */
  PATT_SQUARE,          /*!< square shape */
  PATT_DIAMOND,         /*!< diamond shape */
  PATT_HEXAGON,         /*!< hexagon shape */
  PATT_SHAPE_UNKNOWN    /*!< unknown shape */
} patt_shape_t;

/* stroke styles */
/*! \enum patt_stroke_t
 */
typedef enum {
  PATT_SOLID_STROKE,    /*!< solid stroke */
  PATT_DASHED_STROKE,   /*!< dashed stroke */
  PATT_DOTTED_STROKE,   /*!< dotted stroke */
  PATT_DASHDOT_STROKE,  /*!< dashdot stroke */
  PATT_NO_STROKE        /*!< no stroke */
} patt_stroke_t;

/*! \struct spot_t
 *  \brief LEED spot structure.
 */
typedef struct spot_t
{
  char* label;          /*!< spot label */
  miller_hkl_t index;   /*!< spot index */
  double x;             /*!< x position of spot */
  double y;             /*!< y position of spot */
} spot_t;

/*! \struct spots_t
 *  \brief LEED spot list structure.
 *
 * List structure for LEED spots. Contains members to control drawing 
 * of spots group.
 */
typedef struct spots_t
{
  bool fill;                    /*!< flag to determine if spots are filled */
  bool visible;                 /*!< flag to determine if spots are shown */
  bool indexing;                /*!< flag to determine is labels are shown */
  patt_color_rgb_t color;       /*!< color of drawn spots */
  patt_shape_t shape;           /*!< shape of drawn spots */
  double stroke_width;          /*!< stroke width of drawn spots */
  patt_stroke_t stroke_style;   /*!< stroke type of drawn spots */
  double font_size;             /*!< font size of labels */
  char *font_name;              /*!< font face for label text */
  double radius;                /*!< radius of spots */
  int flag;                     /*!< extra switch */
  spot_t *spots;                /*!< array of spots */
  size_t n_spots;               /*!< maximum number of spots in array */
  size_t allocated_size;        /*!< allocated number of spots for array */
} spots_t;

extern spot_t spot_default; 

/* spots_t functions */
/*! \fn spots_t *spots_alloc(size_t n_spots)
 *  \brief Allocate memory for spots. 
 *  \param n_spots Size of array to allocate to memory.
 *  \return \sa spots_t* Pointer to spots_t structure.
 *
 * Allocate memory for array of spots of size \a n_spots.
 */
spots_t *spots_alloc(size_t n_spots);

/*! \fn spots_t *spots_init(size_t n_spots)
 *  \brief Initialise a \a spots_t structure. 
 *  \param n_spots Size of array to allocate to memory.
 *  \return \sa spots_t* Pointer to spots_t structure.
 *
 * Allocate memory for array of spots of size \a n_spots.
 * Set structure members to default valves.
 */
spots_t *spots_init(size_t n_spots);

/*! \fn void spots_free(spots_t *spots)
 *  \brief Free memory of a \a spots_t structure. 
 *  \param *spots Pointer to spots_t object to free from memory.
 *
 * Free memory of a \a spots_t structure.
 */
void spots_free(spots_t *spots);

/*! \fn int spots_list_realloc(spots_t *spots, size_t size)
 *  \brief Resize a \a spots_t structure. 
 *  \param *spots Pointer to spots_t structure. 
 *  \param size Size of array to allocate to memory.
 *  \return success of realloc.
 *
 */
int spots_list_realloc(spots_t *spots, size_t size);

/*! \fn void spots_append(spots_t *spots, spot_t spot)
 *  \brief Add a \a spot to the \a spots list.
 *  \param *spots Pointer to spots_t structure. 
 *  \param spot spot_t struct.
 *
 */
void spots_append(spots_t *spots, spot_t spot);

/*! \fn void spots_set_list(spots_t *spots, spot_t *spot, size_t n_spots)
 *  \brief Assign a \a spot array to the \a spots struct.
 *  \param *spots Pointer to spots_t structure. 
 *  \param *spot Array of spot_t.
 *
 */
void spots_set_list(spots_t *spots, spot_t *spot, size_t n_spots);

/*! \fn void spots_set_color(spots_t *spots, patt_color_rgb_t color)
 *  \brief Set \a color of the \a spots struct.
 *  \param *spots Pointer to spots_t structure. 
 *  \param *color A patt_color_rgb_t struct.
 *
 */
void spots_set_color(spots_t *spots, patt_color_rgb_t color);

/*! \fn void spots_set_color(spots_t *spots, double red, double green, double blue)
 *  \brief Set color of the \a spots struct using individual RGB channels.
 *  \param *spots Pointer to spots_t structure. 
 *  \param red Fraction of red color.
 *  \param green Fraction of green color.
 *  \param blue Fraction of blue color.
 *
 */
void spots_set_color_rgb(spots_t *spots, 
                         double red, double green, double blue);

void spots_set_fill(spots_t *spots, bool fill);
void spots_set_shape(spots_t *spots, patt_shape_t shape);
void spots_set_stroke_width(spots_t *spots, double stroke_width);
void spots_set_stroke_style(spots_t *spots, int stroke_style);
void spots_set_font_size(spots_t *spots, double font_size);
void spots_set_font_name(spots_t *spots, char *font_name);  
void spots_set_radius(spots_t *spots, double radius);
void spots_set_visible(spots_t *spots, bool visible);
void spots_set_indexing(spots_t *spots, bool indexing);

patt_color_rgb_t spots_get_color(const spots_t *spots);
patt_shape_t spots_get_shape(const spots_t *spots);
patt_stroke_t spots_get_stroke_style(const spots_t *spots);
bool spots_get_fill(const spots_t *spots); 
bool spots_get_visible(const spots_t *spots);
double spots_get_stroke_width(const spots_t *spots);
double spots_get_font_size(const spots_t *spots);
const char* spots_get_font_name(const spots_t *spots);  
double spots_get_radius(const spots_t *spots);
size_t spots_get_n_spots(const spots_t *spots);
const spot_t *spots_get_list(const spots_t *spots);
bool spots_show_indexing(const spots_t *spots);

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
    void setK(double h);

    /*! \brief Miller index l setter function. 
     *  \sa setK()
     *  \param k Value of Miller index .
     */ 
    void setK(double h);
    
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
     * Wrapped spot_t C structure. 
     */
    spot_t spot;  
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
    void setFontName(string name);
    void setRadius(double radius);
    void setVisible(bool visible);
    void setIndexing(bool indexing);
    void setSpotList(list<Spot> spots);
    
    /* getters */
    const bool isFilled();
    const bool isVisible();
    const bool isIndexed();
    const int getColor();
    const int getShape();
    const double getStrokeWidth();
    const int getStrokeStyle();
    const double getFontSize();
    const string getFontName();
    const double getRadius();
    const list<Spot> getSpotList();    
    
  protected:
    spots_t spots;
};

} /* namespace cleed */

#endif

#endif /* SPOTS_H */
