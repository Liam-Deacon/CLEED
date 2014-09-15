/*********************************************************************
GH/12.09.95
File contains:

Utility functions for Num. Rec. programs

Changes
NR          - Copy from "nrutil.c" in Num. Rec. package.
GH/12.09.95 - Change all "double" to "real" and include "search.h"
*********************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "search.h"

void nrerror(const char *error_text)
{

	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
	exit(1);
}

/**************************************************************************/

real *vector(int nl,int nh)
{
	real *v;

	v=(real *)malloc((unsigned) (nh-nl+1)*sizeof(real));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl;
}

/**************************************************************************/

int *ivector(int nl,int nh)
{
	int *v;

	v=(int *)malloc((unsigned) (nh-nl+1)*sizeof(int));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl;
}

/**************************************************************************/

real **matrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	real **m;

	m=(real **) malloc((unsigned) (nrh-nrl+1)*sizeof(real*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(real *) malloc((unsigned) (nch-ncl+1)*sizeof(real));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}

/**************************************************************************/

real **submatrix(real **a,int oldrl,int oldrh,int oldcl,int newrl,int newcl)
{
	int i,j;
	real **m;

	m=(real **) malloc((unsigned) (oldrh-oldrl+1)*sizeof(real*));
	if (!m) nrerror("allocation failure in submatrix()");
	m -= newrl;

	for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+oldcl-newcl;

	return m;
}

/**************************************************************************/

int free_vector(real *v,int nl)
{
	free((char*) (v+nl));
	return 0;
}

/**************************************************************************/

int free_ivector(int *v,int nl)
{
	free((char*) (v+nl));
	return 0;
}

/**************************************************************************/

int free_matrix(real **m,int nrl,int nrh,int ncl)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
	return 0;
}

/**************************************************************************/

int free_imatrix(int **m,int nrl,int nrh,int ncl)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
	return 0;
}

/**************************************************************************/

int free_submatrix(real **b,int nrl)
{
	free((char*) (b+nrl));
	return 0;
}

/**************************************************************************/

real **convert_matrix(real *a,int nrl,int nrh,int ncl,int nch)
{
	int i,j,nrow,ncol;
	real **m;

	nrow=nrh-nrl+1;
	ncol=nch-ncl+1;
	m = (real **) malloc((unsigned) (nrow)*sizeof(real*));
	if (!m) nrerror("allocation failure in convert_matrix()");
	m -= nrl;
	for(i=0,j=nrl;i<=nrow-1;i++,j++) m[j]=a+ncol*i-ncl;
	return m;
}

/**************************************************************************/

int free_convert_matrix(real **b,int nrl)
{
	free((char*) (b+nrl));
	return 0;
}

/**************************************************************************/
