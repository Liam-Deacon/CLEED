/*********************************************************************
 *													Pattern.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/09.01.2015 - creation
 *********************************************************************/

/*!
 * \file Pattern.h
 * \author Liam Deacon
 *
 * Header file defining an object-orientated wrapper to the #pattern structure
 * and its associated functions.
 */


#ifndef __Pattern_H__
#define __Pattern_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <vector>
#include "pattern.h"

namespace cleed {

/*! \class Pattern
 *  \brief C++ wrapper class for pattern struct and associated functions.
 */
class Pattern {
  public:
    Pattern();
    Pattern(const Pattern &other);
    Pattern(const pattern *pattern_ptr);
    Pattern();
    ~Pattern();

    /* operators */


    /* getters */
    const std::string getTitle();
    size_t getNumberOfDomains();
    double getRadius();
    const std::vector<double> getBasisVectorA1();
    const std::vector<double> getBasisVectorA2();
    const std::vector<Matrix2x2> getSuperStructureMatrices();
    bool isSquare();

    /* setters */
    void setTitle(std::string title);
    void setBasisVectorA1(double x, double y);
    void setBasisVectorA2(double x, double y);
    int setNumberOfDomains(std::size_t n_dom);
    void setRadius(double radius);
    void setSuperStructureMatrix(Matrix2x2 *mat, std::size_t domain);
    void setSquare(bool isSquare);

  private:
    pattern pat;
};

} /* namespace cleed */

#endif /* __cplusplus__ */

#endif /* __Pattern_H__ */
