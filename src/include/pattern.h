/*********************************************************************
 *                           PATTERN.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.07.20 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.deacon@diamond.ac.uk>
 * \brief Header file for #pattern \c struct and associated functions.
 */

#ifndef PATTERN_H
#define PATTERN_H

#include "basis_vector.h"
#include "matrix_2x2.h"
#include "spots.h"
#include <stdbool.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  #include <list>

  extern "C" {
#endif

/*! \typedef pattern
 *  \brief Structure containing information needed for LEED pattern generation.
 */
typedef struct _pattern
{
  char *title;              /*!< pointer to title string */
  basis_vector a1;          /*!< a1 basis vector */
  basis_vector a2;          /*!< a2 basis vector */
  matrix_2x2 *M_SS;         /*!< array of superstructure matrices */
  size_t n_domains;         /*!< number of superstructures */
  double radius;            /*!< maximum radius for valid spots */
  bool square;              /*!< switch for circular or square valid area */
} pattern;

extern pattern pattern_default; 

/* pattern functions */
pattern *pattern_alloc(size_t n_domains);

pattern *pattern_init(size_t n_domains);

pattern *pattern_read(FILE *file);

void pattern_free(pattern *pat);

double pattern_get_radius(const pattern *pat);

bool pattern_is_square(const pattern *pat);

bool pattern_domain_is_commensurate(const pattern *pat, size_t domain);

size_t pattern_get_n_domains(const pattern *pat);

const char *pattern_get_title(const pattern *pat);

void pattern_printf(FILE *stream, const pattern *pat);

void pattern_set_a1(pattern *pat, const basis_vector *a1);

void pattern_set_a2(pattern *pat, const basis_vector *a2);

void pattern_set_radius(pattern *pat, double radius);

void pattern_set_title(pattern *pat, const  char *title);

void pattern_set_square(pattern *pat, bool is_square);

int pattern_set_max_domains(pattern *pat, size_t n_domains);

void pattern_set_superstructure_matrix(pattern *pat, const matrix_2x2 *mat,
                                       size_t domain);
        
const matrix_2x2 *get_superstructure_matrix(const pattern *pat, size_t domain);

spots *pattern_calculate_substrate_spots(const pattern *pat);

spots *pattern_calculate_superstructure_spots(const pattern *pat,
                                                size_t domain);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

/*! \class Pattern
 *  \brief C++ wrapper class for pattern struct and associated functions.
 */
class Pattern {
  public:
    Pattern();
    ~Pattern();
    
    void setTitle(std::string title);
    void setBasisVectorA1(double x, double y);
    void setBasisVectorA2(double x, double y);
    int setNumberOfDomains(std::size_t ndom);
    void setRadius(double radius);
    void setSuperStructureMatrix(Matrix2x2 mat, std::size_t domain);
    void setSquare(bool isSquare);
    
    const std::string getTitle();
    size_t getNumberOfDomains();
    double getRadius();
    const std::list<double> getBasisVectorA1();
    const std::list<double> getBasisVectorA2();
    const std::list<Matrix2x2> getSuperStructureMatrices();
    bool isSquare();
    
  protected:
    pattern pat;
};

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* PATTERN_H */
