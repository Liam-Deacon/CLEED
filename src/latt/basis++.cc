#include "basis.h"

using namespace cleed;

Basis::Basis()
{
  this->basis = ::basis_init();
}

Basis::Basis(basis_t *a)
{
  this->basis = a;
}

Basis::~Basis()
{
  ::basis_free(this->basis);
}

void Basis::setA1(const Coordinate &a1)
{
  //const coord_t* coord = a1.get_coord_t();
  //::coord_set(this->basis->a, coord);
}

void Basis::setA1(double a1_x, double a1_y, double a1_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis, 0), a1_x, a1_y, a1_z);
}

void Basis::setA2(const Coordinate &a2)
{

}

void Basis::setA2(double a2_x, double a2_y, double a2_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis, 1), a2_x, a2_y, a2_z);
}

void Basis::setA3(const Coordinate &a3)
{

}

void Basis::setA3(double a3_x, double a3_y, double a3_z)
{
  ::coord_set(BASIS_VECTOR_PTR(this->basis, 2), a3_x, a3_y, a3_z);
}

void Basis::setBasis(const Coordinate &a1, const Coordinate &a2, const Coordinate &a3)
{

}

void Basis::setBasis(const Basis &basis)
{
  this->basis = basis.basis;
}

void Basis::setBasis(const Basis *basis)
{
  this->basis = basis->basis;
}

/* getters */
const Coordinate Basis::getA1()
{
  const Coordinate xyz(this->basis->a[0].x, this->basis->a[0].y, 
                       this->basis->a[0].z);
  return xyz;
}

const Coordinate Basis::getA2()
{
  Coordinate xyz(this->basis->a[1].x, this->basis->a[1].y, 
                       this->basis->a[1].z);
  return xyz;
}

const Coordinate Basis::getA3()
{
  Coordinate xyz(this->basis->a[2].x, this->basis->a[2].y, 
                       this->basis->a[2].z);
  return xyz;
}

double **Basis::calculateRotationMatrix()
{
  return ::normal_get_rotation_matrix(this->basis);
}
