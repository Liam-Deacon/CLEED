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
 *
 * 
 */

#ifndef __Atom_h__
#define __Atom_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include <vector>
#include <cstddef>

#include "leed_def.h"
#include "search_def.h"
#include "Element.h"
#include "Layer.h"

using namespace std;

namespace cleed {

class Atom : BaseElement {
  public:
    Atom();
    Atom(const Atom &other);
    ~Atom();

    /* getters */
    int getType() const;
    std::size_t getNumberOfEquivalentAtoms() const;
    std::size_t getReferenceToSymmetryEquivalentAtom() const;
    std::string &getAtomName() const;
    double getMinimumRadius() const;

    double getXPosition() const;
    double getYPosition() const;
    double getZPosition() const;
    double getDeltaR() const;

    std::vector<real> getXShifts();
    std::vector<real> getYShifts();
    std::vector<real> getZShifts();
    std::vector<real> getDeltaRShifts();

    /* setters */
    void setType(int type);
    void setNumberOfEquivelentAtoms(std::size_t ref);
    void setReferenceToSymmetryEquivalentAtom(std::size_t n_ref);
    void setAtomName(const char *name);
    void setAtomName(const std::string &name);
    void setMinimumRadius(real r_min);

  private:
    search_atom *atom_ptr;
};

class LEEDAtom : BaseElement {
  friend Layer;

  public:
    LEEDAtom();
    LEEDAtom(const LEEDAtom &other);
    ~LEEDAtom();

    /* getters */
    std::size_t getLayerNumber();
    leed_structure getStructureType();
    leed_matrix_diag getMatrixType();
    std::vector<real> getPosition();
    real getDebyeWallerFactor();

    /* setters */
    void setLayerNumber(std::size_t number);
    void setMatrixType(leed_matrix_diag type);
    void setStructureType(leed_structure structure);
    void setPosition(real pos[]);
    void setPosition(const real *pos);
    void setPosition(const std::vector<real> pos);
    void setDebyeWallerFactor(real dwf);

  private:
    leed_atom *atom_ptr;
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Atom_h__ */
