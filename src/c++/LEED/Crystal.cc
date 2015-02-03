/*********************************************************************
 *													Crystal.cc
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/12.01.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Implements Crystal class for manipulating low-level #leed_crystal structure.
 */

#include <cstring>
#include <cstddef>
#include <cstdlib>
#include "LEED/Crystal.h"

using namespace cleed;
using std::size_t;

Crystal::Crystal() {
  this->comments = this->alpha = this->layers = this->m_plane = nullptr;
  this->dmin = this->n_atoms = this->n_layers = 0.;
}

Crystal::Crystal(const Crystal &crystal) {

}

Crystal::Crystal(const leed_crystal *crystal_ptr) {

}

Crystal::~Crystal() {
  if (this->comments != nullptr) {

  }
  if (this->alpha != nullptr) {
      std::free(static_cast<void*>(this->alpha));
  }
  if (this->layers != nullptr) {
      std::free(static_cast<void*>(this->layers));
  }
  if (this->m_plane != nullptr) {
      std::free(static_cast<void*>(this->m_plane));
  }
}

/* operators */
bool Crystal::operator==(const Crystal &other) const {

}

bool Crystal::operator!=(const Crystal &other) const {

}

bool Crystal::operator==(const leed_crystal *other) const {

}

bool Crystal::operator!=(const leed_crystal *other) const {

}

/* getters */
inline double Crystal::getRealOpticalPotential() const {
  return this->vr;
}

inline double Crystal::getImaginaryOpticalPotential() const {
  return this->vi;
}

inline double Crystal::getTemperature() const {
  return this->temp;
}

inline size_t Crystal::getNumberOfRotationalSymmetries() const {
  return this->n_rot;
}

inline vector<double> Crystal::getAxisOfRotationalSymmetry() const {
  vector<double> a;
  a.assign(this->rot_axis+1, this->rot_axis+4);
  return a;
}

inline size_t Crystal::getNumberOfMirrorPlanes() const {
  return this->n_mir;
}

inline const real *Crystal::getMirrorPlaneGeometryPtr() const {
  return const_cast<real*>(this->m_plane);
}

vector<double> &Crystal::getMirrorPlaneGeometry() const {

}

inline leed_mirror_sym Crystal::getMirrorSymmetry() const {
  return this->symmetry;
}

inline const real *Crystal::getAngleAlphaDegreesPtr() const {
  return const_cast<real*>(this->alpha);
}

vector<double> &Crystal::getAngleAlphaDegrees() const {

}

vector<double> Crystal::getSubstrateBasisVectors() const {

}

double Crystal::getSubstrateUnitCellArea() const {
  return this->rel_area_sup;
}

vector<double> Crystal::getSuperstructureBasisVectors() const {

}

inline double Crystal::getSuperstructureRelativeUnitCellArea() const {
  return this->rel_area_sup;
}

inline size_t Crystal::getNumberOfLayers() const {
  return this->n_layers;
}

inline const leed_layer *Crystal::getLayersPtr() const {
  return const_cast<leed_layer*>(this->layers);
}

inline double Crystal::getMinimumInterlayerDistance() const {
  return this->dmin;
}

inline size_t Crystal::getNumberOfAtoms() const {
  return this->n_atoms;
}

inline size_t Crystal::getNumberOfTypes() const {
  return this->n_types;
}

std::vector<string> Crystal::getComments() const {
  vector<string> comments;
  for (int i=0; this->comments[i] != nullptr ||
       this->comments[i] != "\0"; i++) {
    comments.push_back(string(this->comments[i]));
  }
  return comments;
}

/* setters */
inline void Crystal::setRealOpticalPotential(double vr) {
  this->vr = vr;
}

inline void Crystal::setImaginaryOpticalPotential(double vi) {
  this->vi = vi;
}

inline void Crystal::setTemperature(double temp) {
  this->temp = temp;
}

inline void Crystal::setNumberOfRotationalSymmetries(size_t n_rot) {
  this->n_rot = n_rot;
}

void Crystal::setAxisOfRotationalSymmetry(vector<double> rot_axis) {
  //this->rot_axis = ...
}

void Crystal::setAxisOfRotationalSymmetry(double *rot_axis, size_t n) {
  if (n < 2) return; // not enough
  else if (n == 2) { // only 2D plane rotation

  }
  else if(n == 3) { // 3D rotation

  } else { // assume FORTRAN style array format (skip index 0)

  }
}

inline void Crystal::setNumberOfMirrorPlanes(size_t n_mir) {
  this->n_mir = n_mir;
}

void Crystal::setMirrorPlaneGeometry(vector<double> m_plane) {

}

void Crystal::setMirrorPlaneGeometry(double *m_plane, size_t n) {

}

inline void Crystal::setMirrorSymmetry(leed_mirror_sym symmetry) {
  //!FIXME: this->symmetry = symmetry;
}

void Crystal::setAngleAlphaDegrees(vector<double> alpha) {

}

void Crystal::setAngleAlphaDegrees(double *alpha, size_t n) {

}

void Crystal::setSubstrateBasisVectors(vector<double> a) {

}

void Crystal::setSubstrateUnitCellArea(double area){

}

void Crystal::setSuperstructureBasisVectors(vector<double> b){

}

void Crystal::setSuperstructureRelativeUnitCellArea(double rel_area) {

}

inline void Crystal::setNumberOfLayers(size_t n_layers) {
  this->n_layers = n_layers;
}

void Crystal::setLayers(vector<leed_layer> layers) {
  //!TODO:
  this->layers = nullptr;
  this->setNumberOfLayers(layers.size());
}

void Crystal::setLayers(leed_layer *layers, size_t n_layers) {
  this->layers = layers;
  this->setNumberOfLayers(n_layers);
}

inline void Crystal::setMinimumInterlayerDistance(double d_min) {
  this->dmin = d_min;
}

inline void Crystal::setNumberOfAtoms(size_t n_atoms) {
  this->n_atoms = n_atoms;
}

void Crystal::setNumberOfTypes(size_t n_types) {
  this->n_types = n_types;
}

void Crystal::setComments(vector<string> comments) {
  for (int i=0; i < comments.size(); i++) {
    strcpy(this->comments[i], comments[i].c_str());
  }
}

void Crystal::setComments(char **comments, size_t n_comments) {
  this->comments = comments; //!FIXME
}
