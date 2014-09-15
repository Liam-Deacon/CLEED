
/**************************************************************************<
    >
    >         File Name: setcontrol.c
    >
>**************************************************************************/

/**************************************************************************/
#include "mkiv.h"
/**************************************************************************/
 
int setcontrol(nspot,spot,ndesi,desi,nref,ref)

/***************************************************************************
  CS/11.8.93    
  setcontrol sets the spot->control bits according to the input lists:
    bit SPOT_DESI = the spot is member of the desi list
    bit SPOT_REF  = the spot is member of the ref list
***************************************************************************/

int nspot;                            /* number of and                    */
struct spot spot[];                   /* list of measurable reflexes */
int ndesi,nref;                       /* number of members and */
struct lindex desi[],ref[];           /* lists */      
{
   register int k,i;

/**************************************************************************/

/* preset the control bytes */

   for (k=0; k<nspot; k++) spot[k].control = 0;
   for (i=0; i<ndesi; i++) desi[i].control = -1;
   for (i=0; i<nref;  i++) ref[i].control = -1;

/* loop over all spots */

   for( k=0; k<nspot; k++) {
      for (i=0; i<ndesi; i++) {
       	 if( PYTH( spot[k].lind1-desi[i].lind1,
	           spot[k].lind2-desi[i].lind2 ) > TOLERANCE) continue;
	 spot[k].control |= SPOT_DESI;
	 desi[i].control = k;
	 break;
      }
      for (i=0; i<nref; i++) {
	 if( PYTH( spot[k].lind1-ref[i].lind1,
		   spot[k].lind2-ref[i].lind2 ) > TOLERANCE) continue;
	 ref[i].control = k;
	 spot[k].control |= SPOT_REF;
	 break;
      }
   }   /* for( k ...) */
   
   return 0;
}
/**************************************************************************/
