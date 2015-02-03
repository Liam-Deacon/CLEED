/*********************************************************************
 *													LEEDAtom.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.deacon@diamond.ac.uk>
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


#ifndef __LEEDAtom_H__
#define __LEEDAtom_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include "Core/Atom.h"
#include "LEED/Layer.h"
#include "leed_def.h"

namespace cleed {

  class LEEDAtom : public leed_atom {

    public:
      LEEDAtom();
      LEEDAtom(const LEEDAtom &other);
      ~LEEDAtom();

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
      void setLayerNumber(std::size_t number);
      void setMatrixType(leed_matrix_diag type);
      void setStructureType(leed_structure structure);
      void setDebyeWallerFactor(real dwf);

      virtual void setXPosition(double x_pos);
      virtual void setYPosition(double y_pos);
      virtual void setZPosition(double z_pos);
      virtual void setPosition(std::vector<double> pos);

  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __LEEDAtom_H__ */
