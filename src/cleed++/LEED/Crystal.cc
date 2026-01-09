/*********************************************************************
 *													Crystal.cc
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
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

#include <LEED/Crystal.hh>
#include <cstring>
#include <cstddef>
#include <cstdlib>

using namespace cleed;
using std::size_t;

Crystal::Crystal() {
  this->comments = nullptr;
  this->alpha = nullptr;
  this->layers = nullptr;
  this->m_plane = nullptr;
  this->dmin = 0.;
  this->natoms = 0;
  this->nlayers = 0;
  this->ntypes = 0;
}

Crystal::Crystal(const Crystal &crystal) {

}

Crystal::Crystal(const leed_cryst_t *crystal_ptr) {

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
  return this == &other;
}

bool Crystal::operator!=(const Crystal &other) const {
  return !(*this == other);
}

bool Crystal::operator==(const leed_cryst_t *other) const {
  return other != nullptr &&
         static_cast<const leed_cryst_t*>(this) == other;
}

bool Crystal::operator!=(const leed_cryst_t *other) const {
  return !(*this == other);
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
  static vector<double> geometry;
  geometry.clear();
  if (this->m_plane != nullptr && this->n_mir > 0) {
    geometry.assign(this->m_plane, this->m_plane + (this->n_mir * 2));
  }
  return geometry;
}

inline int Crystal::getMirrorSymmetry() const {
  return this->symmetry;
}

inline const real *Crystal::getAngleAlphaDegreesPtr() const {
  return const_cast<real*>(this->alpha);
}

vector<double> &Crystal::getAngleAlphaDegrees() const {
  static vector<double> alpha;
  alpha.clear();
  if (this->alpha != nullptr && this->n_mir > 0) {
    alpha.assign(this->alpha, this->alpha + this->n_mir);
  }
  return alpha;
}

vector<double> Crystal::getSubstrateBasisVectors() const {
  vector<double> a;
  a.assign(this->a + 1, this->a + 5);
  return a;
}

double Crystal::getSubstrateUnitCellArea() const {
  return this->area;
}

vector<double> Crystal::getSuperstructureBasisVectors() const {
  vector<double> b;
  b.assign(this->b + 1, this->b + 5);
  return b;
}

inline double Crystal::getSuperstructureRelativeUnitCellArea() const {
  return this->rel_area_sup;
}

inline size_t Crystal::getNumberOfLayers() const {
  return this->nlayers;
}

inline const leed_layer_t *Crystal::getLayersPtr() const {
  return const_cast<leed_layer_t*>(this->layers);
}

vector<leed_layer_t> &Crystal::getLayers() const {
  static vector<leed_layer_t> layer_list;
  layer_list.clear();
  if (this->layers != nullptr && this->nlayers > 0) {
    layer_list.assign(this->layers, this->layers + this->nlayers);
  }
  return layer_list;
}

inline double Crystal::getMinimumInterlayerDistance() const {
  return this->dmin;
}

inline size_t Crystal::getNumberOfAtoms() const {
  return this->natoms;
}

inline size_t Crystal::getNumberOfTypes() const {
  return this->ntypes;
}

std::vector<string> Crystal::getComments() const {
  vector<string> comments;
  if (this->comments == nullptr) return comments;

  for (int i = 0; this->comments[i] != nullptr &&
       this->comments[i][0] != '\0'; i++) {
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

inline void Crystal::setMirrorSymmetry(int symmetry) {
  this->symmetry = symmetry;
}

void Crystal::setAngleAlphaDegrees(vector<double> alpha) {

}

void Crystal::setAngleAlphaDegrees(double *alpha, size_t n) {

}

void Crystal::setSubstrateBasisVectors(vector<double> a) {

}

void Crystal::setSubstrateUnitCellArea(double area){
  this->area = area;
}

void Crystal::setSuperstructureBasisVectors(vector<double> b){

}

void Crystal::setSuperstructureRelativeUnitCellArea(double rel_area) {
  this->rel_area_sup = rel_area;
}

inline void Crystal::setNumberOfLayers(size_t n_layers) {
  this->nlayers = n_layers;
}

void Crystal::setLayers(vector<leed_layer_t> layers) {
  //!TODO:
  this->layers = nullptr;
  this->setNumberOfLayers(layers.size());
}

void Crystal::setLayers(leed_layer_t *layers, size_t n_layers) {
  this->layers = layers;
  this->setNumberOfLayers(n_layers);
}

inline void Crystal::setMinimumInterlayerDistance(double d_min) {
  this->dmin = d_min;
}

inline void Crystal::setNumberOfAtoms(size_t n_atoms) {
  this->natoms = n_atoms;
}

void Crystal::setNumberOfTypes(size_t n_types) {
  this->ntypes = n_types;
}

void Crystal::setComments(vector<string> comments) {
  for (int i=0; i < comments.size(); i++) {
    strcpy(this->comments[i], comments[i].c_str());
  }
}

void Crystal::setComments(char **comments, size_t n_comments) {
  this->comments = comments; //!FIXME
}
