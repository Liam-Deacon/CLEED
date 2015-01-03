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
 * 
 */

#include "Layer.h"
#include <iterator>

using namespace cleed;

Layer::Layer() {
  this->layer_ptr = new leed_layer;
}

Layer::Layer(const leed_layer *layer) {
  this->layer_ptr = new leed_layer;
  std::copy(layer, layer+sizeof(layer), this->layer_ptr);
  if (layer->atoms != nullptr)
    std::copy(layer->atoms, layer->atoms+layer->n_atoms, this->layer_ptr->atoms);
}

Layer::~Layer() {
  delete[] this->layer_ptr->atoms;
  delete this->layer_ptr;
}

/* getters */
inline bool Layer::isPeriodic() {
  return (this->layer_ptr->periodic);
}

inline std::size_t Layer::getLayerNumber() {
  return (this->layer_ptr->no_of_layer);
}

inline std::size_t Layer::getNumAtoms() {
  return (this->layer_ptr->n_atoms);
}

inline leed_structure Layer::getLayerType() {
  return (this->layer_ptr->bulk_over);
}

std::vector<real> Layer::getA1() {
  std::vector<real> a1(2);
  a1.push_back(this->layer_ptr->a_lat[1]);
  a1.push_back(this->layer_ptr->a_lat[3]);
  return (a1);
}
std::vector<real> Layer::getA2() {
  std::vector<real> a2(2);
  a2.push_back(this->layer_ptr->a_lat[2]);
  a2.push_back(this->layer_ptr->a_lat[4]);
  return (a2);
}

std::vector< std::vector<real> > Layer::getBasis() {

}

inline real Layer::getRelativeArea() {
  return (this->layer_ptr->rel_area);
}

std::vector< std::vector<real> > Layer::getRegistryShift() {

}

std::vector< std::vector<real> > Layer::getVectorFromLast() {

}

std::vector< std::vector<real> > Layer::getVectorToNext() {

}

std::vector<Atom> Layer::getAtomList() {
  std::vector<Atom> atoms;
  for(std::size_t i=0; i < this->layer_ptr->n_atoms; i++) {
    //atoms.push_back(Atom(&this->layer_ptr->atoms[i]));
  }
  return (atoms);
}

/* setters */
inline void Layer::setPeriodic(bool periodic) {
  this->layer_ptr->periodic = periodic;
}

inline void Layer::setLayerNumbers(std::size_t number) {
  this->layer_ptr->no_of_layer = number;
}

inline void Layer::setLayerType(leed_structure type) {
  this->layer_ptr->bulk_over = type;
}

void Layer::setA1(real a1x, real a1y) {

}

void Layer::setA2(real a2x, real a2y) {

}

inline void Layer::setRelativeArea(real area) {
  this->layer_ptr->rel_area = area;
}

void Layer::setAtoms(std::vector<Atom> atomList) {
  if (atomList.size() == 0) return; // no data

  // assign new atom data
  delete[] this->layer_ptr->atoms;
  this->layer_ptr->atoms = new leed_atom[atomList.size()];
  /*
  for(int i = 0; i < atomList.size(); i++) {
    std::copy(atomList.at(i).atom_ptr,
              atomList.at(i).atom_ptr+1,
              this->layer_ptr->atoms[i]);
  }
  */
}

void Layer::setAtoms(const leed_atom *atoms, std::size_t n) {
  if (n == 0 || atoms == nullptr) return; // no data

  delete[] this->layer_ptr->atoms;
  this->layer_ptr->atoms = new leed_atom[n];
  std::copy(atoms, atoms+n, this->layer_ptr->atoms);
}

void Layer::setAtom(leed_atom atom, int index) {

}

void Layer::setAtom(Atom atom, int index) {

}

