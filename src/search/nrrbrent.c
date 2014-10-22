/*********************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *19.09.95

file contains function:

  real brent(real ax, real bx, real cx, real(*f)(real), real tol, real *xmin)

 Find minimum in one dimension (Brent's method)

 Changes:

NR/19.09.95 - Copy from Num. Rec.
GH/19.09.95 - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.

***********************************************************************/

#include <math.h>
#include "csearch.h"

#define ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SIGN(a,b) ((b) > 0.0 ? fabs(a) : -fabs(a))
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

real brent(real ax, real bx, real cx, real(*f)(real), real tol, real *xmin)
{
	int iter;
	real a,b,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
	real d=0.0;
    real e=0.0;
	void nrerror();

	a=((ax < cx) ? ax : cx);
	b=((ax > cx) ? ax : cx);
	x=w=v=bx;
	fw=fv=fx=(*f)(x);
	for (iter=1;iter<=ITMAX;iter++) {
		xm=0.5*(a+b);
		tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
		if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
			*xmin=x;
			return fx;
		}
		if (fabs(e) > tol1) {
			r=(x-w)*(fx-fv);
			q=(x-v)*(fx-fw);
			p=(x-v)*q-(x-w)*r;
			q=2.0*(q-r);
			if (q > 0.0) p = -p;
			q=fabs(q);
			etemp=e;
			e=d;
			if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
				d=CGOLD*(e=(x >= xm ? a-x : b-x));
			else {
				d=p/q;
				u=x+d;
				if (u-a < tol2 || b-u < tol2)
					d=SIGN(tol1,xm-x);
			}
		} else {
			d=CGOLD*(e=(x >= xm ? a-x : b-x));
		}
		u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
		fu=(*f)(u);
		if (fu <= fx) {
			if (u >= x) a=x; else b=x;
			SHFT(v,w,x,u)
			SHFT(fv,fw,fx,fu)
		} else {
			if (u < x) a=u; else b=u;
			if (fu <= fw ||  IS_EQUAL_REAL(w, x)) 
            {
				v=w;
				w=u;
				fv=fw;
				fw=fu;
			} 
            else if (fu <= fv || IS_EQUAL_REAL(v, x) || IS_EQUAL_REAL(v, w)) 
            {
				v=u;
				fv=fu;
			}
		}
	}
	nrerror("Too many iterations in BRENT");
	*xmin=x;
	return fx;
}

#undef ITMAX
#undef CGOLD
#undef ZEPS
#undef SIGN
