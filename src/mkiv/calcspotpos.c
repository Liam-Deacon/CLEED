/************************************************************************     
              File Name: calcspotpos.c
     
*************************************************************************/

/************************************************************************/ 
#include "mkiv.h"
#include <stdlib.h>
/************************************************************************/

int calcspotpos(
        Vector a[],             /* basis vectors, origin */
        int ndom,               /* number of domains */
        Domain superl[],        /* inverse superlattice matrix elements */
        int spot_max,           
        int *nspot,             /* number of spots */
        Spot spot[],            /* list of spots */
        int nexcl,              /* number of excluded elements */
        Lindex excl[],          /* exclusion list */
        ImageMatrix *imask,     /* matrix containing image mask */
        float router,           /* outer radius of screen */
        float rinner,           /* inner radius of screen */
        float *radius,          /* calculated screen radius in pixels */
        float energy,           /* Beam energy in eV */
        float ratio,            /* ratio of screen radius to distance */
        float k_10              /* par value of (1,0)-beam */
    )

/*************************************************************************
   SU/ 29.04.91                                                         
   GH/ 20.08.92                                                         
   CS/ 3.8.93

  Purpose:
   calcspotpos calculates the positions of all reciprocal lattice points   
   within a reasonable frame and makes a list out of them               

  Input:
   a[0]          origin
   a[1],a[2]     basic lattice vectors
   ndom          number of domains
   superl        superstructure lattice vectors
   imask         ImageMatrix of screen boundaries
   router,rinner,center   -"-

  Output:
   nspot         number of measurable reflexes
   spot[i]       list of measurable reflexes
     spot->kp_len    length of k parallel to surface
     spot->lind1/2   lattex indices
     spot->x0,y0     calculated spot position
     spot->xx,yy     spot position that will be modified by fimax,calcoi
     spot->cos_th    cosine of k towards e-beam
   radius        calculated screen radius in pixels

*************************************************************************/
 
{
     register int i, j, k, pos, idom;
     int maxorder, orderb;        /* define frame                       */
     float min, len, rposx, rposy;
     float itok;                  /* ratio k/pixel                      */
     float sin_th, cos_th;        /* sine and cosine of theta */
     float cor_fac;               /* correction factor */
     unsigned short *im = (unsigned short*)imask->imagedata;
     int cols = imask->rows,
         rows = imask->cols;

/************************************************************************/

/* some precalculations */

     itok = k_10/a[1].len;                /* factor pixel to k-value */
     sin_th = k_10*H2M/sqrt(energy);      /* sine of (1,0)-reflex */
     *radius = a[1].len/sin_th;           /* screen-radius (constant) */
     min = MIN( a[1].len , a[2].len );    /* MIN(a1,a2) */
     orderb = RNDUP(*radius/min);         /* maximum integral spot index */
 
/* compute reciprocal lattice points */
     rposx = rposy = 0.;

     k=0;
     for (idom=0; idom <=ndom; idom++)
     {
	if ( idom==ndom ) 
/* max order of substrate reflexes within radius */
	   maxorder = orderb;
	else
/* max order of superstructure reflexes within radius */
	{
	   min = MIN( PYTH(superl[idom].lind11,superl[idom].lind12),
		      PYTH(superl[idom].lind21,superl[idom].lind22) );
	   maxorder = RNDUP(orderb/min);
	}

	for (i= -maxorder; i<= maxorder; i++)
	{
	   for (j= -maxorder; j<= maxorder; j++)
	   {
              if (idom==ndom)           /* substrate lattice */
	      {
                 spot[k].lind1 = (float)i;
	         spot[k].lind2 = (float)j;
	         rposx = a[1].xx *i + a[2].xx *j;
	         rposy = a[1].yy *i + a[2].yy *j;
              }
	      if (idom<ndom)            /* superstructure lattice */
	      {
	         spot[k].lind1 = i*superl[idom].lind11 +j*superl[idom].lind21;
	         spot[k].lind2 = i*superl[idom].lind12 +j*superl[idom].lind22;
                 rposx = a[1].xx *spot[k].lind1 +a[2].xx *spot[k].lind2;
                 rposy = a[1].yy *spot[k].lind1 +a[2].yy *spot[k].lind2;
              }
	      sin_th = PYTH(rposx,rposy)/(*radius);
              if(sin_th > 1.) continue;

/* compute correction factor for the reflex position */

              cos_th = sqrt(1. - SQUARE(sin_th));
              cor_fac = 1. + ratio * (1. - cos_th);
              rposx /= cor_fac;
              rposy /= cor_fac; 

/* store calculated coordinates */

	      spot[k].xx = spot[k].x0 = a[0].xx + rposx;
	      spot[k].yy = spot[k].y0 = a[0].yy + rposy;

/* when spot-position is out of the visible LEED-screen area -> continue */

	      if ( spot[k].y0 < 0 || spot[k].y0 >= rows ) continue;
	      if ( spot[k].x0 < 0 || spot[k].x0 >= cols ) continue;
	      pos = (int)spot[k].y0 * cols + (int)spot[k].x0;
	      if ( imask && !im[pos] ) continue;
	      len = PYTH(rposx,rposy);
	      if ( !imask && (len<rinner || len>router) ) continue;

/* store kp_len and cos_th */

	      spot[k].kp_len = itok * len;
	      spot[k].cos_th = cos_th;
	      if (k<spot_max) k++;
           } 
        }
     }
     if ( (*nspot=k) >= spot_max) 
	ERR_EXIT(corec_lat: Max. No. of spots was exceeded );

/* remove all reflexes which appear twice or more */

     for (k=0; k<*nspot-1; k++)
	for (i=k+1; i<*nspot; i++)
	   if ( PYTH(spot[k].lind1-spot[i].lind1,
         	     spot[k].lind2-spot[i].lind2 ) < TOLERANCE)
	      spot[i--] = spot[--*nspot];

/* remove all reflexes that appear in the list excl */

     for (k=0; k<*nspot; k++)
        for (i=0; i<nexcl; i++)
	   if ( PYTH(spot[k].lind1-excl[i].lind1,
		     spot[k].lind2-excl[i].lind2 ) < TOLERANCE)
	      spot[k--] = spot[--*nspot];
          
    return 0;
}
/***********************************************************************/
