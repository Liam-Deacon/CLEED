/*********************************************************************
 *                      Element.h
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

#ifndef __Element_h__
#define __Element_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */ 

#include "Core/BasicElement.h"
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace cleed {

class Element : public BasicElement {
  public:
    static const std::map<int, char> PERIODS;
    static const std::vector<char> BLOCKS;
    static const std::map<int, std::string> SERIES;
    static const std::map<int, std::tuple<std::string, std::string>> GROUPS;
    static const std::map<int, std::string> DESCRIPTIONS;

    Element(int Z, const std::string &symbol, const std::string &name);
    virtual ~Element();

    int getGroup() const;
    int getPeriod() const;
    char getBlock() const;
    int getSeries() const;
    double getMolarMass() const;
    double getElectronegivity() const;
    double getElectronAffinity() const;
    double getCovalentRadius() const;
    double getAtomicRadius() const;
    double getVanDerWaalsRadius() const;
    double getBoilingPoint() const;
    double getMeltingPoint() const;
    double getDensity() const;
    const std::string &getElectronConfiguration() const;
    std::string &getOxidationStates() const;
    std::vector<double> &getIonisationEnergies() const;
    const std::string &getDescription() const;

    Element &setGroup(int group);
    Element &setGroup(const std::string &group);
    Element &setPeriod(int period);
    Element &setPeriod(const std::string &period);
    Element &setBlock(int block);
    Element &setBlock(char block);
    Element &setSeries(int series);
    Element &setSeries(const std::string &series);
    Element &setMolarMass(double mass);
    Element &setElectronegativity(double electronegativity);
    Element &setElectronAffinity(double electronAffinity);
    Element &setCovalentRadius(double covalentRadius);
    Element &setAtomicRadius(double atomicRadius);
    Element &setVanDerWaalsRadius(double vdwRadius);
    Element &setBoilingPoint(double tboil);
    Element &setMeltingPoint(double tmelt);
    Element &setDensity(double density);
    Element &setElectronConfiguration(const std::string &config);
    Element &setOxidationStates(const std::string &oxidationStates);
    Element &setIonisationEnergies(const std::vector<double> &ionEnergies);
    Element &setDescription(const std::string &description);

  private:
    int group;
    int period;
    char block;
    int series;
    double molarMass;
    double electronegativity;
    double electronAffinity;
    double covalentRadius;
    double atomicRadius;
    double vdwRadius;
    double boilingPoint;
    double meltingPoint;
    double density;
    double relativeAbundance;
    std::string eletronConfig;
    std::string oxidationStates;
    std::vector<double> ionisationEnergies;
    std::string description;
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Element_h__ */
