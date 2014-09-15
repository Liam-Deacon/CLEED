#ifndef MILLER_INDEX_H
#define MILLER_INDEX_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
  extern "C" {
#endif

typedef struct
{
  double h;
  double k;
  double l;
} miller_hkl_t;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

class MillerIndex {
  public:
    MillerIndex();
    MillerIndex(double h, double k);
    MillerIndex(double h, double k, double l);
    ~MillerIndex();
    
    void setH(double h);
    void setK(double k);
    void setL(double l);
    
    double getH();
    double getK();
    double getL();
    
  protected:
    miller_hkl_t *miller;
}; /* class MillerIndex */

} /* namespace cleed */

#endif /* __cplusplus */

#endif /* MILLER_INDEX_H */



