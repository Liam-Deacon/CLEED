/********************************************************************
 *                       CRFSPLINT.C
 *
 *  Natural cubic spline interpolation using second derivatives prepared
 *  by cr_spline().
 ********************************************************************/

#include <stdlib.h>

#include "crfac.h"

real cr_splint(real eng, struct crelist *list, int leng)
{
  if (list == NULL || leng <= 0) return 0.0;
  if (leng == 1) return list[0].intens;

  /* Clamp to data bounds (avoids extrapolation surprises). */
  if (eng <= list[0].energy) return list[0].intens;
  if (eng >= list[leng - 1].energy) return list[leng - 1].intens;

  int lo = 0;
  int hi = leng - 1;
  while (hi - lo > 1) {
    int mid = lo + (hi - lo) / 2;
    if (list[mid].energy > eng) hi = mid;
    else lo = mid;
  }

  real h = list[hi].energy - list[lo].energy;
  if (IS_EQUAL_REAL(h, 0.0)) return list[lo].intens;

  real a = (list[hi].energy - eng) / h;
  real b = (eng - list[lo].energy) / h;

  real y = a * list[lo].intens + b * list[hi].intens;
  y += ((a * a * a - a) * list[lo].deriv2 +
        (b * b * b - b) * list[hi].deriv2) *
       (h * h) / 6.0;

  return y;
}
