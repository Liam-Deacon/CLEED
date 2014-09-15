#ifndef LATTICE_HPP
#define LATTICE_HPP

#include <QPointF>
#include <QGenericMatrix>
#include <QVector>

#include "matrix.hpp"

class Lattice
{
public:
    explicit Lattice();
    explicit Lattice(Matrix2x2 &basis);
    explicit Lattice(QPointF &a1Vector, QPointF &a2Vector);
    ~Lattice();

    void setBasis(Matrix2x2 &basis);
    void setBasis(QPointF &a1Vector, QPointF &a2Vector);

    Matrix2x2 getBasis();
    QPointF getBasisVector(int row = 0);

private:
    QPointF a1, a2;
};

class Lattice3D : public Lattice
{
public:
    Lattice3D();
    Lattice3D(QMatrix3x3 &basis);
    ~Lattice3D();

private:
    QPointF c;
};

/* the supercell structure depends on the underlying lattice */
class SuperLattice : public Lattice
{
public:
    SuperLattice();
    SuperLattice(Lattice &lattice, Matrix2x2 &matrix);
    ~SuperLattice();

    void setLattice(Lattice &lattice);
    void setSuperMatrix(Matrix2x2 &matrix);

    Matrix2x2 getSuperMatrix();
    Lattice getLattice();

private:
    Lattice lattice;
    Matrix2x2 superMatrix;
};

/* each domain is a specific superlattice operation */
class Domain : public SuperLattice
{
public:
    Domain();
    Domain(SuperLattice &superLattice, QString &domainOperation);
    ~Domain();

    void setSuperLattice(SuperLattice &super);
    void setDomain(QString &domainOperation);


    SuperLattice getSuperLattice();
    QString getDomainOp();

private:
    SuperLattice superLattice;
    QString domainOperation;
};

#endif // LATTICE_HPP
