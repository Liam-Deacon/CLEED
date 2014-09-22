#include "leed_index.h"
#include <cstdlib>

using namespace cleed;

Index::Index()
{
  index.h = 0.;
  index.k = 0.;
}

Index::Index(double h, double k)
{
  index.h = h;
  index.k = k;
}

Index::~Index()
{
}
    
void Index::setH(double h)
{
  index.h = h;
}

void Index::setK(double k)
{
  index.k = k;
}

double Index::getH()
{
  return Index::index.h;
}

double Index::getK()
{
  return Index::index.k;
}
   