/*********************************************************************
 *													Basis.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/23 Jan 2015 - creation
 *********************************************************************/

/*!
 * \file Basis.h
 * \author Liam Deacon
 *
 * Provides an OO interface to the #basis \c struct and its associated functions.
 */


#ifndef __BASIS_HH__
#define __BASIS_HH__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <Core/Coordinate.hh>
#include "basis.h"

namespace cleed {

/*!
 * \brief C++ class for #basis and its associated functions.
 */
class Basis : public basis {
  public:
    Basis();
    Basis(const basis *a);
    Basis(const Basis &a);
    ~Basis();

    /* setters */
    void setA1(const Coordinate &a1);
    void setA1(double a1_x, double a1_y, double a1_z);
    void setA2(const Coordinate &a3);
    void setA2(double a2_x, double a2_y, double a2_z);
    void setA3(const Coordinate &a3);
    void setA3(double a3_x, double a3_y, double a3_z);
    void setBasis(const Coordinate &a1, const Coordinate &a2, const Coordinate &a3);
    void setBasis(const Basis &basis);

    /* getters */
    const Coordinate getA1();
    const Coordinate getA2();
    const Coordinate getA3();

    /* other methods */
    double **calculateRotationMatrix();
    void rotate(double **R);
    void rotate(const std::vector< std::vector<double> > &R);
    void rotate(double alpha, double beta, double gamma);
};


} /* namespace cleed */
#endif /* __cplusplus */

#endif /* __BASIS_HH__ */
