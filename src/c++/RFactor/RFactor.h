/*********************************************************************
 *													Rfactor.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/9 Dec 2014 - creation
 *********************************************************************/

/*!
 * \file Rfactor.h
 * \author Liam Deacon
 * \date 9 Dec 2014
 *
 * Provides object orientated interface for rfac and associated structs/functions.
 */


#ifndef RFACTOR_H
#define RFACTOR_H

#ifdef __cplusplus

#include <cstddef>
#include <vector>
#include <string>

#include "rfac_def.h"
#include "rfac_func.h"

#include "IVCurvePair.h"
#include "Rfactor/RFactorArgs.h"

/* use std items in local namespace */
using std::vector;
using std::size_t;
using std::string;

/* mangle global rfac functions to local namespace */
using ::rfac_rmin;

namespace cleed {

/* classes */

class Search;   // forward declaration

class RFactor {

  friend Search;
  friend IVCurve;

  public:
    /* constructors & destructor */
    RFactor(const string &control_filepath,
            const string &theory_filepath);
    //RFactor(const cleed::RFactorArgs &args);

    virtual ~RFactor();

    /* operators */
    bool operator==(const RFactor &other) const;
    bool operator!=(const RFactor &other) const;

    /* setters */
    virtual void calculateRFactor();
    virtual void updateIVs(const string &control_filepath, const string &theory_filepath);
    virtual void updateIVs(const vector<IVCurvePair> &ivs);

    /* getters */
    double getRFactor() const;
    double getRR() const;

    double getTotalEnergyRange() const;
    double getTheoryToExperimentalIntegralRatio() const;
    double getMinimumEnergyShift() const;

  private:
    vector<IVCurvePair> iv_datasets;
    RFactorArgs args;
    double rfactor = -1.;
    double relativeError = -1.;
    double iRatio = 0.;
    double energyShift = 0.;
    double energyRange = 0.;
};

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* RFACTOR_H */
