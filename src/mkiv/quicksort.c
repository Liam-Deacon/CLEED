/**************************************************************************
     
              File Name: quicksort.c
     
 **************************************************************************/

/***************************************************************************/
#include "mkiv.h"
/***************************************************************************/

void quicksort(

/****************************************************************************
   SU/ 17.04.91 (extracted from Kerninghan,Ritchie)
   quicksort sorts the elements of struct spot in increasing order of kp_len
****************************************************************************/

struct spot *low_ptr,              /* pointer to lower/upper array end    */ 
struct spot *up_ptr                /* pointer to lower/upper array end    */ 
)
{
    struct spot *piv_ptr,          /* pointer to angle point              */
                *partition();      /* make partition                      */

    if (low_ptr < up_ptr)          /* if there's something to sort        */
    {
        piv_ptr = partition(low_ptr,up_ptr); /* divide in sortable parts */
        quicksort(low_ptr,piv_ptr-1);        /* sort lower part          */
        quicksort(piv_ptr,up_ptr);           /* sort upper part          */
    }
}
/***************************************************************************/
 
struct spot *partition(low_ptr,up_ptr)
 
/****************************************************************************
   SU/ 17.04.91
   partition divides the table into two parts referring to the angle point
****************************************************************************/

struct spot *up_ptr, *low_ptr;
{
    struct spot pivot,temp;

    pivot = *(low_ptr + (up_ptr - low_ptr)/2);
    while (low_ptr <= up_ptr)          /* until pointers meet             */
    {
        /* search for badly placed value in the lower array part */
        while ( low_ptr->kp_len < pivot.kp_len) low_ptr++;
            /* search for badly placed value in the upper array part */
            while ( up_ptr->kp_len > pivot.kp_len) up_ptr--; 

            if (low_ptr <= up_ptr)        /*exchange badly placed value      */
            {
                temp = *low_ptr;
                *low_ptr = *up_ptr;
                *up_ptr = temp;
                low_ptr++; up_ptr--;
            }
    }
    return low_ptr;
}
/***************************************************************************/
