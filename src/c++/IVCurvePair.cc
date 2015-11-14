/*********************************************************************
 *													IVCurvePair.cc
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Implements IVCurvePair class.
 */

#include <IVCurvePair.hh>

using namespace cleed;

IVCurvePair::IVCurvePair(const IVCurvePair &other) {
  this->iv_pair = new rfac_ivcur;
  std::copy(other.iv_pair, other.iv_pair + sizeof(rfac_ivcur), this->iv_pair);
}

IVCurvePair::IVCurvePair(const rfac_ivcur *ivcur_ptr) {
  this->iv_pair = new rfac_ivcur;
  if (ivcur_ptr != nullptr)
    std::copy(ivcur_ptr, ivcur_ptr + sizeof(rfac_ivcur), this->iv_pair);
}

IVCurvePair::IVCurvePair(const IVCurve &theory, const IVCurve &experimental) {
  this->iv_pair = new rfac_ivcur;
  this->setExperimentalIVCurve(experimental);
  this->setTheoryIVCurve(theory);
}

IVCurvePair::~IVCurvePair() {
  delete this->iv_pair;
}

// operator overloads
/*
IVCurvePair IVCurvePair::operator=(const IVCurvePair &other) {
  if (this != &other) {
    return IVCurvePair(other);
  } else {
    return *(this);
  }
}

IVCurvePair IVCurvePair::operator=(const rfac_ivcur *ivcur_ptr) {
  return IVCurvePair(ivcur_ptr);
}
*/

// getters
double IVCurvePair::getWeighting() const {
  return this->iv_pair->weight;
}

double IVCurvePair::getEnergyOfFirstAppearance() const {
  return this->iv_pair->eng_0;
}

double IVCurvePair::getOverlap() const {
  return this->iv_pair->overlap; //!TODO
}

int IVCurvePair::getGroupID() const {
  return this->iv_pair->group_id;
}

const rfac_ivcur *IVCurvePair::getIVPairPtr() const {
  return const_cast<rfac_ivcur *>(this->iv_pair);
}

// setters
void IVCurvePair::calculateOverlap() {
  //!TODO
}

IVCurvePair& IVCurvePair::setWeighting(double weight) {
  this->iv_pair->weight = weight;
  return *this;
}

IVCurvePair& IVCurvePair::setSpotID(const rfac_spot *spot) {
  this->iv_pair->spot_id = spot;
  return *this;
}

IVCurvePair& IVCurvePair::setTheoryIVCurve(const IVCurve &theory) {
  //!TODO:
  rfac_iv iv = theory.get_rfac_iv_ptr();
  std::copy(iv, iv + sizeof(rfac_iv), this->iv_pair->theory);
  return *this;
}

IVCurvePair& IVCurvePair::setExperimentalIVCurve(const IVCurve &experimental) {
  //!TODO:
  rfac_iv iv = experimental.get_rfac_iv_ptr();
  std::copy(iv, iv + sizeof(rfac_iv), this->iv_pair->experimental);
  return *this;
}
