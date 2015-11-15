/*********************************************************************
 *													SearchAtom.cc
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/15.01.2015 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Implements SearchAtom class.
 */

#include <Search/SearchAtom.hh>
#include <cstring>
#include <cstdlib>
#include "gh_stddef.h"

using namespace cleed;

SearchAtom::SearchAtom() {
  this->x_par = nullptr;
  this->y_par = nullptr;
  this->z_par = nullptr;
  this->dr_par = nullptr;

  this->setXPosition(0.);
  this->setYPosition(0.);
  this->setZPosition(0.);
  this->setAtomName("\0");
}

SearchAtom::SearchAtom(const SearchAtom &other) {

}

SearchAtom::~SearchAtom() {
  std::free(this->x_par);
  std::free(this->y_par);
  std::free(this->z_par);
  std::free(this->dr_par);
}

// getters
inline int SearchAtom::getType() const {
  return this->type;
}

inline std::size_t SearchAtom::getNumberOfEquivalentAtoms() const {
  return this->n_ref;
}

inline std::size_t SearchAtom::getReferenceToSymmetryEquivalentAtom() const {
  return this->ref;
}

inline std::string SearchAtom::getAtomName() const {
  return std::string(this->name);
}

inline double SearchAtom::getMinimumRadius() const {
  return this->r_min;
}

inline double SearchAtom::getXPosition() const {
  return this->x;
}

inline double SearchAtom::getYPosition() const {
  return this->y;
}

inline double SearchAtom::getZPosition() const {
  return this->z;
}

inline double SearchAtom::getDeltaR() const {
  return this->dr;
}

inline std::vector<double> SearchAtom::getXShifts() {
  return std::vector<double>(this->x_par, this->x_par + this->n_ref);
}

inline std::vector<double> SearchAtom::getYShifts() {
  return std::vector<double>(this->y_par, this->y_par + this->n_ref);
}

std::vector<double> SearchAtom::getZShifts() {
  return std::vector<double>(this->z_par, this->z_par + this->n_ref);
}

std::vector<double> SearchAtom::getDeltaRShifts() {
  return std::vector<double>(this->dr_par, this->dr_par + this->n_ref);
}

std::vector<double> SearchAtom::getPosition() const {
  return std::vector<double>({this->getXPosition(),
    this->getYPosition(), this->getZPosition()});
}

// setters
inline void SearchAtom::setType(int type) {
  this->type = type;
}

inline void SearchAtom::setNumberOfEquivelentAtoms(std::size_t ref) {
  this->ref = ref;
}

inline void SearchAtom::setReferenceToSymmetryEquivalentAtom(std::size_t n_ref) {
  this->n_ref = n_ref;
}

inline void SearchAtom::setAtomName(const char *name) {
  std::strncpy(this->name, name, STRSZ);
}

inline void SearchAtom::setAtomName(const std::string &name) {
  std::strncpy(this->name, name.c_str(), STRSZ);
}

inline void SearchAtom::setMinimumRadius(double r_min) {
  this->dr = r_min;
}

inline void SearchAtom::setXPosition(double x_pos) {
  this->x = x_pos;
}

inline void SearchAtom::setYPosition(double y_pos) {
  this->y = y_pos;
}

inline void SearchAtom::setZPosition(double z_pos) {
  this->z = z_pos;
}

void SearchAtom::setPosition(std::vector<double> pos) {
  this->setXPosition(pos.at(0));
  this->setYPosition(pos.at(1));
  this->setZPosition(pos.at(2));
}
