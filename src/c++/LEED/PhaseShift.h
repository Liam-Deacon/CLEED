/*********************************************************************
 *                      PhaseShift.h
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *
 * Object Orientated wrapper class for leed_phase structure.
 */

#ifndef __PHASESHIFT_HH__
#define __PHASESHIFT_HH__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */

#include "cleed_real.h"
#include "leed_def.h"
#include <vector>
#include <string>

namespace cleed {

class PhaseShift : public leed_phase {

  public:
    PhaseShift();
    PhaseShift(const leed_phase *phsh_ptr);
    PhaseShift(const std::string &inputFile);
    ~PhaseShift();

    PhaseShift &operator+(const PhaseShift& phsh);

    /* getters */
    int getLmax() const;
    leed_matrix_diag getMatrixType() const;
    real getMaxEnergy() const;
    real getMinEnergy() const;
    std::vector<real> getEnergyList() const;
    std::vector<real> getPhaseShiftValuesList() const;
    std::vector<real> getDeltaRList() const;
    std::size_t getNumberOfEnergies() const;

    /* setters */
    void setLmax(int lmax);
    void setMatrixType(leed_matrix_diag Type);
    void setMaxEnergy(real Emax);
    void setMinEnergy(real Emin);
    void setInputFile(const std::string &filepath);
    void setInputFile(const char *filepath);
    void setData(const real *energies_ptr, const real *phaseshifts_ptr, std::size_t n);
    void setData(const std::vector<real> energies, const std::vector<real> phaseshifts);
    void setDeltaR(std::vector<real> dr);
    void setDeltaR(real dr[4]);

    void setDataFromFile();
};

} /* namespace cleed */

# endif

#endif /* __PHASESHIFT_HH__ */
