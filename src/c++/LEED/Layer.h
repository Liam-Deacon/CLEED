/*********************************************************************
 *                      Layer.h
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
 * Provides object-orientated wrapper class for leed_layer.
 */

#ifndef __Layer_h__
#define __Layer_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include <cstddef>
#include <vector>
#include "leed_def.h"
#include "LEED/LEEDAtom.h"

using namespace std;

namespace cleed {

class Layer : public leed_layer {
  public:
    Layer();
    Layer(const leed_layer *layer);
    ~Layer();

    /* getters */
    bool isPeriodic();
    std::size_t getLayerNumber();
    std::size_t getNumberOfAtoms();
    leed_structure getLayerType();
    std::vector<real> getA1();
    std::vector<real> getA2();
    std::vector< std::vector<real> > getBasis();
    real getRelativeArea();
    std::vector< std::vector<real> > getRegistryShift();
    std::vector< std::vector<real> > getVectorFromLast();
    std::vector< std::vector<real> > getVectorToNext();
    std::vector<Atom> getAtomList();

    /* setters */
    void setPeriodic(bool periodic);
    void setLayerNumbers(std::size_t number);
    void setLayerType(leed_structure type);
    void setA1(real a1x, real a1y);
    void setA2(real a2x, real a2y);
    void setRelativeArea(real area);
    void setAtoms(std::vector<Atom> atomList);
    void setAtoms(const leed_atom *atoms, std::size_t n);
    void setAtom(leed_atom *atom, int index);
    void setAtom(LEEDAtom &atom, int index);

};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Layer_h__ */
