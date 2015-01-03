/*********************************************************************
 *                      PhaseShift.cc
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
 * Implements PhaseShift class.
 */

#include "PhaseShift.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <iterator>
#include <vector>

using namespace std;
using namespace cleed;

PhaseShift::PhaseShift()
{
  this->phsh_ptr = new leed_phase;
}

PhaseShift::PhaseShift(const leed_phase *phsh_ptr) {
  this->phsh_ptr = new leed_phase;
  std::copy(phsh_ptr, phsh_ptr+sizeof(leed_phase), this->phsh_ptr);
  std::copy(phsh_ptr->energy, phsh_ptr->energy+phsh_ptr->n_eng,
            this->phsh_ptr->energy);
  std::copy(phsh_ptr->pshift, phsh_ptr->pshift+phsh_ptr->n_eng,
            this->phsh_ptr->pshift);
}

PhaseShift::PhaseShift(const std::string &inputFile) {
  leed_phase *phsh_ptr = nullptr;

  /* attempt to coerce legacy C code */
  try {
    this->phsh_ptr = new leed_phase();
    std::copy(phsh_ptr, phsh_ptr+sizeof(leed_phase), this->phsh_ptr);
    std::copy(phsh_ptr->energy, phsh_ptr->energy+phsh_ptr->n_eng,
              this->phsh_ptr->energy);
    std::copy(phsh_ptr->pshift, phsh_ptr->pshift+phsh_ptr->n_eng,
              this->phsh_ptr->pshift);
    if (phsh_ptr != nullptr) std::free(phsh_ptr);
  }
  catch (...)
  {
    if (phsh_ptr != nullptr) std::free(phsh_ptr);
  }
}

PhaseShift::~PhaseShift() {
  delete this->phsh_ptr;
}

/* getters */
inline int PhaseShift::getLmax()
{
  return (this->phsh_ptr->lmax);
}

inline leed_matrix_diag PhaseShift::getMatrixType() {
  return (this->phsh_ptr->t_type);
}

inline real PhaseShift::getMaxEnergy() {
  return (this->phsh_ptr->eng_max);
}

inline real PhaseShift::getMinEnergy() {
  return (this->phsh_ptr->eng_min);
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
inline std::size_t PhaseShift::getNumEnergies() {
  return (this->phsh_ptr->n_eng);
}

/* setters */
inline void PhaseShift::setLmax(int lmax) {
  // only set value if lmax is a sensible value */
  if (lmax >= 0 && lmax < 19)
    this->phsh_ptr->lmax = lmax;
}

inline void PhaseShift::setMatrixType(leed_matrix_diag type) {
  this->phsh_ptr->t_type = type;
}

inline void PhaseShift::setMaxEnergy(real Emax) {
  this->phsh_ptr->eng_max = Emax;
}

inline void PhaseShift::setMinEnergy(real Emin) {
  this->phsh_ptr->eng_min = Emin;
}

inline void PhaseShift::setInputFile(const std::string &filepath) {
  std::strcpy(this->phsh_ptr->input_file, filepath.c_str());
}

inline void PhaseShift::setInputFile(const char *filepath) {
  std::strcpy(this->phsh_ptr->input_file, filepath);
}

void PhaseShift::setData(const real *energies_ptr,
  const real *phaseshifts_ptr, std::size_t n) {
  try {
    delete[] this->phsh_ptr->energy;
    delete[] this->phsh_ptr->pshift;
    this->phsh_ptr->energy = new real[n]();
    this->phsh_ptr->pshift = new real[n]();
    std::copy(energies_ptr, energies_ptr + n, this->phsh_ptr->energy);
    std::copy(phaseshifts_ptr, phaseshifts_ptr + n, this->phsh_ptr->pshift);
    this->phsh_ptr->n_eng = n;
  } catch (...) {
    //!TODO
  }
}
void PhaseShift::setData(const std::vector<real> energies,
  const std::vector<real> phaseshifts) {
  std::size_t n = (energies.size() < phaseshifts.size())
                          ? energies.size() : phaseshifts.size();
  delete[] this->phsh_ptr->energy;
  delete[] this->phsh_ptr->pshift;
  this->phsh_ptr->energy = new real[n];
  this->phsh_ptr->pshift = new real[n];
  std::copy(energies.begin(), energies.end(), this->phsh_ptr->energy);
  std::copy(phaseshifts.begin(), phaseshifts.end(), this->phsh_ptr->pshift);
}

void PhaseShift::setDataFromFile() {
  delete[] this->phsh_ptr->energy;
  delete[] this->phsh_ptr->pshift;
}

inline void PhaseShift::setDeltaR(std::vector<real> dr) {
  std::copy(&dr[0], &dr[0]+4, this->phsh_ptr->dr);
}

inline void PhaseShift::setDeltaR(real dr[4]) {
  for (int i=0; i < 4; i++) {
    this->phsh_ptr->dr[i] = dr[i];
  }
}
