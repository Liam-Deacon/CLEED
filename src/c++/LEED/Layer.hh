/*********************************************************************
 *                      Layer.h
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
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

#ifndef __LAYER_HH__
#define __LAYER_HH__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include <cstddef>
#include <iostream>
#include <vector>
#include "leed_def.h"
#include <LEED/LEEDAtom.hh>

using namespace std;

namespace cleed {

class Layer : public leed_layer {
  public:
    Layer();
    Layer(const leed_layer *layer);
    ~Layer();

    /* operators */
    friend std::ostream& operator<<(std::ostream& out, const Layer& layer);

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
    Layer& setPeriodic(bool periodic);
    Layer& setLayerNumbers(std::size_t number);
    Layer& setLayerType(leed_structure type);
    Layer& setA1(real a1x, real a1y);
    Layer& setA2(real a2x, real a2y);
    Layer& setRelativeArea(real area);
    Layer& setAtoms(std::vector<Atom> atomList);
    Layer& setAtoms(const leed_atom *atoms, std::size_t n);
    Layer& setAtom(leed_atom *atom, int index);
    Layer& setAtom(LEEDAtom &atom, int index);

};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __LAYER_HH__ */
