/*********************************************************************
 *													RFactorArgs.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file RFactorArgs.h
 * \author Liam Deacon
 * \date 17 Dec 2014
 */


#ifndef __RFactorArgs_H__
#define __RFactorArgs_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <string>
#include <memory>
#include <cstdlib>

#include "rfac_func.h"

using ::rfac_rdargs;
using ::rfac_args;

using std::string;
using std::size_t;
using std::free;

namespace cleed {


class RFactorArgs : public rfac_args {
  public:
    /* constructors & destructor */
    RFactorArgs(const RFactorArgs &other);
    RFactorArgs(int argc, char *argv[]);
    RFactorArgs(const rfac_args *args_ptr);
    RFactorArgs(const string &ctrFile, const string &theoryFile,
        rfactor_type type=RP_FACTOR, double initialEnergyShift=-10.,
        double finalEnergyShift=10., double energyShiftStep=0.5, double vi=4.,
        bool allGroups=true, const char *ivOutputFile=nullptr,
        const char *outputFile=nullptr);

    ~RFactorArgs();

    /* operators */
    RFactorArgs &operator=(const RFactorArgs &other);
    RFactorArgs &operator=(const rfac_args *args_ptr);

    /* getters */
    string getControlFile() const;
    string getTheoryFile() const;
    string getOutputFile() const;
    string getIVOutputFile() const;

    rfactor_type getRFactorType() const;
    double getInitialEnergyShift() const;
    double getFinalEnergyShift() const;
    double getEnergyShiftStep() const;
    double getOpticalPotential() const;
    bool getAllGroups() const;

    /* setters */
    void mkiv_set_controlFile(const string &ctrFile);
    void setTheoryFile(const string &theoryFile);
    void setOutputFile(const string &outputFile);
    void setIVOutputFile(const string &ivOutputFile);

    void setRFactorType(rfactor_type type);
    void setInitialEnergyShift(double dE0);
    void setFinalEnergyShift(double dEf);
    void setEnergyShiftStep(double dEs);
    void setOpticalPotential(double vi);
    void setAllGroups(bool allGroups);

  private:
    rfac_args c_args;
};

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __RFactorArgs_H__ */
