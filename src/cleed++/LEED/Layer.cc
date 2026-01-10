/*********************************************************************
 *                      Layer.cc
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
 * Implements Layer class for manipulating models of LEED layers.
 */

#include <LEED/Layer.hh>
#include <iterator>
#include <cstdlib>
#include <stdexcept>

using namespace cleed;

Layer::Layer() {
  this->atoms = nullptr;
  this->natoms = 0;
  this->no_of_layer = 0;
  this->periodic = 0;
  this->rel_area = 0;
}

Layer::Layer(const leed_layer_t *layer) {
  if (layer->atoms != nullptr) {
    this->atoms = static_cast<leed_atom_t*>
      (std::calloc(layer->natoms, sizeof(leed_atom_t)));

    std::copy(layer->atoms, layer->atoms+layer->natoms, this->atoms);
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
  return (this->natoms);
}

inline int Layer::getLayerType() {
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
  std::vector< std::vector<real> > basis;
  basis.push_back(getA1());
  basis.push_back(getA2());
  return basis;
}

inline real Layer::getRelativeArea() {
  return (this->rel_area);
}

std::vector< std::vector<real> > Layer::getRegistryShift() {
  return std::vector< std::vector<real> >();
}

std::vector< std::vector<real> > Layer::getVectorFromLast() {
  return std::vector< std::vector<real> >();
}

std::vector< std::vector<real> > Layer::getVectorToNext() {
  return std::vector< std::vector<real> >();
}

std::vector<Atom> Layer::getAtomList() {
  std::vector<Atom> atoms;
  for(std::size_t i=0; i < this->natoms; i++) {
    //atoms.push_back(Atom(&this->atoms[i]));
  }
  return (atoms);
}

/* setters */
inline Layer& Layer::setPeriodic(bool periodic) {
  this->periodic = periodic;
  return *this;
}

inline Layer& Layer::setLayerNumbers(std::size_t number) {
  this->no_of_layer = number;
  return *this;
}

inline Layer& Layer::setLayerType(int type) {
  this->bulk_over = type;
  return *this;
}

inline Layer& Layer::setA1(real a1x, real a1y) {
  this->a_lat[1] = a1x;
  this->a_lat[3] = a1y;
  return *this;
}

inline Layer& Layer::setA2(real a2x, real a2y) {
  this->a_lat[2] = a2x;
  this->a_lat[4] = a2y;
  return *this;
}

inline Layer& Layer::setRelativeArea(real area) {
  this->rel_area = area;
  return *this;
}

Layer& Layer::setAtoms(std::vector<Atom> atomList) {
  (void)atomList;
  return *this;
}

Layer& Layer::setAtoms(const leed_atom_t *atoms, std::size_t n) {
  if (n == 0 || atoms == nullptr) return *this; // no data

  // allocate new atom array and copy data
  if (this->atoms != nullptr) {
    std::free(this->atoms);
  }
  this->atoms = static_cast<leed_atom_t*>(std::calloc(n, sizeof(leed_atom_t)));
  std::copy(atoms, atoms + n, this->atoms);
  this->natoms = n;

  return *this;
}

Layer& Layer::setAtom(leed_atom_t *atom, int index) {
  std::size_t n_atoms = this->getNumberOfAtoms();

  // check for out of bounds
  if (std::abs(index) > n_atoms) {
    throw std::out_of_range("Invalid index for atom array\n");
  }

  // access array elements in NumPy style
  if (index < 0) {
    std::copy(atom, atom + 1, &this->atoms[n_atoms - index]);
  } else {
    std::copy(atom, atom + 1, &this->atoms[index]);
  }
  return *this;
}

Layer& Layer::setAtom(LEEDAtom &atom, int index) {
  //! potentially dangerous cast:
  this->setAtom(static_cast<leed_atom_t*>(&atom), index);
  return *this;
}
