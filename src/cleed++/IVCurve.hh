/*********************************************************************
 *													IVCurve.hh
 *
 *  Copyright (C) 2013-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/15 Dec 2014 - creation
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 * \date 15 Dec 2014
 *
 * Provides IVCurve class for manipulating IV curves used in R Factor program.
 */


#ifndef __IVCurve_HH__
#define __IVCurve_HH__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "rfac_def.h"
#include "rfac_func.h"

using ::rfac_iv;
using ::rfac_iv_alloc;
using ::rfac_iv_data;
using ::rfac_iv_copy;
using ::rfac_iv_free;
using ::rfac_iv_sort;
using ::rfac_iv_spline;
using ::rfac_iv_lorentz_smooth;

using std::vector;
using std::string;
using std::size_t;

namespace cleed {

/* classes */

  class IVCurve : public rfac_iv {

    friend class RFactor;

    public:
      IVCurve();
      IVCurve(const rfac_iv &iv);
      IVCurve(const string &ivFilePath);
      IVCurve(const IVCurve &ivCurve);
      ~IVCurve();

      /* operators */
      IVCurve& operator=(const IVCurve &other);
      bool operator==(const IVCurve &other) const;
      bool operator!=(const IVCurve &other) const;

      friend std::istream& operator>>(std::istream& in, IVCurve& iv);
      friend std::ostream& operator<<(std::ostream& out, const IVCurve& iv);

      /* setters */
      IVCurve &setIVData(const rfac_iv_data &iv_data, size_t n);
      IVCurve &setIVData(vector<real> x, vector<real> y, vector<real> deriv2);

      IVCurve &sort();
      IVCurve &smooth(double vi=4.0);
      IVCurve &spline();

      IVCurve &setInitialEnergy(double e_0);
      IVCurve &setLastEnergy(double e_n);
      IVCurve &setMaximumIntensity(double maxI);

      void writeIVData(const string &ivFilePath);

      /* getters */
      const rfac_iv_data *getRfacIVData() const;
      void getIVData(vector<real> &x, vector<real> &y, vector<real> &deriv2) const;

      bool isEquiDistant() const;
      bool isSorted() const;
      bool isSmoothed() const;
      bool isSplined() const;

      double getInitialEnergy() const;
      double getLastEnergy() const;
      double getMaximumIntensity() const;

      const rfac_iv *get_rfac_iv_ptr() const;

      void readIVData(const string &ivFilePath);

    private:
      rfac_iv *iv_ptr;
      string comments;
  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __IVCurve_HH__ */
