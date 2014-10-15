/*********************************************************************
 *                           MILLER_INDEX.H
 *
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: header file for miller_index_t (C) & MillerIndex (C++)
 *
 * Changes:
 *   LD/2014.08.23 - creation
 *********************************************************************/

#ifndef MILLER_INDEX_H
#define MILLER_INDEX_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  extern "C" {
#endif

/*! \typedef \struct miller_hkl
 *  \brief data structure for (hkl) Miller index values.
 */
typedef struct
{
  double h;   /*!< h Miller index */
  double k;   /*!< k Miller index */
  double l;   /*!< l Miller index */
} miller_hkl;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

class MillerIndex {
  public:
    MillerIndex();
    MillerIndex(double h, double k);
    MillerIndex(double h, double k, double l);
    ~MillerIndex();
    
    void setH(double h);
    void setK(double k);
    void setL(double l);
    
    double getH();
    double getK();
    double getL();
    
  protected:
    miller_hkl *miller;
}; /* class MillerIndex */

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* MILLER_INDEX_H */



