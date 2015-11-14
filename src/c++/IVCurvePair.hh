/*********************************************************************
 *													IVCurvePair.hh
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 *
 * Provides IVCurvePair class for combining theoretical and
 * experimental IV curves.
 */


#ifndef __IVCurvePair_HH__
#define __IVCurvePair_HH__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include <iostream>
#include <IVCurve.hh>

using ::rfac_ivcur;

namespace cleed {

  /* classes */
  class IVCurvePair {

    public:
      /* constructors and destructor */
      IVCurvePair(const IVCurvePair &other);
      IVCurvePair(const rfac_ivcur *ivcur_ptr);
      IVCurvePair(const IVCurve &theory, const IVCurve &experimental);
      ~IVCurvePair();

      /* operator overloads */
      //IVCurvePair operator=(const IVCurvePair &other);
      //IVCurvePair operator=(const rfac_ivcur *ivcur_ptr);
      friend std::ostream& operator<<(std::ostream& out, const IVCurvePair& ivs);

      /* getters */
      double getWeighting() const;
      double getEnergyOfFirstAppearance() const;
      double getOverlap() const;
      int getGroupID() const;

      const rfac_ivcur *getIVPairPtr() const;

      /* setters */
      void calculateOverlap();
      IVCurvePair& setWeighting(double weight);
      IVCurvePair& setSpotID(const rfac_spot *spot);
      IVCurvePair& setTheoryIVCurve(const IVCurve &theory);
      IVCurvePair& setExperimentalIVCurve(const IVCurve &experimental);

    private:
      rfac_ivcur *iv_pair;
  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __IVCurvePair_H__ */
