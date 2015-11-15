/************************************************************************
 *                            BASIS++.CC
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Change Log:
 *   LD/2014.07.30 - Creation
 ************************************************************************/

/*!
 * \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * \brief Implements #Basis class for object-orientated basis vectors data type
 * and associated member functions.
 */

#include <Core/Basis.hh>

using namespace cleed;

Basis::Basis()
{
  this->basis_ptr = ::basis_init();
}

Basis::Basis(const basis *a)
{
  this->basis_ptr = ::basis_init();
  ::basis_copy(this->basis_ptr, a);
}

Basis::~Basis()
{
  ::basis_free(this->basis_ptr);
}

inline Basis& Basis::setA1(const Coordinate &a1)
{
  ::coord_copy(BASIS_VECTOR_PTR(this->basis_ptr, 0), const_cast<coord*>(a1.pos));
  return *this;
}

inline Basis& Basis::setA2(const Coordinate &a2)
{
  ::coord_copy(BASIS_VECTOR_PTR(this->basis_ptr, 1), const_cast<coord*>(a2.pos));
  return *this;
}

inline Basis& Basis::setA3(const Coordinate &a3)
{
  ::coord_copy(BASIS_VECTOR_PTR(this->basis_ptr, 2), const_cast<coord*>(a3.pos));
  return *this;
}

inline Basis& Basis::setA1(double a1_x, double a1_y, double a1_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis_ptr, 0), a1_x, a1_y, a1_z);
  return *this;
}

inline Basis& Basis::setA2(double a2_x, double a2_y, double a2_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis_ptr, 1), a2_x, a2_y, a2_z);
  return *this;
}

inline Basis& Basis::setA3(double a3_x, double a3_y, double a3_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis_ptr, 2), a3_x, a3_y, a3_z);
}

Basis& Basis::setBasis(
    const Coordinate &a1,
    const Coordinate &a2,
    const Coordinate &a3)
{
  this->setA1(a1).setA2(a2).setA3(a3);
  return *this;
}

Basis& Basis::setBasis(const Basis &basis)
{
  this->basis_ptr = basis.basis_ptr;
  return *this;
}

Basis& Basis::setBasis(const Basis *basis)
{
  this->basis_ptr = basis->basis_ptr;
  return *this;
}

/* getters */
const Coordinate Basis::getA1() const
{
  const Coordinate xyz(this->basis_ptr->a[0].x,
                       this->basis_ptr->a[0].y,
                       this->basis_ptr->a[0].z);
  return xyz;
}

const Coordinate Basis::getA2() const
{
  Coordinate xyz(this->basis_ptr->a[1].x,
                 this->basis_ptr->a[1].y,
                 this->basis_ptr->a[1].z);
  return xyz;
}

const Coordinate Basis::getA3() const
{
  Coordinate xyz(this->basis_ptr->a[2].x,
                 this->basis_ptr->a[2].y,
                 this->basis_ptr->a[2].z);
  return xyz;
}

