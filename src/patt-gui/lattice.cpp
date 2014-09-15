#include "lattice.hpp"


////////////////////////////////////////////////////////////////////////////////
///                      CLASS LATTICE                                       ///
////////////////////////////////////////////////////////////////////////////////

Lattice::Lattice()
{
    QPointF a1v = QPointF(1., 0.);
    QPointF a2v = QPointF(0., 1.);
    setBasis(a1v, a2v);
}

Lattice::Lattice(Matrix2x2 &basis)
{
    setBasis(basis);
}


Lattice::Lattice(QPointF &a1Vector, QPointF &a2Vector)
{
    setBasis(a1Vector, a2Vector);
}

Lattice::~Lattice(){

}

void Lattice::setBasis(Matrix2x2 &basis)
{
    a1 = QPointF(basis.m[0][0], basis.m[0][1]);
    a2 = QPointF(basis.m[1][0], basis.m[1][1]);
}


void Lattice::setBasis(QPointF &a1Vector, QPointF &a2Vector)
{
    a1 = a1Vector;
    a2 = a2Vector;
}

QPointF Lattice::getBasisVector(int row)
{
    switch(row)
    {
        case 0:
            return a1;
        case 1:
            return a2;
        default:
            return QPointF(0., 0.);
    }
}



////////////////////////////////////////////////////////////////////////////////
///                 CLASS SUPERLATTICE                                       ///
////////////////////////////////////////////////////////////////////////////////

SuperLattice::SuperLattice()
{
    /* default to 1x1 */
    Matrix2x2 m;

    setSuperMatrix(m);

    lattice = Lattice();
}

SuperLattice::SuperLattice(Lattice &lattice, Matrix2x2 &matrix )
{
    setLattice(lattice);
    setSuperMatrix(matrix);
}

SuperLattice::~SuperLattice()
{

}


Lattice SuperLattice::getLattice()
{
    return lattice;
}

Matrix2x2 SuperLattice::getSuperMatrix()
{
   return superMatrix;
}

void SuperLattice::setLattice(Lattice &lattice)
{
    this->lattice = lattice;
}

void SuperLattice::setSuperMatrix(Matrix2x2 &m)
{
    superMatrix = m;
}


////////////////////////////////////////////////////////////////////////////////
///                       CLASS DOMAIN                                       ///
////////////////////////////////////////////////////////////////////////////////


Domain::Domain()
{
    SuperLattice super();
    domainOperation = "R0";
}


Domain::Domain(SuperLattice &superLattice, QString &domainOperation)
{
    setSuperLattice(superLattice);
    setDomain(domainOperation);
}


Domain::~Domain()
{

}


QString Domain::getDomainOp()
{
    return domainOperation;
}


SuperLattice Domain::getSuperLattice()
{
    return superLattice;
}


void Domain::setDomain(QString &domainOperation)
{
    if(domainOperation.startsWith("R")
            || domainOperation.startsWith("S"))
    {
        this->domainOperation = domainOperation;
    }
}

void Domain::setSuperLattice(SuperLattice &super)
{
    this->superLattice = super;
}
