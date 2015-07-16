/*********************************************************************
 *                      Element.cc
 *
 *  Copyright 2013-2015 Liam Deacon <liam.deacon@diamond.ac.uk>
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

#include <Element.h>

using namespace cleed;

const std::map<int, char> Element::PERIODS = {
        {1, 'K'}, {2, 'L'}, {3, 'M'}, {4, 'N'}, {5, 'O'}, {6, 'P'}, {7, 'Q'}
};

const std::vector<char> Element::BLOCKS = {'s', 'g', 'f', 'd', 'p'};

const std::map<int, std::tuple<std::string, std::string>> GROUPS = {
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

static const std::map<int, char> PERIODS;
static const std::vector<char> BLOCKS;
static const std::map<int, std::string> SERIES;
static const std::map<int, std::tuple<std::string, std::string>> GROUPS;
static const std::map<int, std::string> DESCRIPTIONS;

Element::Element(int Z, const std::string &symbol, const std::string &name);
virtual Element::~Element();

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
  return const_cast<std::string&>(electronConfig);
}

inline std::string &Element::getOxidationStates() const {
  return oxidationStates;
}

inline std::vector<double> &Element::getIonisationEnergies() const {
  return ionisationEnergies;
}

inline const std::string &Element::getDescription() const {
  return const_cast<std::string&>(description);
}

inline Element &Element::setGroup(int group) {
  this->group = (group > 0 && group <= GROUPS.size()) ? group :
    throw(invalidElementException("invalid chemical group: " +
                                  std::string(group)));
  return *this;
}

Element &Element::setGroup(const std::string &group) {
  this->group = (group > 0 && group <= GROUPS.size()) ? group :
    throw(invalidElementException("invalid chemical group: " + group));
  return *this;
}

Element &Element::setPeriod(int period) {
  this->period = (PERIODS[period] > 0) ? PERIODS[period] :
      throw(invalidElementException("invalid chemical period: " + period));
    return *this;
}

Element &Element::setPeriod(const std::string &period) {
  this->period = (PERIODS[period] > 0) ? PERIODS[period] :
    throw(invalidElementException("invalid chemical period: " + period));
  return *this;
}

Element &Element::setBlock(int block) {
  this->block = (BLOCKS[block] > 0) ? BLOCKS[block] :
    throw(invalidElementException("invalid chemistry block: " +
                                  std::string(block)));
  return *this;
}

Element &Element::setBlock(char block) {
  this->block = (BLOCKS[block] > 0) ? BLOCKS[block] :
      throw(invalidElementException("invalid chemistry block: " +
                                    std::string(block)));
    return *this;
}

Element &Element::setSeries(int series) {
  this->series = (SERIES[series] > 0) ? SERIES[series] :
      throw(invalidElementException("invalid chemical series: " +
                                    std::string(series)));
  return *this;
}

Element &Element::setSeries(const std::string &series) {
  this->series = (SERIES[series] > 0) ? SERIES[series] :
      throw(invalidElementException("invalid chemical series: " + series));
  return *this;
}

Element &Element::setMolarMass(double mass) {
  this->molarMass = (mass > 0.) ? mass :
    throw(invalidElementException("invalid molar mass: " + std::string(mass)));
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
  this->covalentRadius = (covalentRadius > 0.) ? covalentRadius :
    throw(invalidElementException("invalid covalent radius: " +
                                  std::string(covalentRadius)));
  return *this;
}

Element &Element::setAtomicRadius(double atomicRadius) {
  this->atomicRadius = (atomicRadius > 0.) ? atomicRadius :
    throw(invalidElementException("invalid atomic radius: " +
                                  std::string(atomicRadius)));
  return *this;
}

Element &Element::setVanDerWaalsRadius(double vdwRadius) {
  this->vdwRadius = (vdwRadius > 0.) ? vdwRadius :
    throw(invalidElementException("invalid van der Waals radius: " +
                                  std::string(vdwRadius)));
  return *this;
}

Element &Element::setBoilingPoint(double tboil) {
  this->boilingPoint = (tboil > 0.) ? tboil : abs(tboil);
}

Element &Element::setMeltingPoint(double tmelt) {
  this->meltingPoint = (tmelt > 0.) ? tmelt : abs(tmelt);
}

Element &Element::setDensity(double density) {
 this->density = density;
}

Element &Element::setElectronConfiguration(const std::string &config) {

}

Element &Element::setOxidationStates(const std::string &oxidationStates) {

}

Element &Element::setIonisationEnergies(const std::vector<double> &ionEnergies) {

}

Element &Element::setDescription(const std::string &description) {

}
