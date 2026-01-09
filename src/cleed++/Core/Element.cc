/*********************************************************************
 *                      Element.cc
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
 *
 * 
 */

#include <algorithm>
#include <cmath>

#include <Core/Element.hh>

using namespace cleed;

const std::map<int, char> Element::PERIODS = {
        {1, 'K'}, {2, 'L'}, {3, 'M'}, {4, 'N'}, {5, 'O'}, {6, 'P'}, {7, 'Q'}
};

const std::vector<char> Element::BLOCKS = {'s', 'g', 'f', 'd', 'p'};

const std::map<int, std::tuple<std::string, std::string>> Element::GROUPS = {
        {1, std::make_tuple("IA", "Alkali metals")},
        {2, std::make_tuple("IIA", "Alkaline earths")},
        {3, std::make_tuple("IIIB", "")},
        {4, std::make_tuple("IVB", "")},
        {5, std::make_tuple("VB", "")},
        {6, std::make_tuple("VIB", "")},
        {7, std::make_tuple("VIIB", "")},
        {8, std::make_tuple("VIIIB", "")},
        {9, std::make_tuple("VIIIB", "")},
        {10, std::make_tuple("VIIIB", "")},
        {11, std::make_tuple("IB", "Coinage metals")},
        {12, std::make_tuple("IIB", "")},
        {13, std::make_tuple("IIIA", "Boron group")},
        {14, std::make_tuple("IVA", "Carbon group")},
        {15, std::make_tuple("VA", "Pnictogens")},
        {16, std::make_tuple("VIA", "Chalcogens")},
        {17, std::make_tuple("VIIA", "Halogens")},
        {18, std::make_tuple("VIIIA", "Noble gases")}
};

const std::map<int, std::string> Element::SERIES = {
        {1, "Nonmetals"},
        {2, "Noble gases"},
        {3, "Alkali metals"},
        {4, "Alkaline earth metals"},
        {5, "Metalloids"},
        {6, "Halogens"},
        {7, "Poor metals"},
        {8, "Transition metals"},
        {9, "Lanthanides"},
        {10, "Actinides"}
};

const std::map<int, std::string> Element::DESCRIPTIONS = {};

Element::Element(int Z, const std::string &symbol, const std::string &name)
  : BasicElement(Z)
{
  setSymbol(symbol);
  setName(name);
  group = 0;
  period = 0;
  block = '\0';
  series = 0;
  molarMass = 0.0;
  electronegativity = 0.0;
  electronAffinity = 0.0;
  covalentRadius = 0.0;
  atomicRadius = 0.0;
  vdwRadius = 0.0;
  boilingPoint = 0.0;
  meltingPoint = 0.0;
  density = 0.0;
  relativeAbundance = 0.0;
}

Element::~Element() {

}

inline int Element::getGroup() const {
  return group;
}

inline int Element::getPeriod() const {
  return period;
}

inline char Element::getBlock() const {
  return block;
}

inline int Element::getSeries() const {
  return series;
}

inline double Element::getMolarMass() const {
  return molarMass;
}

inline double Element::getElectronegivity() const {
  return electronegativity;
}

inline double Element::getElectronAffinity() const {
  return electronAffinity;
}

inline double Element::getCovalentRadius() const {
  return covalentRadius;
}

inline double Element::getAtomicRadius() const {
  return atomicRadius;
}

inline double Element::getVanDerWaalsRadius() const {
  return vdwRadius;
}

inline double Element::getBoilingPoint() const {
  return boilingPoint;
}

inline double Element::getMeltingPoint() const {
  return meltingPoint;
}

inline double Element::getDensity() const {
  return density;
}

inline const std::string &Element::getElectronConfiguration() const {
  return electronConfig;
}

inline const std::string &Element::getOxidationStates() const {
  return oxidationStates;
}

inline const std::vector<double> &Element::getIonisationEnergies() const {
  return ionisationEnergies;
}

inline const std::string &Element::getDescription() const {
  return description;
}

inline Element &Element::setGroup(int group) {
  if (GROUPS.find(group) == GROUPS.end()) {
    throw invalidElementException(
        ("invalid chemical group: " + std::to_string(group)).c_str());
  }
  this->group = group;
  return *this;
}

Element &Element::setGroup(const std::string &group) {
  for (const auto &entry : GROUPS) {
    if (std::get<0>(entry.second) == group) {
      this->group = entry.first;
      return *this;
    }
  }
  throw invalidElementException(("invalid chemical group: " + group).c_str());
  return *this;
}

Element &Element::setPeriod(int period) {
  if (PERIODS.find(period) == PERIODS.end()) {
    throw invalidElementException(
        ("invalid chemical period: " + std::to_string(period)).c_str());
  }
  this->period = period;
  return *this;
}

Element &Element::setPeriod(const std::string &period) {
  if (period.size() == 1) {
    char symbol = period[0];
    for (const auto &entry : PERIODS) {
      if (entry.second == symbol) {
        this->period = entry.first;
        return *this;
      }
    }
  }
  throw invalidElementException(("invalid chemical period: " + period).c_str());
  return *this;
}

Element &Element::setBlock(int block) {
  if (block < 0 || static_cast<size_t>(block) >= BLOCKS.size()) {
    throw invalidElementException(
        ("invalid chemistry block: " + std::to_string(block)).c_str());
  }
  this->block = BLOCKS[static_cast<size_t>(block)];
  return *this;
}

Element &Element::setBlock(char block) {
  if (std::find(BLOCKS.begin(), BLOCKS.end(), block) == BLOCKS.end()) {
    throw invalidElementException(
        ("invalid chemistry block: " + std::string(1, block)).c_str());
  }
  this->block = block;
  return *this;
}

Element &Element::setSeries(int series) {
  if (SERIES.find(series) == SERIES.end()) {
    throw invalidElementException(
        ("invalid chemical series: " + std::to_string(series)).c_str());
  }
  this->series = series;
  return *this;
}

Element &Element::setSeries(const std::string &series) {
  for (const auto &entry : SERIES) {
    if (entry.second == series) {
      this->series = entry.first;
      return *this;
    }
  }
  throw invalidElementException(("invalid chemical series: " + series).c_str());
  return *this;
}

Element &Element::setMolarMass(double mass) {
  if (mass <= 0.) {
    throw invalidElementException(
        ("invalid molar mass: " + std::to_string(mass)).c_str());
  }
  this->molarMass = mass;
  return *this;
}

Element &Element::setElectronegativity(double electronegativity) {
  this->electronegativity = electronegativity;
  return *this;
}

Element &Element::setElectronAffinity(double electronAffinity) {
  this->electronAffinity = electronAffinity;
  return *this;
}

Element &Element::setCovalentRadius(double covalentRadius) {
  if (covalentRadius <= 0.) {
    throw invalidElementException(
        ("invalid covalent radius: " + std::to_string(covalentRadius)).c_str());
  }
  this->covalentRadius = covalentRadius;
  return *this;
}

Element &Element::setAtomicRadius(double atomicRadius) {
  if (atomicRadius <= 0.) {
    throw invalidElementException(
        ("invalid atomic radius: " + std::to_string(atomicRadius)).c_str());
  }
  this->atomicRadius = atomicRadius;
  return *this;
}

Element &Element::setVanDerWaalsRadius(double vdwRadius) {
  if (vdwRadius <= 0.) {
    throw invalidElementException(
        ("invalid van der Waals radius: " + std::to_string(vdwRadius)).c_str());
  }
  this->vdwRadius = vdwRadius;
  return *this;
}

Element &Element::setBoilingPoint(double tboil) {
  this->boilingPoint = (tboil > 0.) ? tboil : std::abs(tboil);
  return *this;
}

Element &Element::setMeltingPoint(double tmelt) {
  this->meltingPoint = (tmelt > 0.) ? tmelt : std::abs(tmelt);
  return *this;
}

Element &Element::setDensity(double density) {
  this->density = density;
  return *this;
}

Element &Element::setElectronConfiguration(const std::string &config) {
  electronConfig = config;
  return *this;
}

Element &Element::setOxidationStates(const std::string &oxidationStates) {
  this->oxidationStates = oxidationStates;
  return *this;
}

Element &Element::setIonisationEnergies(const std::vector<double> &ionEnergies) {
  ionisationEnergies = ionEnergies;
  return *this;
}

Element &Element::setDescription(const std::string &description) {
  this->description = description;
  return *this;
}
