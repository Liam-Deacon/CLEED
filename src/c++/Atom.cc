/*********************************************************************
 *													Atom.cc
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/21 Jan 2015 - creation
 *********************************************************************/

/*!
 * \file Atom.cc
 * \author Liam Deacon
 * \date 21 Jan 2015
 */

#include "Core/Atom.h"

using namespace cleed;


Atom::Atom() : x(0.), y(0.), z(0.) {}

Atom::Atom(double x, double y, double z) {
  setXPosition(x);
  setYPosition(y);
  setZPosition(z);
}

Atom::Atom(std::vector<double> pos) {
  setPosition(pos);
}

Atom::~Atom() {

}

inline double Atom::getXPosition() const {
  return this->x;
}

inline double Atom::getYPosition() const {
  return this->y;
}

inline double Atom::getZPosition() const {
  return this->z;
}

std::vector<double> Atom::getPosition() const {
  std::vector<double> pos;
  pos.push_back(this->x); // X
  pos.push_back(this->y); // Y
  pos.push_back(this->z); // Z
  return pos;
}

inline void Atom::setXPosition(double x_pos) {
  this->x = x_pos;
}

void Atom::setYPosition(double y_pos) {
  this->y = y_pos;
}

void Atom::setZPosition(double z_pos) {
  this->z = z_pos;
}

void Atom::setPosition(std::vector<double> pos) {
  setXPosition(pos.at(0));
  setYPosition(pos.at(1));
  setZPosition(pos.at(2));
}
