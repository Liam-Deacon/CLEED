#include "MillerIndex.hh"

using namespace cleed;

MillerIndex::MillerIndex(
  double h, 
  double k, 
  double l) {
  this->h = h;
  this->k = k;
  this->l = l;
}

MillerIndex::MillerIndex(const miller_hkl *hkl) {

}


MillerIndex::~MillerIndex() {

}

double MillerIndex::getMillerH() const {
  return this->h;
}

double MillerIndex::getMillerK() const {
  return this->k;
}

double MillerIndex::getMillerL() const {
  return this->l;
}

MillerIndex &MillerIndex::setMillerH(double h) {
  this->h = h;
  return *this;
}

MillerIndex &MillerIndex::setMillerK(double k) {
  this->k = k;
  return *this;
}

MillerIndex &MillerIndex::setMillerL(double l) {
  this->l = l;
  return *this;
}

MillerIndex &MillerIndex::setMillerIndices(double h, double k, double l) {
  this->setMillerH(h);
  this->setMillerK(k);
  this->setMillerL(l);
  return *this;
}

void MillerIndex::getMillerIndices(double &h, double &k, double &l) const {
  h = this->getMillerH();
  k = this->getMillerK();
  l = this->getMillerL();
}

const std::string &MillerIndex::getMillerIndicesAsString() const {
  const string s = "(" + this->h + ", " + this->getMillerK() + ", " + this->getMillerL() + ")";
  return 
}

std::ostream & operator << (std::ostream &os, MillerIndex const &hkl) { 
  return os << hkl.getMillerIndicesAsString();
}
 