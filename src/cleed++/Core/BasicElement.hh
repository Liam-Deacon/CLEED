/*********************************************************************
 *                      BasicElement.h
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
 * Provides a base chemical element class.
 */

#ifndef __BASICELEMENT_HH__
#define __BASICELEMENT_HH__

#ifdef __cplusplus

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <exception>

namespace cleed {

class invalidElementException : public std::exception {
  public:
    invalidElementException(const char *msg) : err_msg(msg) {};
    virtual ~invalidElementException() throw() {};
    virtual const char *what() const throw() { return this->err_msg.c_str(); };

  private:
    std::string err_msg;
};

class BasicElement {
  public:
    typedef enum ATOMIC_NUMBERS {
      HYDROGEN=1,
      HELIUM=2,
      LITHIUM=3,
      BERYLLIUM=4,
      BORON=5,
      CARBON=6,
      NITROGEN=7,
      OXYGEN=8,
      FLUORINE=9,
      NEON=10,
      SODIUM=11,
      MAGNESIUM=12,
      ALUMINIUM=13,
      SILICON=14,
      PHOSPHORUS=15,
      SULFUR=16,
      CHLORINE=17,
      ARGON=18,
      POTASSIUM=19,
      CALCIUM=20,
      SCANDIUM=21,
      TITANIUM=22,
      VANADIUM=23,
      CHROMIUM=24,
      MANGANESE=25,
      IRON=26,
      COBALT=27,
      NICKEL=28,
      COPPER=29,
      ZINC=30,
      GALLIUM=31,
      GERMANIUM=32,
      ARSENIC=33,
      SELENIUM=34,
      BROMINE=35,
      KRYPTON=36,
      RUBIDIUM=37,
      STRONTIUM=38,
      YTTRIUM=39,
      ZIRCONIUM=40,
      NIOBIUM=41,
      MOLYBDENUM=42,
      TECHNETIUM=43,
      RUTHENIUM=44,
      RHODIUM=45,
      PALLADIUM=46,
      SILVER=47,
      CADMIUM=48,
      INDIUM=49,
      TIN=50,
      ANTIMONY=51,
      TELLURIUM=52,
      IODINE=53,
      XENON=54,
      CAESIUM=55,
      BARIUM=56,
      LANTHANUM=57,
      CERIUM=58,
      PRASEODYMIUM=59,
      NEODYMIUM=60,
      PROMETHIUM=61,
      SAMARIUM=62,
      EUROPIUM=63,
      GADOLINIUM=64,
      TERBIUM=65,
      DYSPROSIUM=66,
      HOLMIUM=67,
      ERBIUM=68,
      THULIUM=69,
      YTTERBIUM=70,
      LUTETIUM=71,
      HAFNIUM=72,
      TANTALUM=73,
      TUNGSTEN=74,
      RHENIUM=75,
      OSMIUM=76,
      IRIDIUM=77,
      PLATINUM=78,
      GOLD=79,
      MERCURY=80,
      THALLIUM=81,
      LEAD=82,
      BISMUTH=83,
      POLONIUM=84,
      ASTATINE=85,
      RADON=86,
      FRANCIUM=87,
      RADIUM=88,
      ACTINIUM=89,
      THORIUM=90,
      PROTACTINIUM=91,
      URANIUM=92,
      NEPTUNIUM=93,
      PLUTONIUM=94,
      AMERICIUM=95,
      CURIUM=96,
      BERKELIUM=97,
      CALIFORNIUM=98,
      EINSTEINIUM=99,
      FERMIUM=100,
      MENDELEVIUM=101,
      NOBELIUM=102,
      LAWRENCIUM=103,
      RUTHERFORDIUM=104,
      DUBNIUM=105,
      SEABORGIUM=106,
      BOHRIUM=107,
      HASSIUM=108,
      MEITNERIUM=109,
      DARMSTADTIUM=110,
      ROENTGENIUM=111,
      COPERNICIUM=112
    } atomicNumber;

    static std::map<const std::string, const unsigned int> NAMES;
    static std::map<const std::string, const unsigned int> SYMBOLS;
    static std::map<const std::string, const BasicElement> ELEMENTS;

    BasicElement();
    BasicElement(unsigned int Z);
    BasicElement(const std::string &name);
    virtual ~BasicElement();

    virtual bool operator!();
    virtual BasicElement &operator=(std::string id);
    virtual BasicElement &operator=(int Z);
    virtual BasicElement &operator-(int Z);
    virtual BasicElement &operator+(int Z);
    virtual bool operator==(const BasicElement &other);
    virtual bool operator==(const std::string &element);
    virtual bool operator!=(const BasicElement &other);
    virtual bool operator!=(const std::string &element);
    virtual bool operator<(const BasicElement &other);
    virtual bool operator>(const BasicElement &other);
    virtual bool operator<=(const BasicElement &other);
    virtual bool operator>=(const BasicElement &other);

    int getAtomicNumber() const;
    const std::string &getName() const;
    const std::string &getSymbol() const;

    BasicElement &setAtomicNumber(size_t Z);
    BasicElement &setName(const std::string &name);
    BasicElement &setSymbol(const std::string &symbol);

  private:
    unsigned int Z;
    std::string name;
    std::string symbol;
};


} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __BASICELEMENT_HH__ */
