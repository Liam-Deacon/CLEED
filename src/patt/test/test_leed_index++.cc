#include <cstdio>
#include "leed_index.h"
using namespace cleed;

int main()
{
  Index i;
  i.setH(10.);
  i.setK(20.);
  
  printf("%f, %f\n", i.getH(), i.getK());
  
  return 0;
}