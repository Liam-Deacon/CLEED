/*********************************************************************
 *													SearchAtom.h
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/16 Jan 2015 - creation
 *********************************************************************/

/*!
 * \file SearchAtom.h
 * \author Liam Deacon
 *
 * Header for providing object-orientated interface to the low-level
 * #search_atom structure.
 */


#ifndef __SearchAtom_H__
#define __SearchAtom_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <vector>
#include <cstddef>
#include <string>

#include <Core/Atom.hh>
#include "search_def.h"

namespace cleed {

  class SearchAtom : public search_atom {
    public:
      SearchAtom();
      SearchAtom(const SearchAtom &other);
      ~SearchAtom();

      /* getters */
      int getType() const;
      std::size_t getNumberOfEquivalentAtoms() const;
      std::size_t getReferenceToSymmetryEquivalentAtom() const;
      std::string getAtomName() const;
      double getMinimumRadius() const;
      double getDeltaR() const;

      virtual double getXPosition() const;
      virtual double getYPosition() const;
      virtual double getZPosition() const;
      virtual std::vector<double> getPosition() const;

      std::vector<double> getXShifts();
      std::vector<double> getYShifts();
      std::vector<double> getZShifts();
      std::vector<double> getDeltaRShifts();

      /* setters */
      void setType(int type);
      void setNumberOfEquivelentAtoms(std::size_t ref);
      void setReferenceToSymmetryEquivalentAtom(std::size_t n_ref);
      void setAtomName(const char *name);
      void setAtomName(const std::string &name);
      void setMinimumRadius(double r_min);

      virtual void setXPosition(double x_pos);
      virtual void setYPosition(double y_pos);
      virtual void setZPosition(double z_pos);
      virtual void setPosition(std::vector<double> pos);

  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __SearchAtom_H__ */
