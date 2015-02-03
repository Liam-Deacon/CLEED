/*********************************************************************
 *                      Layer.cc
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
 * Implements Layer class for manipulating models of LEED layers.
 */

#include "LEED/Layer.h"
#include <iterator>
#include <cstdlib>
#include <stdexcept>

using namespace cleed;

Layer::Layer() {
  this->atoms = nullptr;
  this->n_atoms = 0;
  this->no_of_layer = 0;
  this->periodic = 0;
  this->rel_area = 0;
}

Layer::Layer(const leed_layer *layer) {
  if (layer->atoms != nullptr) {
    this->atoms = static_cast<leed_atom*>
      (std::calloc(layer->n_atoms, sizeof(leed_atom)));

    std::copy(layer->atoms, layer->atoms+layer->n_atoms, this->atoms);
  }
}

Layer::~Layer() {
  if (this->atoms != nullptr) {
    std::free(this->atoms);
  }
}

// getters
inline bool Layer::isPeriodic() {
  return (this->periodic);
}

inline std::size_t Layer::getLayerNumber() {
  return (this->no_of_layer);
}

inline std::size_t Layer::getNumberOfAtoms() {
  return (this->n_atoms);
}

inline leed_structure Layer::getLayerType() {
  return (this->bulk_over);
}

std::vector<real> Layer::getA1() {
  std::vector<real> a1(2);
  a1.push_back(this->a_lat[1]);
  a1.push_back(this->a_lat[3]);
  return (a1);
}
std::vector<real> Layer::getA2() {
  std::vector<real> a2(2);
  a2.push_back(this->a_lat[2]);
  a2.push_back(this->a_lat[4]);
  return (a2);
}

std::vector< std::vector<real> > Layer::getBasis() {
  //!TODO std::vector< std::vector <double> > a(3, std::vector<double>(3));
}

inline real Layer::getRelativeArea() {
  return (this->rel_area);
}

std::vector< std::vector<real> > Layer::getRegistryShift() {

}

std::vector< std::vector<real> > Layer::getVectorFromLast() {

}

std::vector< std::vector<real> > Layer::getVectorToNext() {

}

std::vector<Atom> Layer::getAtomList() {
  std::vector<Atom> atoms;
  for(std::size_t i=0; i < this->n_atoms; i++) {
    //atoms.push_back(Atom(&this->atoms[i]));
  }
  return (atoms);
}

/* setters */
inline void Layer::setPeriodic(bool periodic) {
  this->periodic = periodic;
}

inline void Layer::setLayerNumbers(std::size_t number) {
  this->no_of_layer = number;
}

inline void Layer::setLayerType(leed_structure type) {
  this->bulk_over = type;
}

inline void Layer::setA1(real a1x, real a1y) {
  this->a_lat[1] = a1x;
  this->a_lat[3] = a1y;
}

inline void Layer::setA2(real a2x, real a2y) {
  this->a_lat[2] = a2x;
  this->a_lat[4] = a2y;
}

inline void Layer::setRelativeArea(real area) {
  this->rel_area = area;
}

void Layer::setAtoms(std::vector<Atom> atomList) {
  std::size_t n = atomList.size();
  if (n == 0) return; // no data

  // allocate new atom array and copy data
  if (this->atoms != nullptr) {
    std::free(this->atoms);
  }
  this->atoms = static_cast<leed_atom*>(std::calloc(n, sizeof(leed_atom)));
  std::copy(atoms, atoms + (n*sizeof(leed_atom)), this->atoms);
}

void Layer::setAtoms(const leed_atom *atoms, std::size_t n) {
  if (n == 0 || atoms == nullptr) return; // no data

  // allocate new atom array and copy data
  if (this->atoms != nullptr) {
    std::free(this->atoms);
  }
  this->atoms = static_cast<leed_atom*>(std::calloc(n, sizeof(leed_atom)));
  std::copy(atoms, atoms + (n*sizeof(leed_atom)), this->atoms);
}

void Layer::setAtom(leed_atom *atom, int index) {
  std::size_t n_atoms = this->getNumberOfAtoms();

  // check for out of bounds
  if (std::abs(index) > n_atoms) {
    throw std::out_of_range("Invalid index for atom array\n");
  }

  // access array elements in NumPy style
  if (index < 0) {
    std::copy(atom, atom + sizeof(leed_atom), &this->atoms[n_atoms - index]);
  } else {
    std::copy(atom, atom + sizeof(leed_atom), &this->atoms[index]);
  }
}

void Layer::setAtom(LEEDAtom &atom, int index) {
  //! potentially dangerous cast:
  this->setAtom(reinterpret_cast<leed_atom*>(&atom), index);
}

