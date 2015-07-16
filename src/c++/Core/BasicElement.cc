/*********************************************************************
 *                      BasicElement.cc
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
 * Implementation file for base element class.
 */

#include "Core/BasicElement.h"

using namespace cleed;

const std::map<std::string, BasicElement::BasicElement>
BasicElement::ELEMENTS = {
  {"Hydrogen", BasicElement(1)},
  {"Helium", BasicElement(2)},
  {"Lithium", BasicElement(3)},
  {"Beryllium", BasicElement(4)},
  {"Boron", BasicElement(5)},
  {"Carbon", BasicElement(6)},
  {"Nitrogen", BasicElement(7)},
  {"Oxygen", BasicElement(8)},
  {"Fluorine", BasicElement(9)},
  {"Neon", BasicElement(10)},
  {"Sodium", BasicElement(11)},
  {"Magnesium", BasicElement(12)},
  {"Aluminium", BasicElement(13)},
  {"Silicon", BasicElement(14)},
  {"Phosphorus", BasicElement(15)},
  {"Sulfur", BasicElement(16)},
  {"Chlorine", BasicElement(17)},
  {"Argon", BasicElement(18)},
  {"Potassium", BasicElement(19)},
  {"Calcium", BasicElement(20)},
  {"Scandium", BasicElement(21)},
  {"Titanium", BasicElement(22)},
  {"Vanadium", BasicElement(23)},
  {"Chromium", BasicElement(24)},
  {"Manganese", BasicElement(25)},
  {"Iron", BasicElement(26)},
  {"Cobalt", BasicElement(27)},
  {"Nickel", BasicElement(28)},
  {"Copper", BasicElement(29)},
  {"Zinc", BasicElement(30)},
  {"Gallium", BasicElement(31)},
  {"Germanium", BasicElement(32)},
  {"Arsenic", BasicElement(33)},
  {"Selenium", BasicElement(34)},
  {"Bromine", BasicElement(35)},
  {"Krypton", BasicElement(36)},
  {"Rubidium", BasicElement(37)},
  {"Strontium", BasicElement(38)},
  {"Yttrium", BasicElement(39)},
  {"Zirconium", BasicElement(40)},
  {"Niobium", BasicElement(41)},
  {"Molybdenum", BasicElement(42)},
  {"Technetium", BasicElement(43)},
  {"Ruthenium", BasicElement(44)},
  {"Rhodium", BasicElement(45)},
  {"Palladium", BasicElement(46)},
  {"Silver", BasicElement(47)},
  {"Cadmium", BasicElement(48)},
  {"Indium", BasicElement(49)},
  {"Tin", BasicElement(50)},
  {"Antimony", BasicElement(51)},
  {"Tellurium", BasicElement(52)},
  {"Iodine", BasicElement(53)},
  {"Xenon", BasicElement(54)},
  {"Caesium", BasicElement(55)},
  {"Barium", BasicElement(56)},
  {"Lanthanum", BasicElement(57)},
  {"Cerium", BasicElement(58)},
  {"Praseodymium", BasicElement(59)},
  {"Neodymium", BasicElement(60)},
  {"Promethium", BasicElement(61)},
  {"Samarium", BasicElement(62)},
  {"Europium", BasicElement(63)},
  {"Gadolinium", BasicElement(64)},
  {"Terbium", BasicElement(65)},
  {"Dysprosium", BasicElement(66)},
  {"Holmium", BasicElement(67)},
  {"Erbium", BasicElement(68)},
  {"Thulium", BasicElement(69)},
  {"Ytterbium", BasicElement(70)},
  {"Lutetium", BasicElement(71)},
  {"Hafnium", BasicElement(72)},
  {"Tantalum", BasicElement(73)},
  {"Tungsten", BasicElement(74)},
  {"Rhenium", BasicElement(75)},
  {"Osmium", BasicElement(76)},
  {"Iridium", BasicElement(77)},
  {"Platinum", BasicElement(78)},
  {"Gold", BasicElement(79)},
  {"Mercury", BasicElement(80)},
  {"Thallium", BasicElement(81)},
  {"Lead", BasicElement(82)},
  {"Bismuth", BasicElement(83)},
  {"Polonium", BasicElement(84)},
  {"Astatine", BasicElement(85)},
  {"Radon", BasicElement(86)},
  {"Francium", BasicElement(87)},
  {"Radium", BasicElement(88)},
  {"Actinium", BasicElement(89)},
  {"Thorium", BasicElement(90)},
  {"Protactinium", BasicElement(91)},
  {"Uranium", BasicElement(92)},
  {"Neptunium", BasicElement(93)},
  {"Plutonium", BasicElement(94)},
  {"Americium", BasicElement(95)},
  {"Curium", BasicElement(96)},
  {"Berkelium", BasicElement(97)},
  {"Californium", BasicElement(98)},
  {"Einsteinium", BasicElement(99)},
  {"Fermium", BasicElement(100)},
  {"Mendelevium", BasicElement(101)},
  {"Nobelium", BasicElement(102)},
  {"Lawrencium", BasicElement(103)},
  {"Rutherfordium", BasicElement(104)},
  {"Dubnium", BasicElement(105)},
  {"Seaborgium", BasicElement(106)},
  {"Bohrium", BasicElement(107)},
  {"Hassium", BasicElement(108)},
  {"Meitnerium", BasicElement(109)},
  {"Darmstadtium", BasicElement(110)},
  {"Roentgenium", BasicElement(111)},
  {"Copernicium", BasicElement(112)}
};

const std::map<std::string, int> BasicElement::NAMES = {
  {"Hydrogen", 1},
  {"Helium", 2},
  {"Lithium", 3},
  {"Beryllium", 4},
  {"Boron", 5},
  {"Carbon", 6},
  {"Nitrogen", 7},
  {"Oxygen", 8},
  {"Fluorine", 9},
  {"Neon", 10},
  {"Sodium", 11},
  {"Magnesium", 12},
  {"Aluminium", 13},
  {"Silicon", 14},
  {"Phosphorus", 15},
  {"Sulfur", 16},
  {"Chlorine", 17},
  {"Argon", 18},
  {"Potassium", 19},
  {"Calcium", 20},
  {"Scandium", 21},
  {"Titanium", 22},
  {"Vanadium", 23},
  {"Chromium", 24},
  {"Manganese", 25},
  {"Iron", 26},
  {"Cobalt", 27},
  {"Nickel", 28},
  {"Copper", 29},
  {"Zinc", 30},
  {"Gallium", 31},
  {"Germanium", 32},
  {"Arsenic", 33},
  {"Selenium", 34},
  {"Bromine", 35},
  {"Krypton", 36},
  {"Rubidium", 37},
  {"Strontium", 38},
  {"Yttrium", 39},
  {"Zirconium", 40},
  {"Niobium", 41},
  {"Molybdenum", 42},
  {"Technetium", 43},
  {"Ruthenium", 44},
  {"Rhodium", 45},
  {"Palladium", 46},
  {"Silver", 47},
  {"Cadmium", 48},
  {"Indium", 49},
  {"Tin", 50},
  {"Antimony", 51},
  {"Tellurium", 52},
  {"Iodine", 53},
  {"Xenon", 54},
  {"Caesium", 55},
  {"Barium", 56},
  {"Lanthanum", 57},
  {"Cerium", 58},
  {"Praseodymium", 59},
  {"Neodymium", 60},
  {"Promethium", 61},
  {"Samarium", 62},
  {"Europium", 63},
  {"Gadolinium", 64},
  {"Terbium", 65},
  {"Dysprosium", 66},
  {"Holmium", 67},
  {"Erbium", 68},
  {"Thulium", 69},
  {"Ytterbium", 70},
  {"Lutetium", 71},
  {"Hafnium", 72},
  {"Tantalum", 73},
  {"Tungsten", 74},
  {"Rhenium", 75},
  {"Osmium", 76},
  {"Iridium", 77},
  {"Platinum", 78},
  {"Gold", 79},
  {"Mercury", 80},
  {"Thallium", 81},
  {"Lead", 82},
  {"Bismuth", 83},
  {"Polonium", 84},
  {"Astatine", 85},
  {"Radon", 86},
  {"Francium", 87},
  {"Radium", 88},
  {"Actinium", 89},
  {"Thorium", 90},
  {"Protactinium", 91},
  {"Uranium", 92},
  {"Neptunium", 93},
  {"Plutonium", 94},
  {"Americium", 95},
  {"Curium", 96},
  {"Berkelium", 97},
  {"Californium", 98},
  {"Einsteinium", 99},
  {"Fermium", 100},
  {"Mendelevium", 101},
  {"Nobelium", 102},
  {"Lawrencium", 103},
  {"Rutherfordium", 104}
  {"Dubnium", 105},
  {"Seaborgium", 106},
  {"Bohrium", 107},
  {"Hassium", 108},
  {"Meitnerium", 109},
  {"Darmstadtium", 110},
  {"Roentgenium", 111},
  {"Copernicium", 112}
};

const std::map<std::string, int> BasicElement::SYMBOLS = {
  {"H", 1},
  {"He", 2},
  {"Li", 3},
  {"Be", 4},
  {"B", 5},
  {"C", 6},
  {"N", 7},
  {"O", 8},
  {"F", 9},
  {"Ne", 10},
  {"Na", 11},
  {"Mg", 12},
  {"Al", 13},
  {"Si", 14},
  {"P", 15},
  {"S", 16},
  {"Cl", 17},
  {"Ar", 18},
  {"K", 19},
  {"Ca", 20},
  {"Sc", 21},
  {"Ti", 22},
  {"V", 23},
  {"Cr", 24},
  {"Mn", 25},
  {"Fe", 26},
  {"Co", 27},
  {"Ni", 28},
  {"Cu", 29},
  {"Zn", 30},
  {"Ga", 31},
  {"Ge", 32},
  {"As", 33},
  {"Se", 34},
  {"Br", 35},
  {"Kr", 36},
  {"Rb", 37},
  {"Sr", 38},
  {"Y", 39},
  {"Zr", 40},
  {"Nb", 41},
  {"Mo", 42},
  {"Tc", 43},
  {"Ru", 44},
  {"Rh", 45},
  {"Pd", 46},
  {"Ag", 47},
  {"Cd", 48},
  {"In", 49},
  {"Sn", 50},
  {"Sb", 51},
  {"Te", 52},
  {"I", 53},
  {"Xe", 54},
  {"Cs", 55},
  {"Ba", 56},
  {"La", 57},
  {"Ce", 58},
  {"Pr", 59},
  {"Nd", 60},
  {"Pm", 61},
  {"Sm", 62},
  {"Eu", 63},
  {"Gd", 64},
  {"Tb", 65},
  {"Dy", 66},
  {"Ho", 67},
  {"Er", 68},
  {"Tm", 69},
  {"Yb", 70},
  {"Lu", 71},
  {"Hf", 72},
  {"Ta", 73},
  {"W", 74},
  {"Re", 75},
  {"Os", 76},
  {"Ir", 77},
  {"Pt", 78},
  {"Au", 79},
  {"Hg", 80},
  {"Tl", 81},
  {"Pb", 82},
  {"Bi", 83},
  {"Po", 84},
  {"At", 85},
  {"Rn", 86},
  {"Fr", 87},
  {"Ra", 88},
  {"Ac", 89},
  {"Th", 90},
  {"Pa", 91},
  {"U", 92},
  {"Np", 93},
  {"Pu", 94},
  {"Am", 95},
  {"Cm", 96},
  {"Bk", 97},
  {"Cf", 98},
  {"Es", 99},
  {"Fm", 100},
  {"Md", 101},
  {"No", 102},
  {"Lr", 103},
  {"Rf", 104},
  {"Db", 105},
  {"Sg", 106},
  {"Bh", 107},
  {"Hs", 108},
  {"Mt", 109},
  {"Ds", 110},
  {"Rg", 111},
  {"Cn", 112}
};

BasicElement::BasicElement() {
  setAtomicNumber(1); // default to Hydrogen (most abundant element anyway)
}

BasicElement::BasicElement(int Z) {
  setAtomicNumber(Z);
}

BasicElement::BasicElement(const std::string &name) {
  int Z = NAMES[name] > 0 ? NAMES[name] : SYMBOLS[name];
  setAtomicNumber(Z);
}

BasicElement::~BasicElement() {

}

inline bool BasicElement::operator!() {
  return (this != nullptr);
}

BasicElement &BasicElement::operator=(const BasicElement &other) {
  return &other;
}

BasicElement &BasicElement::operator=(int Z) {
  return setAtomicNumber(Z);
}

BasicElement &BasicElement::operator-(int Z) {
  return setAtomicNumber(getAtomicNumber() - Z);
}

BasicElement &BasicElement::operator+(int Z) {
  return setAtomicNumber(getAtomicNumber() + Z);
}

inline bool BasicElement::operator==(const BasicElement &other) {
  return (this == other);
}

inline bool BasicElement::operator==(const std::string &element) {
  return (this->getName() == element || this->getSymbol() == element);
}

inline bool BasicElement::operator!=(const BasicElement &other) {
  return (this->getAtomicNumber() >= other.atomicNumber);
}

inline bool BasicElement::operator!=(const std::string &element) {
  return !(this == element);
}

inline bool BasicElement::operator<(const BasicElement &other) {
  (this->getAtomicNumber() < other.atomicNumber);
}

inline bool BasicElement::operator>(const BasicElement &other) {
  (this->getAtomicNumber() > other.atomicNumber);
}

inline bool BasicElement::operator<=(const BasicElement &other) {
  return (this->getAtomicNumber() <= other.atomicNumber);
}

inline bool BasicElement::operator>=(const BasicElement &other) {
  return (this->getAtomicNumber() >= other.atomicNumber);
}

inline int BasicElement::getAtomicNumber() const {
  return atomicNumber;
}

inline const std::string &BasicElement::getName() const {
  return const_cast<std::string&>(name);
}

inline const std::string &BasicElement::getSymbol() const {
  return const_cast<std::string&>(symbol);
}

inline BasicElement &BasicElement::setAtomicNumber(int Z) {
  return (Z > 0 && Z < ELEMENTS.size()) ? &ELEMENTS(Z) :
    throw invalidElementException("Atomic number Z is not valid");
}

BasicElement &BasicElement::setName(const std::string &name) {
  return setAtomicNumber(static_cast<int>(NAMES[name]));
}

BasicElement &BasicElement::setSymbol(const std::string &symbol) {
  return setAtomicNumber(static_cast<int>(SYMBOLS[symbol]));
}
