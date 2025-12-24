/*********************************************************************
 *													Search.cc
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/5 Jan 2015 - creation
 *********************************************************************/

/*!
 * \file Search.cc
 * \author Liam Deacon
 *
 * Provides an object-orientated interface to the search program.
 */

#include <Search/Search.hh>
#include <cstring>
#include <string>
#include <vector>

using namespace cleed;
using std::vector;
using std::string;
using std::size_t;

Search::Search(const string &control_filepath,
       const string &theory_filepath, search_method driver) {

}

Search::~Search() {

}

/* operators */


/* getters */
inline size_t Search::getNumberOfIndependentParameters() const {
  return this->c_search.n_par;
}

inline size_t Search::getNumberOfGeometricalParameters() const {
  return this->c_search.n_par_geo;
}

// coordinate-related
inline double Search::getMinimumX() const {
  return this->c_search.x_min;
}

inline double Search::getMinimumY() const {
  return this->c_search.y_min;
}

inline double Search::getMinimumZ() const {
  return this->c_search.z_min;
}

inline double Search::getMaximumX() const {
  return this->c_search.x_max;
}

inline double Search::getMaximumY() const {
  return this->c_search.y_max;
}

inline double Search::getMaximumZ() const {
  return this->c_search.z_max;
}

vector<double> Search::getBasis() const {

}

// angle-related
inline bool Search::isAngleSearch() const {
  return this->c_search.sr_angle;
}

inline size_t Search::getNumberOfThetaParameters() const {
  return this->c_search.i_par_theta;
}

inline size_t Search::getNumberOfPhiParameters() const {
  return this->c_search.i_par_phi;
}

inline double Search::getInitialThetaValue() const {
  return this->c_search.theta_0;
}

inline double Search::getInitialPhiValue() const {
  return this->c_search.phi_0;
}

// symmetry-related
inline bool Search::isZOnlySearch() const {
  return this->c_search.z_only;
}

inline size_t Search::getNumberOfRotationalSymmetries() const {
  return this->c_search.rot_deg;
}

vector<double> Search::getRotationalAxis() const {

}

vector<double> Search::getMirrorPlanePoint() const {

}

vector<double> Search::getMirrorPlaneDirection() const {

}

// R-factor
inline string Search::getRFactorType() const {
  return string(this->c_search.rfac_type);
}

inline double Search::getRFactorShiftRange() const {
  return this->c_search.rfac_range;
}

inline const RFactor *Search::getRFactor() const {
  return const_cast<RFactor*>(this->rfactor_ptr);
}

/* setters */
inline void Search::setNumberOfIndependentParameters(size_t n_par) {
  this->c_search.n_par = n_par;
}

inline void Search::setNumberOfGeometricalParameters(size_t n_geo) {
  this->c_search.n_par_geo = n_geo;
}

// coordinate-related
inline void Search::setMinimumX(double x_min) {
  this->c_search.x_min = x_min;
}

inline void Search::setMinimumY(double y_min) {
  this->c_search.y_min = y_min;
}

inline void Search::setMinimumZ(double z_min) {
  this->c_search.z_min = z_min;
}

inline void Search::setMaximumX(double x_max) {
  this->c_search.x_max = x_max;
}

inline void Search::setMaximumY(double y_max) {
  this->c_search.y_max = y_max;
}

inline void Search::setMaximumZ(double z_max) {
  this->c_search.z_max = z_max;
}

void Search::setBasis(vector<double> basis) {

}

// angle-related
inline void Search::setAngleSearch(bool angle) {
  this->c_search.sr_angle = angle;
}

inline void Search::setNumberOfThetaParameters(size_t n_theta) {
  this->c_search.i_par_theta = n_theta;
}

inline void Search::setNumberOfPhiParameters(size_t n_phi) {
  this->c_search.i_par_phi = n_phi;
}

inline void Search::setInitialThetaValue(double theta_0) {
  this->c_search.theta_0 = theta_0;
}

inline void Search::setInitialPhiValue(double phi_0) {
  this->c_search.phi_0 = phi_0;
}

// symmetry-related
inline void Search::setZOnlySearch(bool z_only) {
  this->c_search.z_only = z_only;
}

inline void Search::setNumberOfRotationalSymmetries(size_t n_rot) {
  //this->c_search.rot_deg = n_rot;
}

void Search::setRotationalAxis(vector<double> rot_axis) {
  for(int i=0; i < 3; i++) {
    this->c_search.rot_axis[i] = rot_axis.at(i);
  }
}

void Search::setMirrorPlanePoint(vector<double> mir_point) {

}

void Search::setMirrorPlaneDirection(vector<double> mir_dir) {

}

// R-factor
inline void Search::setRFactorType(const string &type) {
  std::strncpy(this->c_search.rfac_type, type.c_str(), 16);
}

inline void Search::setRFactorType(const char *type) {
  std::strncpy(this->c_search.rfac_type, type, 16);
}

inline void Search::setRFactorShiftRange(double range) {
  this->c_search.rfac_range = range;
}

inline void Search::setRFactor(const RFactor &rfac) {
  //!TODO
}

inline void Search::setTheoryFilePath(const string &theory_filepath) {
  this->theory_filepath = theory_filepath;
}

inline void Search::setControlFilePath(const string &control_filepath) {
  this->control_filepath = control_filepath;
}

/** Functions **/
void Search::startSearch() {

}

void Search::stopSearch() {

}

void Search::resumeSearch() {

}
