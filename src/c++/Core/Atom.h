/*********************************************************************
 *                      Atom.h
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 * \author Liam Deacon
 * Provides a base class for atom-derived classes.
 */

#ifndef __Atom_h__
#define __Atom_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include <vector>
#include <cstddef>
#include <string>

#include "BaseElement.h"

using namespace std;

namespace cleed {

  class Atom : public BaseElement {
    public:
      Atom();
      Atom(double x, double y, double z);
      Atom(std::vector<double> pos);
      virtual ~Atom();

      /* pure virtual functions */
      virtual double getXPosition() const = 0;
      virtual double getYPosition() const = 0;
      virtual double getZPosition() const = 0;
      virtual std::vector<double> getPosition() const = 0;

      virtual void setXPosition(double x_pos) = 0;
      virtual void setYPosition(double y_pos) = 0;
      virtual void setZPosition(double z_pos) = 0;
      virtual void setPosition(std::vector<double> pos) = 0;

    private:
      double x;
      double y;
      double z;
  };

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Atom_h__ */
