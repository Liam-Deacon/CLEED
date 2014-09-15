#ifndef LATTICESTRUCTURE_HPP
#define LATTICESTRUCTURE_HPP

#include "lattice.hpp"

/* combine into the overall structure */
class LatticeStructure
{
public:
    LatticeStructure();
    LatticeStructure(Lattice &lattice,
                     QList<SuperLattice> &superLattices,
                     QList<Domain> &domains);
    ~LatticeStructure();

    void setLattice(Lattice &lattice);
    void setSuperLattice(SuperLattice &superLattices, int pos = -1);
    void setSuperLattices(QList<SuperLattice> &superLattices);
    void setDomain(Domain &domain, int pos = -1);
    void setDomains(QList<Domain> &domains);

    Lattice getLattice();
    QList<SuperLattice> getSuperLatticeList();
    QList<Domain> getDomainList();

private:
    Lattice lattice;
    QList<SuperLattice> superLattices;
    QList<Domain> domains;

};

class LatticeStructure3D
{
public:
    LatticeStructure3D();
    ~LatticeStructure3D();

    void setLattice(Lattice3D &lattice);

private:
    Lattice3D *lattice;
};


#endif // LATTICESTRUCTURE_HPP
