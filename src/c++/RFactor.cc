/*********************************************************************
 *													RFactor.cc
 *
 *  Copyright (C) 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/9 Dec 2014 - creation
 *********************************************************************/

/*!
 * \file RFactor.cc
 * \author Liam Deacon
 * \date 9 Dec 2014
 */

#include "RFactor.h"
#include <algorithm>
#include <iterator>

using namespace cleed;

RFactor::RFactor(const string &control_filepath, const string &theory_filepath) {
  this->args = RFactorArgs(control_filepath, theory_filepath);
  this->updateIVs(control_filepath, theory_filepath);
  this->calculateRFactor();
}

RFactor::~RFactor() {

}

/* operators */
inline bool RFactor::operator==(const RFactor &other) const {
  return (*this == other);
}

inline bool RFactor::operator!=(const RFactor &other) const {
  return !(*this == other);
}

/* setters */
void RFactor::calculateRFactor() {
  //!TODO
  for (size_t i=0; i < 10 ; i++)
  {

  }
  //this->rf = rf_rmin(&iv_datasets[0], this->args.args_ptr);
}

void RFactor::updateIVs(const string &control_filepath, const string &theory_filepath) {
  rfac_ivcur *ivcur_ptr = ::rfac_ivcur_read( control_filepath.c_str(),
                                             theory_filepath.c_str()  );
  size_t n = 0;
  while (ivcur_ptr[n].group_id == END_OF_GROUP_ID) {n++;}
  this->iv_datasets.assign(ivcur_ptr, ivcur_ptr+n);

  // free memory
  rfac_ivcur_free_all(ivcur_ptr);
}

/* getters */
inline double RFactor::getRFactor() const {
  return this->rfactor;
}

inline double RFactor::getRR() const {
  return this->relativeError;
}
