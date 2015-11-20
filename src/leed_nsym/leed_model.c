/*********************************************************************
 *													leed_model.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/19 Mar 2015 - creation
 *********************************************************************/

/*!
 * \file leed_model.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "leed.h"

leed_model *leed_model_init(leed_args *args) {
  leed_model *model = (leed_model*) calloc(1, sizeof(leed_model));

  int iaux = 0;
  size_t n_set = 0;

  if(model == NULL) {
    ERROR_MSG("could not allocate memory for leed_model\n");
    exit(ENOMEM);
  }

  model->args = args;

  leed_beam *beams_all = model->beams_all;
  leed_beam *beams_out = model->beams_out;
  leed_crystal *bulk = model->bulk;
  leed_crystal *over = model->over;
  leed_energy *energy = model->energy;
  leed_phase *phs_shifts = model->phs_shifts;
  leed_var *v_par = model->v_par;

  /*============================================================
   * Read input parameters
   *============================================================*/
  leed_inp_read_bul_nd(&bulk, &phs_shifts, args->bul_file);
  leed_inp_leed_read_par(&v_par, &energy, bulk, args->bul_file);
  leed_read_overlayer_nd(&over, &phs_shifts, bulk, args->par_file);

  if ( (iaux = leed_beam_gen(&beams_all, bulk, v_par, energy->final)) > 0)
    model->n_set = (size_t)iaux;
  else
    model->n_set = 0;

  leed_inp_show_beam_op(bulk, over, phs_shifts);

  if( args->ctr_flag == CTR_EARLY_RETURN )
  {
    CONTROL_MSG(CONTROL, "EARLY RETURN \n");
    exit(0);
  }

  leed_output_header(args->res_stream);
  leed_output_beam_list(&beams_out, beams_all, energy, args->res_stream);

  CONTROL_MSG(CONTROL, "E_ini = %.1f, E_fin = %.1f, E_stp %.1f\n",
              energy->initial*HART, energy->final*HART, energy->step*HART);

  CONTROL_MSG(CONTROL, "n_set = %d\n", n_set);

  return model;
}


void leed_model_free(leed_model *model) {
  for (int i_beam=0; &model->beams_all[i_beam] != NULL; i_beam++) {
    leed_beam_free(&model->beams_all[i_beam]);
  }
  for (int i_beam=0; &model->beams_out[i_beam] != NULL; i_beam++) {
    leed_beam_free(&model->beams_out[i_beam]);
  }
  for (int i_phase=0; &model->phs_shifts[i_phase] != NULL; i_phase++) {
    leed_phase_free(&model->phs_shifts[i_phase]);
  }
  leed_crystal_free(model->bulk);
  leed_crystal_free(model->over);
  leed_var_free(model->v_par);
  leed_args_free(model->args);
  free(model);
  model = NULL;
}

