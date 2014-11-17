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
 *   CS/01.09.93 - creation
 **************************************************************************/

/*! \file
 *
 * Contains quicksort() and partition() functions for handling #mkiv_reflex arrays.
 */

#include "mkiv.h"

/*!
 * Sorts the elements of struct spot in increasing order of kp_len.
 *
 * \note Extracted from Kerninghan, Ritchie.
 *
 * \param low_ptr Pointer to the lower end of #mkiv_reflex array.
 * \param up_ptr Pointer to the upper end of #mkiv_reflex array.
 */
void quicksort(mkiv_reflex *low_ptr, mkiv_reflex *up_ptr)
{
  mkiv_reflex *piv_ptr;          /* pointer to angle point              */
  mkiv_reflex *partition();      /* make partition                      */

  if (low_ptr < up_ptr)          /* if there's something to sort        */
  {
    piv_ptr = partition(low_ptr, up_ptr); /* divide in sortable parts */
    quicksort(low_ptr, piv_ptr-1);        /* sort lower part          */
    quicksort(piv_ptr, up_ptr);           /* sort upper part          */
  }
} /* end of function quicksort */

/*!
 * Divides the table into two parts referring to the angle point.
 *
 * \param low_ptr Pointer to the lower end of #mkiv_reflex array.
 * \param up_ptr Pointer to the upper end of #mkiv_reflex array.
 *  * \return Pointer to \p low_ptr .
 */
mkiv_reflex *partition(mkiv_reflex *low_ptr, mkiv_reflex *up_ptr)
{
  mkiv_reflex pivot, temp;

  pivot = *(low_ptr + (up_ptr - low_ptr)/2);
  while (low_ptr <= up_ptr)          /* until pointers meet             */
  {
    /* search for badly placed value in the lower array part */
    while (low_ptr->kp_len < pivot.kp_len) low_ptr++;

    /* search for badly placed value in the upper array part */
    while (up_ptr->kp_len > pivot.kp_len) up_ptr--;

    /* exchange badly placed value if necessary */
    if (low_ptr <= up_ptr)
    {
      temp = *low_ptr;
      *low_ptr = *up_ptr;
      *up_ptr = temp;

      low_ptr++;
      up_ptr--;
    }
  }
  return(low_ptr);
}
