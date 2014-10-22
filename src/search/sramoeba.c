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
 *28.09.00

file contains function:

  void sr_amoeba(real **p, real *y, int ndim, 
                 real ftol, real (*funk)(), int *nfunk)

 Find minimum by simplex algorithm. (From Num. Rec.)

 Changes:

NR/23.07.93 - Copy from Num. Rec.
BN/   07.93 - Copy vertex to *.ver after each iteration.
GH/12.09.95 - Criterion of termination is absolute deviation in vertex
              rather than relative deviation.
LD/30.04.14 - Removed dependence on 'cp' and 'date' system calls

***********************************************************************/
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include "csearch.h"
#include "copy_file.h"

#ifndef MAX_ITER_AMOEBA        /* should be defined in "search_def.h" */
#define MAX_ITER_AMOEBA 500
#endif

#define ALPHA 1.0
#define BETA 0.5
#define GAMMA 2.0

#define GET_PSUM for (j=1;j<=ndim;j++) \
                 { for (i=1,sum=0.0;i<=mpts;i++)\
                     sum += p[i][j]; psum[j]=sum;\
                 }

/**************************************************************************/

extern char *sr_project;

void sr_amoeba(real **p, real *y, int ndim, 
               real ftol, real (*funk)(), int *nfunk)

{

FILE *ver_stream;
char ver_file[STRSZ];

int i,j,ilo,ihi,inhi,mpts=ndim+1;
real ytry,ysave,sum,rtol,*psum;
real amotry();

char *old_file;
char *new_file;
time_t result;

	psum=vector(1,ndim);
	*nfunk=0;
	GET_PSUM
	for (;;) {
		ilo=1;
		ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
		for (i=1;i<=mpts;i++) {
			if (y[i] < y[ilo]) ilo=i;
			if (y[i] > y[ihi]) {
				inhi=ihi;
				ihi=i;
			} else if (y[i] > y[inhi])
				if (i != ihi) inhi=i;
		}
/* relative deviation:
		rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo]));
*/
/* absolute deviation: */
		rtol=2.0*fabs(y[ihi]-y[ilo]);
		if (rtol < ftol) break;
		if (*nfunk >= MAX_ITER_AMOEBA) 
                   nrerror("Too many iterations in AMOEBA");
		ytry=amotry(p,y,psum,ndim,funk,ihi,nfunk,-ALPHA);
		if (ytry <= y[ilo])
			ytry=amotry(p,y,psum,ndim,funk,ihi,nfunk,GAMMA);
		else if (ytry >= y[inhi]) {
			ysave=y[ihi];
			ytry=amotry(p,y,psum,ndim,funk,ihi,nfunk,BETA);
			if (ytry >= ysave) {
				for (i=1;i<=mpts;i++) {
					if (i != ilo) {
						for (j=1;j<=ndim;j++) {
							psum[j]=0.5*(p[i][j]+p[ilo][j]);
							p[i][j]=psum[j];
						}
						y[i]=(*funk)(psum);
					}
				}
				*nfunk += ndim;
				GET_PSUM
			}
		}
/***************************************************************************
  Write y/p to backup file
***************************************************************************/
       /* removed dependence on 'cp' & 'date' system calls */
       old_file = (char *) malloc(sizeof(char)*(strlen(sr_project)+5));
       new_file = (char *) malloc(sizeof(char)*(strlen(sr_project)+5));
       
       /* remove 'cp' system call dependence */
       strcpy(old_file, sr_project);
       strcpy(new_file, sr_project);
       strcat(old_file, ".ver");
       strcat(new_file, ".vbk");
       if (copy_file(old_file, new_file)) 
       {
         fprintf(STDERR, "*** error (sramebsa): "
            "failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
         exit(1);
       }

        strcpy(ver_file, new_file);
        ver_stream = fopen(ver_file,"w");
        fprintf(ver_stream,"%d %d %s\n",ndim, mpts, sr_project);
        for (i = 1; i<= mpts; i++) 
        {
          fprintf(ver_stream,"%e ", y[i]);
          for(j=1; j<= ndim; j++) 
            fprintf(ver_stream,"%e ", p[i][j]);
          fprintf(ver_stream,"\n");
        }
        
        /* add date */
        result = time(NULL);
        fprintf(ver_stream, "%s\n", asctime(localtime(&result)));
        
        fclose(ver_stream);

	}
	free_vector(psum,1);
}  /* end of function sr_amoeba */

/**************************************************************************/

real amotry(real **p, real *y, real *psum, int ndim,
            real (*funk)(), int ihi, int *nfunk, real fac)

{
	int j;
	real fac1,fac2,ytry,*ptry,*vector();

	ptry=vector(1,ndim);
	fac1=(1.0-fac)/ndim;
	fac2=fac1-fac;
	for (j=1;j<=ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
	ytry=(*funk)(ptry);
	++(*nfunk);
	if (ytry < y[ihi]) {
		y[ihi]=ytry;
		for (j=1;j<=ndim;j++) {
			psum[j] += ptry[j]-p[ihi][j];
			p[ihi][j]=ptry[j];
		}
	}
	free_vector(ptry,1);
	return ytry;
}  /* end of function amotry */

/**************************************************************************/

#undef ALPHA
#undef BETA
#undef GAMMA
