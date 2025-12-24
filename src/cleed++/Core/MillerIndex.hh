/*********************************************************************
 *                      MillerIndex.hh
 *
 *  Copyright 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 * \author Liam Deacon
 * Provides a base class for Miller index/plane derived classes.
 */
#ifndef MILLER_INDEX_HH
#define MILLER_INDEX_HH

#include <string>
#include <ofstream>

#include "miller_hkl.h"


namespace cleed {

/*! A base class for defining Miller indices & planes.
 */
class MillerIndex {

public: 

  MillerIndex(double h, double k, double l);
  MillerIndex(const miller_hkl *hkl);

  ~MillerIndex();

  // operators
  friend std::ostream & operator << (std::ostream &os, MillerIndex const &);

  // getters
  double getMillerH() const;
  double getMillerK() const;
  double getMillerL() const;
  void getMillerIndices(double &h, double &k, double &l) const;
  const std::string &getMillerIndicesAsString() const;

  // setters
  MillerIndex &setMillerH(double h);
  MillerIndex &setMillerK(double k);
  MillerIndex &setMillerL(double l);
  MillerIndex &setMillerIndices(double h, double k, double l);

private:
  double h;
  double k;
  double l;
} /* class MillerIndex */

} /* namespace cleed /*

#endif /* MILLER_INDEX_HH /*