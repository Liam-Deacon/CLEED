/*********************************************************************
 *                      Element.h
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
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

#ifndef __Element_h__
#define __Element_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */ 

using std::size_t;
using std::string;

namespace cleed {

class BaseElement {

public:
  BaseElement();
  BaseElement(std::size_t num);
  BaseElement(const std::string name);
  BaseElement(const char symbol[]);
  virtual ~BaseElement();

  /* getters */
  std::size_t getAtomicNumber();
  std::string getElementName();
  std::string getElementSymbol();

private:
  std::size_t atomicNumber;
  std::string name;
  char symbol[2];
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __Element_h__ */
