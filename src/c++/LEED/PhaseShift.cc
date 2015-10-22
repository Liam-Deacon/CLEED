/*********************************************************************
 *                      PhaseShift.cc
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
 * Implements PhaseShift class.
 */

#include <string>
#include <cstring>
#include <cstdlib>
#include <iterator>
#include <vector>

#include "LEED/PhaseShift.hh"

using namespace std;
using namespace cleed;

PhaseShift::PhaseShift()
{
  // initialise variables
  this->energy = this->pshift = nullptr;
}

PhaseShift::PhaseShift(const leed_phase *phsh_ptr) {
  // initialise variables
  this->energy = this->pshift = nullptr;

  std::copy(phsh_ptr->energy, phsh_ptr->energy+phsh_ptr->n_eng,
            this->energy);
  std::copy(phsh_ptr->pshift, phsh_ptr->pshift+phsh_ptr->n_eng,
            this->pshift);
}

PhaseShift::PhaseShift(const std::string &inputFile) {

  // attempt to coerce legacy C code
  try {
    /*
    std::copy(phsh_ptr->energy, phsh_ptr->energy+phsh_ptr->n_eng,
              this->energy);
    std::copy(phsh_ptr->pshift, phsh_ptr->pshift+phsh_ptr->n_eng,
              this->pshift);
              */
  }
  catch (...)
  {
    if (this->energy != nullptr) {
      std::free(static_cast<void*>(this->energy));
    }
    if (this->pshift != nullptr) {
      std::free(static_cast<void*>(this->pshift));
    }
  }
}

PhaseShift::~PhaseShift() {
  if (this->energy != nullptr) {
    std::free(static_cast<void*>(this->energy));
  }
  if (this->pshift != nullptr) {
    std::free(static_cast<void*>(this->pshift));
  }
}

// getters
inline int PhaseShift::getLmax() const {
  return (this->lmax);
}

inline leed_matrix_diag PhaseShift::getMatrixType() const {
  return (this->t_type);
}

inline real PhaseShift::getMaxEnergy() const {
  return (this->eng_max);
}

inline real PhaseShift::getMinEnergy() const {
  return (this->eng_min);
}
/*
inline std::vector<real> PhaseShift::getEnergyList() {
  std::vector<real> v(this->phsh_ptr->n_eng, this->phsh_ptr->energy);
  return v;
}

inline std::vector<real> PhaseShift::getPhaseShiftValuesList() {
  std::vector<real> v(this->phsh_ptr->n_eng, this->phsh_ptr->pshift);
  return v;
}

inline std::vector<real> PhaseShift::getDeltaRList() {
  std::vector<real> v(4, this->phsh_ptr->dr);
  return v;
}
*/
inline std::size_t PhaseShift::getNumberOfEnergies() const {
  return (this->n_eng);
}

// setters
inline void PhaseShift::setLmax(int lmax) {
  // only set value if lmax is a sensible value */
  if (lmax >= 0 && lmax < 19)
    this->lmax = lmax;
}

inline void PhaseShift::setMatrixType(leed_matrix_diag type) {
  this->t_type = type;
}

inline void PhaseShift::setMaxEnergy(real Emax) {
  this->eng_max = Emax;
}

inline void PhaseShift::setMinEnergy(real Emin) {
  this->eng_min = Emin;
}

inline void PhaseShift::setInputFile(const std::string &filepath) {
  std::strcpy(this->input_file, filepath.c_str());
}

inline void PhaseShift::setInputFile(const char *filepath) {
  std::strcpy(this->input_file, filepath);
}

void PhaseShift::setData(const real *energies_ptr,
  const real *phaseshifts_ptr, std::size_t n) {
  try {
    if (this->energy != nullptr) {
      std::free(static_cast<void*>(this->energy));
    }
    if (this->pshift != nullptr) {
      std::free(static_cast<void*>(this->pshift));
    }
    this->energy = static_cast<real*>(std::calloc(n, sizeof(real)));
    this->pshift = static_cast<real*>(std::calloc(n, sizeof(real)));
    std::copy(energies_ptr, energies_ptr + n, this->energy);
    std::copy(phaseshifts_ptr, phaseshifts_ptr + n, this->pshift);
    this->n_eng = n;
  } catch (...) {
    //!TODO
  }
}
void PhaseShift::setData(const std::vector<real> energies,
  const std::vector<real> phaseshifts) {
  std::size_t n = (energies.size() < phaseshifts.size())
                          ? energies.size() : phaseshifts.size();
  if (this->energy != nullptr) {
    std::free(static_cast<void*>(this->energy));
  }
  if (this->pshift != nullptr) {
    std::free(static_cast<void*>(this->pshift));
  }
  this->energy = static_cast<real*>(std::calloc(n, sizeof(real)));
  this->pshift = static_cast<real*>(std::calloc(n, sizeof(real)));
  std::copy(energies.begin(), energies.end(), this->energy);
  std::copy(phaseshifts.begin(), phaseshifts.end(), this->pshift);
}

void PhaseShift::setDataFromFile() {
  if (this->energy != nullptr) {
    std::free(static_cast<void*>(this->energy));
  }
  if (this->pshift != nullptr) {
    std::free(static_cast<void*>(this->pshift));
  }

}

inline void PhaseShift::setDeltaR(std::vector<real> dr) {
  std::copy(&dr[0], &dr[0]+4, this->dr);
}

inline void PhaseShift::setDeltaR(real dr[4]) {
  for (int i=0; i < 4; i++) {
    this->dr[i] = dr[i];
  }
}
