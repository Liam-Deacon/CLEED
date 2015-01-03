#include "pattern.h"

using namespace cleed;

Pattern::Pattern()
{


}

Pattern::~Pattern()
{
  pattern_free(&pat);
}


inline void Pattern::setTitle(std::string title)
{
  pattern_set_title(&pat, title.c_str());
}

void Pattern::setBasisVectorA1(double x, double y)
{
  
}

void Pattern::setBasisVectorA2(double x, double y)
{


}

inline void Pattern::setRadius(double radius)
{
  pattern_set_radius(&pat, radius);
}

inline int Pattern::setNumberOfDomains(std::size_t ndom)
{
  return pattern_set_n_domains(&pat, ndom);
}

inline const string Pattern::getTitle()
{
  const std::string str(pattern_get_title(&pat));
}

inline std::size_t Pattern::getNumberOfDomains()
{
  return pattern_get_n_domains(&pat);
} 

std::list<double> Pattern::getSuperStructureMatrices()
{
}

