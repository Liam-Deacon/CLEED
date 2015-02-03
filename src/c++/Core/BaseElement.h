/*********************************************************************
 *                      BaseElement.h
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
 * Header for a basic element class.
 */

#ifndef __BaseElement_h__
#define __BaseElement_h__

#ifdef __cplusplus /* use C linkage if this is a C++ compiler */ 

#include <string>

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
  virtual std::size_t getAtomicNumber() const;
  virtual std::string getElementName() const;
  virtual std::string getElementSymbol() const;

  /* setters */
  virtual void setAtomicNumber(std::size_t Z);
  virtual void setElementName(const std::string &name);
  virtual void setElementSymbol(const char symbol[]);

private:
  std::size_t atomicNumber;
  std::string name;
  char symbol[2];
};

} /* namespace CLEED */

#endif /* __cplusplus */

#endif /* __BaseElement_h__ */
