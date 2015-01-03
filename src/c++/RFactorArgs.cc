/*********************************************************************
 *													RFactorArgs.cc
 *
 *  Copyright (C) 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/17 Dec 2014 - creation
 *********************************************************************/

/*!
 * \file RFactorArgs.cc
 * \author Liam Deacon
 * \date 17 Dec 2014
 */

#include "RFactorArgs.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

using namespace cleed;

/* constructors & destructor */
RFactorArgs::RFactorArgs(int argc, char *argv[]) {
  this->args_ptr.reset(rfac_rdargs(argc, argv));

  /* begin legacy C style memory allocation */
  rfac_args *args_c_ptr = nullptr;
  try {
    std::copy(args_c_ptr, args_c_ptr+1, this->args_ptr);
    std::free(args_c_ptr);
  }
  catch (...) {
    std::cerr << "Unable to allocate args_ptr\n";
    if (args_c_ptr != nullptr) std::free(args_c_ptr);
  }


}

RFactorArgs::RFactorArgs(const rfac_args *args_ptr) {
  this->args_ptr.reset(new rfac_args);
  std::copy(args_ptr, args_ptr+1, this->args_ptr);
}
RFactorArgs::RFactorArgs(const string &ctrFile, const string &theoryFile,
  rfactor_type type, double initialEnergyShift,
  double finalEnergyShift, double energyShiftStep, double vi, bool allGroups,
  const char *ivOutputFile, const char *outputFile) {

  /* create new args pointer and set values */
  this->args_ptr.reset(rfac_args);

  this->setAllGroups(allGroups);

  this->setControlFile(ctrFile);
  this->setTheoryFile(theoryFile);
  this->setIVOutputFile(ivOutputFile);
  this->setOutputFile(outputFile);

  this->setInitialEnergyShift(initialEnergyShift);
  this->setFinalEnergyShift(finalEnergyShift);
  this->setEnergyShiftStep(energyShiftStep);

  this->setOpticalPotential(vi);
  this->setRFactorType(type);
}

RFactorArgs::~RFactorArgs() {
  delete this->args_ptr;
}

/* operators */
RFactorArgs &RFactorArgs::operator=(const RFactorArgs &other) {
  if (this != &other) {
    std::copy(other.args_ptr, other.args_ptr+sizeof(rfac_args), this->args_ptr);
  }
  return *this;
}

RFactorArgs &RFactorArgs::operator=(const rfac_args *args_ptr) {
  if (this->args_ptr != args_ptr) {
    std::copy(args_ptr, args_ptr+sizeof(rfac_args), this->args_ptr);
  }
  return *this;
}

/* getters */
inline string RFactorArgs::getControlFile() const {
  return string(this->args_ptr->ctr_file);
}

inline string RFactorArgs::getTheoryFile() const {
  return string(this->args_ptr->the_file);
}

inline string RFactorArgs::getOutputFile() const {
  return string(this->args_ptr->out_file);
}

inline string RFactorArgs::getIVOutputFile() const {
  return string(this->args_ptr->iv_file);
}

inline rfactor_type RFactorArgs::getRFactorType() const {
  return this->args_ptr->r_type;
}

inline double RFactorArgs::getInitialEnergyShift() const {
  return this->args_ptr->s_ini;
}

inline double RFactorArgs::getFinalEnergyShift() const {
  return this->args_ptr->s_fin;
}

inline double RFactorArgs::getEnergyShiftStep() const {
  return this->args_ptr->s_step;
}

inline double RFactorArgs::getOpticalPotential() const {
  return this->args_ptr->vi;
}

inline bool RFactorArgs::getAllGroups() const {
  return this->args_ptr->all_groups;
}

/* setters */
void RFactorArgs::setControlFile(const string &ctrFile) {
  std::strcpy(this->args_ptr->ctr_file, ctrFile.c_str());
}

void RFactorArgs::setTheoryFile(const string &theoryFile) {
  std::strcpy(this->args_ptr->the_file, theoryFile.c_str());
}

void RFactorArgs::setOutputFile(const string &outputFile) {
  std::strcpy(this->args_ptr->out_file, outputFile.c_str());
}

void RFactorArgs::setIVOutputFile(const string &ivOutputFile) {
  std::strcpy(this->args_ptr->iv_file, ivOutputFile.c_str());
}

void RFactorArgs::setRFactorType(rfactor_type type) {
  switch (type) {
    case RP_FACTOR: case R1_FACTOR: case R2_FACTOR: case RB_FACTOR:
      this->args_ptr->r_type = type;
      break;
    default:
      break;
  }
}

void RFactorArgs::setInitialEnergyShift(double dE0) {
  this->args_ptr->s_ini = dE0;
}

void RFactorArgs::setFinalEnergyShift(double dEf) {
  this->args_ptr->s_fin = dEf;
}

void RFactorArgs::setEnergyShiftStep(double dEs) {
  this->args_ptr->s_step = dEs;
}

void RFactorArgs::setOpticalPotential(double vi) {
  this->args_ptr->vi = vi;
}

inline void RFactorArgs::setAllGroups(bool allGroups) {
  this->args_ptr->all_groups = allGroups;
}
