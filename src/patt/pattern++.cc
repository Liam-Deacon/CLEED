#include "pattern.h"

using namespace cleed;

Pattern::Pattern()
{


}

Pattern::~Pattern()
{
  ::pattern_free(&pat);
}

Pattern::Pattern(const pattern *pat)
{
	std::memcpy(&this->pat, pat, sizeof(pattern));
}

Pattern::Pattern(const std::string &filename) 
{
	FILE *fp = std::fopen(filename.c_str(), "r");
	pattern *pat_ = pattern_read(fp);
	if (pat_ != NULL)
		std::memcpy(&this->pat, pat_, sizeof(pattern));
	pattern_free(pat_);
	std::fclose(fp);
}

Pattern::Pattern(FILE *fp) 
{
	pattern *pat_ = pattern_read(fp);
	if (pat_ != NULL)
		std::memcpy(&this->pat, pat_, sizeof(pattern));
	pattern_free(pat_);
}

void Pattern::setBasisVectorA1(double x, double y)
{
	pat.a1.x = x;
	pat.a1.y = y;
}

void Pattern::setBasisVectorA2(double x, double y)
{
	pat.a2.x = x;
	pat.a2.y = y;
}

std::vector<std::vector<double>> Pattern::getSuperStructureMatrices()
{
	std::vector<std::vector<double>> l(this->getNumberOfDomains(), std::vector<double>(4));
	
	for (std::size_t i = 0; i < l.size(); i++) {
		l[i][0] = pat.M_SS[0].M11;
		l[i][1] = pat.M_SS[0].M12;
		l[i][2] = pat.M_SS[0].M21;
		l[i][3] = pat.M_SS[0].M22;
	}

	return l;
}

