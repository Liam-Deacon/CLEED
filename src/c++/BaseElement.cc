/*********************************************************************
 *													BaseElement.cc
 *
 *  Copyright (C) 2015 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file BaseElement.cc
 * \author Liam Deacon
 *
 * Implements BaseElement class for describing basic chemical elements.
 */

#include "Core/BaseElement.h"
#include <cstring>

using namespace cleed;


BaseElement::BaseElement() {

}

BaseElement::BaseElement(std::size_t num) {

}

BaseElement::BaseElement(const std::string &name) {

}

BaseElement::BaseElement(const char symbol[]) {

}

BaseElement::~BaseElement() {

}

/* getters */
inline std::size_t BaseElement::getAtomicNumber() const {
  return this->atomicNumber;
}

inline std::string BaseElement::getElementName() const {
  return std::string(this->name);
}

inline std::string BaseElement::getElementSymbol() const {
  return std::string(this->symbol);
}

inline void BaseElement::setAtomicNumber(std::size_t Z) {
  this->atomicNumber = (Z < 118) ? Z : 1;
}

inline void BaseElement::setElementName(const std::string &name) {
  this->name = name;
}

inline void BaseElement::setElementSymbol(const char symbol[]) {
  std::strncpy(this->symbol, symbol, 2);
}

