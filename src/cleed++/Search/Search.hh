/*********************************************************************
 *                      Search.h
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
 * 
 * Provides an object-orientated wrapper to the low-level #search structure and
 * its associated C functions.
 */

#ifndef __Search_h__
#define __Search_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */ 

#include <string>
#include <cstddef>
#include <vector>

#include "search_def.h"
#include "search_func.h"
#include <LEED/LEED.hh>
#include <RFactor/RFactor.hh>

namespace cleed {

class Search {
  public:
    /* constructors and destructor */
    Search(const string &control_filepath,
           const string &theory_filepath,
           search_method driver=SR_SIMPLEX);
    ~Search();

    /* operators */


    /* getters */
    std::size_t getNumberOfIndependentParameters() const;
    std::size_t getNumberOfGeometricalParameters() const;

    // coordinate-related
    double getMinimumX() const;
    double getMinimumY() const;
    double getMinimumZ() const;
    double getMaximumX() const;
    double getMaximumY() const;
    double getMaximumZ() const;

    std::vector<double> getBasis() const;

    // angle-related
    bool isAngleSearch() const;
    std::size_t getNumberOfThetaParameters() const;
    std::size_t getNumberOfPhiParameters() const;
    double getInitialThetaValue() const;
    double getInitialPhiValue() const;

    // symmetry-related
    bool isZOnlySearch() const;
    std::size_t getNumberOfRotationalSymmetries() const;
    std::vector<double> getRotationalAxis() const;
    std::vector<double> getMirrorPlanePoint() const;
    std::vector<double> getMirrorPlaneDirection() const;

    // R-factor
    string getRFactorType() const;
    double getRFactorShiftRange() const;

    const RFactor *getRFactor() const;

    /* setters */
    void setNumberOfIndependentParameters(std::size_t n_par);
    void setNumberOfGeometricalParameters(std::size_t n_geo);

    // coordinate-related
    void setMinimumX(double x_min);
    void setMinimumY(double y_min);
    void setMinimumZ(double z_min);
    void setMaximumX(double x_max);
    void setMaximumY(double y_max);
    void setMaximumZ(double z_max);

    void setBasis(std::vector<double> basis);

    // angle-related
    void setAngleSearch(bool angle);
    void setNumberOfThetaParameters(std::size_t n_theta);
    void setNumberOfPhiParameters(std::size_t n_phi);
    void setInitialThetaValue(double theta_0);
    void setInitialPhiValue(double phi_0);

    // symmetry-related
    void setZOnlySearch(bool z_only);
    void setNumberOfRotationalSymmetries(std::size_t n_rot);
    void setRotationalAxis(std::vector<double> rot_axis);
    void setMirrorPlanePoint(std::vector<double> mir_point);
    void setMirrorPlaneDirection(std::vector<double> mir_dir);

    // R-factor
    void setRFactorType(const std::string &type);
    void setRFactorType(const char *type);
    void setRFactorShiftRange(double range);

    void setRFactor(const RFactor &rfac);

    void setTheoryFilePath(const std::string &theory_filepath);
    void setControlFilePath(const std::string &control_filepath);

    /** Functions **/
    virtual void startSearch();
    virtual void stopSearch();
    virtual void resumeSearch();

  private:
    search c_search;
    RFactor *rfactor_ptr; // pointer to facilitate polymorphic behavior
    LEED *leed_ptr;
    std::string theory_filepath;
    std::string control_filepath;
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Search_h__ */
