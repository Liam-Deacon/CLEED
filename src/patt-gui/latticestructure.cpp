#include "latticestructure.hpp"

LatticeStructure::LatticeStructure()
{
    /* setup a default structure */

    Lattice latt = Lattice();
    Matrix2x2 m = Matrix2x2();
    SuperLattice super = SuperLattice(latt, m);
    Domain dom = Domain();

    QList<Domain> doms;
    doms.append(dom);

    setLattice(latt);
    setSuperLattice(super);
    setDomains(doms);

}

LatticeStructure::
LatticeStructure(Lattice &lattice,
                 QList<SuperLattice> &superLattices,
                 QList<Domain> &domains)
{
    setLattice(lattice);
    setSuperLattices(superLattices);
    setDomains(domains);
}

LatticeStructure::~LatticeStructure()
{

}

QList<Domain> LatticeStructure::getDomainList()
{
    return domains;
}

Lattice LatticeStructure::getLattice()
{
    return lattice;
}

QList<SuperLattice> LatticeStructure::getSuperLatticeList()
{
    return superLattices;
}

void LatticeStructure::setDomain(Domain &domain, int pos)
{
    if (pos < 0)
    {
        pos = domains.count() - 1;
    }

    if (pos < domains.count())
    {
        domains[pos] = domain;
    } else {
        domains.append(domain);
    }

}

void LatticeStructure::setDomains(QList<Domain> &domains)
{
    this->domains = domains;
}

void LatticeStructure::setLattice(Lattice &lattice)
{
    this->lattice = lattice;
}

void LatticeStructure::setSuperLattice(SuperLattice &superLattice, int pos)
{
    if (pos < 0)
    {
        pos = superLattices.count()-1;
    }

    if (pos < superLattices.count())
    {
        superLattices[pos] = superLattice;
    } else {
        superLattices.append(superLattice);
    }

}

void LatticeStructure::setSuperLattices(QList<SuperLattice> &superLattices)
{
    this->superLattices = superLattices;
}
