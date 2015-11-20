/*********************************************************************
 *													rfac_ivcur.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*!
 * \file
 * \author Liam Deacon
 * \date 20 Mar 2015
 */

#include <stddef.h>
#include <stdio.h>

#include "rfac.h"

void rfac_iv_print(const rfac_iv *iv) {
  if (iv == NULL)
  {
    fprintf(stderr, "NULL\n");
  }
  else
  {
    fprintf(stderr,
      ".data@%p .n_eng=%3d .smooth=%d .sort=%d .spline=%d .equidist=%d",
      (void*)iv->data, iv->n_eng, iv->smooth, iv->sort, iv->spline, iv->equidist);
  }
}

/*!
 * Debug function for printing rfac_ivcur information list
 * \param ivs Pointer to start of rfac_ivcur list
 */
inline void rfac_ivcur_print(const rfac_ivcur *ivs) {
  if (ivs == NULL)
  {
    fprintf(stderr, "NULL\n");
    return;
  }

  fprintf(stderr, "sizeof iv_cur = %u\n", sizeof(rfac_ivcur));
  for (size_t i=0; ivs[i].group_id != END_OF_GROUP_ID; i++)
  {
    fprintf(stderr, "rfac_ivcur [%i]@%p\n", i, (void*)&ivs[i]);
    fprintf(stderr, "  .experimental@%p\n    ", (void*)ivs[i].experimental);
    rfac_iv_print(ivs[i].experimental);
    fprintf(stderr, "\n");
    fprintf(stderr, "  .theory@%p\n    ", (void*)ivs[i].theory);
    rfac_iv_print(ivs[i].theory);
    fprintf(stderr, "\n");
    fprintf(stderr, "  .group_id=%d\n", ivs[i].group_id);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
}

/*!
 * Processes theoretical and experimental iv curves within \p iv_cur list
 * using smoothing or interpolation if needed in order to prepare direct
 * comparison between theory and experimental curves.
 *
 * \param iv_cur Pointer to start of iv curves list, which should be terminated
 * with #END_OF_GROUP_ID
 * \param vi Imaginary part of the optical potential - used for
 * lorentz smoothing.
 * @return
 */
int rfac_ivcur_process(rfac_ivcur *iv_cur, real vi) {
  for(size_t i_list=0;
#if DEBUG
      iv_cur != NULL &&
#endif
          iv_cur[i_list].group_id != END_OF_GROUP_ID; i_list++)
  {
    printf("i=%d\n", i_list);
    /* smooth both experimental and theoretical curves */
    rfac_iv_lorentz_smooth(iv_cur[i_list].experimental, vi / 2);
    rfac_iv_lorentz_smooth(iv_cur[i_list].theory, vi / 2);

    /* prepare cubic spline */
    rfac_iv_spline((&iv_cur[i_list])->experimental);
    rfac_iv_spline((&iv_cur[i_list])->theory);
  }  /* for i_list */
  return 0;
}
