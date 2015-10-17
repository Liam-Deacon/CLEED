/*********************************************************************
 *                      Crystal.h
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
 * Defines an object orientated wrapper class for leed_crystal structure.
 */

#ifndef __Crystal_h__
#define __Crystal_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include <vector>
#include <string>
#include <cstddef>
#include "leed_def.h"

using std::size_t;
using std::string;
using std::vector;

namespace cleed {

class Crystal : public leed_crystal {
  public:
    /* constructors and destructor */
    Crystal();
    Crystal(const leed_crystal *crystal_ptr);
    Crystal(const Crystal &crystal);
    ~Crystal();

    /* operators */
    bool operator==(const Crystal &other) const;
    bool operator!=(const Crystal &other) const;
    bool operator==(const leed_crystal *other) const;
    bool operator!=(const leed_crystal *other) const;

    /* getters */
    double getRealOpticalPotential() const;
    double getImaginaryOpticalPotential() const;
    double getTemperature() const;

    size_t getNumberOfRotationalSymmetries() const;
    vector<double> getAxisOfRotationalSymmetry() const;

    size_t getNumberOfMirrorPlanes() const;
    const real *getMirrorPlaneGeometryPtr() const;
    vector<double> &getMirrorPlaneGeometry() const;
    leed_mirror_sym getMirrorSymmetry() const;
    const real *getAngleAlphaDegreesPtr() const;
    vector<double> &getAngleAlphaDegrees() const;

    vector<double> getSubstrateBasisVectors() const;
    double getSubstrateUnitCellArea() const;
    vector<double> getSuperstructureBasisVectors() const;
    double getSuperstructureRelativeUnitCellArea() const;

    size_t getNumberOfLayers() const;
    const leed_layer *getLayersPtr() const;
    vector<leed_layer> &getLayers() const;
    double getMinimumInterlayerDistance() const;
    size_t getNumberOfAtoms() const;
    size_t getNumberOfTypes() const;

    std::vector<string> getComments() const;

    /* setters */
    void setRealOpticalPotential(double vr);
    void setImaginaryOpticalPotential(double vi);
    void setTemperature(double temp);

    void setNumberOfRotationalSymmetries(size_t n_rot);
    void setAxisOfRotationalSymmetry(vector<double> rot_axis);
    void setAxisOfRotationalSymmetry(double *rot_axis, size_t n);

    void setNumberOfMirrorPlanes(size_t n_mir);
    void setMirrorPlaneGeometry(vector<double> m_plane);
    void setMirrorPlaneGeometry(double *m_plane, size_t n);
    void setMirrorSymmetry(leed_mirror_sym symmetry);
    void setAngleAlphaDegrees(vector<double> alpha);
    void setAngleAlphaDegrees(double *alpha, size_t n);

    void setSubstrateBasisVectors(vector<double> a);
    void setSubstrateUnitCellArea(double area);
    void setSuperstructureBasisVectors(vector<double> b);
    void setSuperstructureRelativeUnitCellArea(double rel_area);

    void setNumberOfLayers(size_t n_layers);
    void setLayers(vector<leed_layer> layers);
    void setLayers(leed_layer *layers, size_t n_layers);
    void setMinimumInterlayerDistance(double d_min);
    void setNumberOfAtoms(size_t n_atoms);
    void setNumberOfTypes(size_t n_types);

    void setComments(vector<string> comments);
    void setComments(char **comments, size_t n_comments);
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Crystal_h__ */
