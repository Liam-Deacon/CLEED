/*********************************************************************
 *                      Search.h
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

#ifndef __Search_h__
#define __Search_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */ 

#include "search_def.h"
using namespace std;

namespace cleed {

class Search {
  public:
    Search();
    ~Search();

  private:
    search *search_ptr;
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Search_h__ */
