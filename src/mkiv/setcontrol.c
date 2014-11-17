/*********************************************************************
 *                        QUICKSORT.C
 *
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   CS/11.08.93 - creation
 **************************************************************************/

/*! \file
 *
 * Contains setcontrol() function to set the control bits of each mkiv_reflex.
 */

#include "mkiv.h"

/*!
 * Sets the @mkiv_reflex::control bits according to the input lists:
 *  bit SPOT_DESI = the spot is member of the \p desi array
    bit SPOT_REF  = the spot is member of the \p ref array
 *
 * \param nspot Number of measurable reflexes.
 * \param spot Pointer to array of measurable reflexes.
 * \param ndesi Number of desired (marked) reflexes.
 * \param desi Pointer to array of desired (marked) reflexes.
 * \param nref Number of reference reflexes.
 * \param ref Pointer to array of reference reflexes.
 *  * \return
 */
int setcontrol(size_t nspot, mkiv_reflex *spot, size_t ndesi, mkiv_index *desi,
               size_t nref, mkiv_index *ref)
{
  register size_t k, i;

  /* preset the control bytes */
  for (k=0; k < nspot; k++) spot[k].control = 0;
  for (i=0; i < ndesi; i++) desi[i].control = -1;
  for (i=0; i < nref;  i++) ref[i].control = -1;

  /* loop over all spots */
  for (k=0; k < nspot; k++)
  {
    for (i=0; i < ndesi; i++)
    {
      if (PYTH( spot[k].lind1-desi[i].lind1,
                spot[k].lind2-desi[i].lind2 ) > TOLERANCE) continue;
      spot[k].control |= SPOT_DESI;
      desi[i].control = (long int)k;

      break;
    }

    for (i=0; i < nref; i++)
    {
      if( PYTH( spot[k].lind1-ref[i].lind1,
                spot[k].lind2-ref[i].lind2 ) > TOLERANCE) continue;
      ref[i].control = (long int)k;
      spot[k].control |= SPOT_REF;

      break;
    }
  }   /* for( k ...) */

  return(0);
}
