/*********************************************************************
 *													IVCurvePair.h
 *
 *  Copyright (C) 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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
 * \file IVCurvePair.h
 * \author Liam Deacon
 * \date 15 Dec 2014
 */


#ifndef __IVCurvePair_H__
#define __IVCurvePair_H__

#ifdef __cplusplus /* if this is a C++ compiler then use C linkage */

#include "IVCurve.h"

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
      IVCurvePair &operator=(const IVCurvePair &other);
      IVCurvePair &operator=(const rfac_ivcur *ivcur_ptr);

      /* getters */
      double getWeighting() const;
      double getEnergyOfFirstAppearance() const;
      double getOverlap() const;
      int getGroupID() const;

      /* setters */
      void calculateOverlap();
      void setWeighting(double weight);
      void setSpotID(const rfac_spot *spot);
      void setTheoryIVCurve(const IVCurve &theory);
      void setExperimentalIVCurve(const IVCurve &experimental);

    private:
      rfac_ivcur *iv_pair;
  };

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* __IVCurvePair_H__ */
