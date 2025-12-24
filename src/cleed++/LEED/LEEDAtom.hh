/*********************************************************************
 *													LEEDAtom.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/16.01.2015 - creation
 *********************************************************************/

/*!
 * \file LEEDAtom.h
 * \author Liam Deacon
 */


#ifndef __LEEDATOM_HH__
#define __LEEDATOM_HH__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <iostream>
#include <Core/Atom.hh>
#include <LEED/Layer.hh>
#include "leed_def.h"

namespace cleed {

  class LEEDAtom : public leed_atom {

    public:
      LEEDAtom();
      LEEDAtom(const LEEDAtom &other);
      virtual ~LEEDAtom();

      /* operators */
      LEEDAtom& operator=(const LEEDAtom &other);
      bool operator==(const LEEDAtom &other) const;
      bool operator!=(const LEEDAtom &other) const;
      friend std::ostream& operator<<(std::ostream& out, const LEEDAtom& atom);

      /* getters */
      std::size_t getLayerNumber();
      leed_structure getStructureType();
      leed_matrix_diag getMatrixType();
      real getDebyeWallerFactor();

      virtual double getXPosition() const;
      virtual double getYPosition() const;
      virtual double getZPosition() const;
      virtual std::vector<double> getPosition() const;

      /* setters */
      LEEDAtom& setLayerNumber(std::size_t number);
      LEEDAtom& setMatrixType(leed_matrix_diag type);
      LEEDAtom& setStructureType(leed_structure structure);
      LEEDAtom& setDebyeWallerFactor(real dwf);

      virtual LEEDAtom& setXPosition(double x_pos);
      virtual LEEDAtom& setYPosition(double y_pos);
      virtual LEEDAtom& setZPosition(double z_pos);
      virtual LEEDAtom& setPosition(std::vector<double> pos);

  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __LEEDATOM_HH__ */
