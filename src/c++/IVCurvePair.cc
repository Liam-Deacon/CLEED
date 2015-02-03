/*********************************************************************
 *													IVCurvePair.cc
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file IVCurvePair.cc
 * \author Liam Deacon
 *
 * Implements IVCUrvePair class.
 */

#include "IVCurvePair.h"

using namespace cleed;

IVCurvePair::IVCurvePair(const IVCurvePair &other) {
  std::copy(other.iv_pair, other.iv_pair + sizeof(rfac_ivcur), this->iv_pair);
}

IVCurvePair::IVCurvePair(const rfac_ivcur *ivcur_ptr) {

}

IVCurvePair::IVCurvePair(const IVCurve &theory, const IVCurve &experimental) {

}

IVCurvePair::~IVCurvePair() {

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

}

void IVCurvePair::setWeighting(double weight) {
  this->iv_pair->weight = weight;
}

void IVCurvePair::setSpotID(const rfac_spot *spot) {

}

void IVCurvePair::setTheoryIVCurve(const IVCurve &theory) {
  //!TODO: this->iv_pair->theory = theory.get_rfac_iv_ptr();
}

void IVCurvePair::setExperimentalIVCurve(const IVCurve &experimental) {
  //!TODO: this->iv_pair->experimental = experimental.get_rfac_iv_ptr();
}
