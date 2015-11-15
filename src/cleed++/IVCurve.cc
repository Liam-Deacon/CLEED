/*********************************************************************
 *													IVCurve.cc
 *
 *  Copyright (C) 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/15 Dec 2014 - creation
 *********************************************************************/

/*!
 * \file IVCurve.cc
 * \author Liam Deacon
 * \date 15 Dec 2014
 *
 * Implements IVCurve class for manipulating IV curves in the R Factor program.
 */

#include <IVCurve.hh>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace cleed;

IVCurve::IVCurve() {
  this->iv_ptr = new rfac_iv;
}

IVCurve::IVCurve(const rfac_iv &iv) {
  this->iv_ptr = new rfac_iv;
  rfac_iv_copy(this->iv_ptr, &iv);
}

IVCurve::IVCurve(const string &ivFilePath) {
  this->iv_ptr = new rfac_iv;
  this->iv_ptr->data = rfac_iv_read(ivFilePath.c_str());
}

IVCurve::IVCurve(const IVCurve &ivCurve) {
  this->iv_ptr = new rfac_iv;
  rfac_iv_copy(this->iv_ptr, ivCurve.iv_ptr);
}

IVCurve::~IVCurve() {
  rfac_iv_free(this->iv_ptr);
}

IVCurve& IVCurve::operator=(const IVCurve &other) {
  if (this != &other) {
    rfac_iv_copy(this->iv_ptr, other.iv_ptr);
  }
  return *this;
}

bool IVCurve::operator!=(const IVCurve &other) const {
    return !(*this == other);
}

bool IVCurve::operator==(const IVCurve &other) const {
    return (*this == other);
}

/* setters */
IVCurve& IVCurve::setIVData(const rfac_iv_data &iv_data, size_t n) {
  if ( &iv_data == nullptr)
    return *this;

  delete [] this->iv_ptr->data;

  this->iv_ptr->data = new rfac_iv_data[n];
  this->iv_ptr->n_eng = n;

  std::copy(&iv_data, &iv_data + n*sizeof(rfac_iv_data), this->iv_ptr->data);
  return *this;
}

IVCurve& IVCurve::setIVData(vector<real> x, vector<real> y, vector<real> deriv2) {
  std::size_t n = ( x.size() < y.size() ) ? x.size() : y.size();

  rfac_iv_data *data = new rfac_iv_data[n];
  for (std::size_t i=0; i < n; i++ ) {
    data[i].energy = x[i];
    data[i].intens = y[i];
  }

  if (n < deriv2.size()) {
    for (std::size_t i=0; i < n; i++)
      data[i].deriv2 = deriv2[i];
  } else {
    for (std::size_t i=0; i < deriv2.size(); i++)
      data[i].deriv2 = deriv2[i];
    for (std::size_t i = deriv2.size(); i < n; i++)
      data[i].deriv2 = 0.;
  }

  if (this->iv_ptr->data != nullptr)
    delete[] this->iv_ptr->data;

  this->iv_ptr->data = data;
  return *this;
}

inline IVCurve& IVCurve::sort() {
  rfac_iv_sort(this->iv_ptr);
  return *this;
}

inline IVCurve& IVCurve::smooth(double vi) {
  rfac_iv_lorentz_smooth(this->iv_ptr, vi/2.);
  return *this;
}

inline IVCurve& IVCurve::spline() {
  rfac_iv_spline(this->iv_ptr);
  return *this;
}

inline IVCurve& IVCurve::setInitialEnergy(double e_0) {
  this->iv_ptr->first_eng = e_0;
  return *this;
}

inline IVCurve& IVCurve::setLastEnergy(double e_n) {
  this->iv_ptr->last_eng = e_n;
  return *this;
}

inline IVCurve& IVCurve::setMaximumIntensity(double maxI) {
  this->iv_ptr->max_int = maxI;
  return *this;
}

void IVCurve::writeIVData(const string &ivFilePath) {
  std::ofstream write ( ivFilePath.c_str() );
  double energy, intensity;

  if ( write.is_open() ) {

    /* set output format */
    write << std::fixed << std::setw(11);
    write << std::setprecision(6) << std::setfill('0');

    /* write comments */
    write << comments << std::endl;

    /* write processing data */
    if (iv_ptr->smooth)
      write << "# Smoothed: True" << std::endl;

    if (iv_ptr->spline)
      write << "# Interpolated: True" << std::endl;

    /* write IV data */
    for (std::size_t i=0; i < iv_ptr->n_eng; i++)
    {
      energy = iv_ptr->data[i].energy;
      intensity = iv_ptr->data[i].intens;

      write << energy << " " << intensity << std::endl;
    }

    write.close();
  }
}

/* getters */
const rfac_iv_data *IVCurve::getRfacIVData() const {
  return const_cast<rfac_iv_data*>(this->iv_ptr->data);
}

void IVCurve::getIVData(vector<real> &x, vector<real> &y, vector<real> &deriv2) const {
  std::size_t n = this->iv_ptr->n_eng;
  rfac_iv_data *data = this->iv_ptr->data;

  /* resize vectors */
  x.resize(n);
  y.resize(n);
  deriv2.resize(n);

  /* fill in data */
  for (std::size_t i=0; i < n; i++) {
    x[i] = data[i].energy;
    y[i] = data[i].intens;
    deriv2[i] = data[i].deriv2;
  }
}

inline bool IVCurve::isEquiDistant() const {
  return this->iv_ptr->equidist;
}

inline bool IVCurve::isSorted() const {
  return this->iv_ptr->sort;
}

inline bool IVCurve::isSmoothed() const {
  return this->iv_ptr->smooth;
}

inline bool IVCurve::isSplined() const {
  return this->iv_ptr->spline;
}

inline double IVCurve::getInitialEnergy() const {
  return this->iv_ptr->first_eng;
}

inline double IVCurve::getLastEnergy() const {
  return this->iv_ptr->last_eng;
}

inline double IVCurve::getMaximumIntensity() const {
  return this->iv_ptr->max_int;
}

inline const rfac_iv *IVCurve::get_rfac_iv_ptr() {
  return const_cast<rfac_iv*>(this->iv_ptr);
}

inline void IVCurve::readIVData(const string &ivFilePath) {
  if (this->iv_ptr->data != nullptr)
    delete[] this->iv_ptr->data;

  this->iv_ptr->data = rfac_iv_read(ivFilePath.c_str());
}
