/*********************************************************************
 *													leed_energy_loop.c
 *
 *  Copyright (C) 1992-2015 Georg Held <g.held@reading.ac.uk>
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*!
 * \file
 *
 * Provides functions for LEED IV calculations for different energies.
 *
 * \see leed_model_calculate_energies()
 *
 * \note Experimental multi-core calculations of the energy loop can be
 * enabled by defining #USE_OPENMP when compiling and linking with the
 * '-fopenmp' flag. The environment variable \c OMP_NUM_THREADS should be
 * set prior to executing the program, otherwise the maximum number of threads
 * will be used as the default setting. This can be disabled by defining
 * \c OMP_DEFAULT_IS_SERIAL when compiling.
 */

#include "leed.h"
#include <stdio.h>

#if USE_OPENMP
#include <omp.h>
#endif

static const unsigned long int IFAC = 1000000UL;

/*!
 * Checks if \c OMP_NUM_THREADS environment variable is set if compiled with
 * \c USE_OPENMP defined; the default is to have maximum number of parallel threads
 * (but minimum efficiency as speed up is not linear with added threads).
 * This behavior can be disabled by defining \c OMP_DEFAULT_IS_SERIAL==1 .
 */
static inline void leed_omp_init() {
#if USE_OPENMP
# if ! OMP_DEFAULT_IS_SERIAL
  if (getenv("OMP_NUM_THREADS") == NULL)
    omp_set_num_threads ( omp_get_max_threads() );
# endif
#endif
  return;
}

/*!
 * Prints the transmission and reflections matrices (used for debugging)
 *
 * \param Tpp Pointer to the ++ transmission matrix
 * \param Tmm Pointer to the -- transmission matrix
 * \param Rpm Pointer to the +- reflection matrix
 * \param Rmp Pointer to the -+ reflection matrix
 */
static inline void
print_scattering_matrices(mat Tpp, mat Tmm, mat Rpm, mat Rmp) {

#ifdef CONTROL_X
  fprintf(STDCTR, "(%s): after leed_ms_nd: Tpp:", LEED_NAME);
  matshow(Tpp);
  fprintf(STDCTR, "(%s): after leed_ms_nd: Tmm:", LEED_NAME);
  matshow(Tmm);
  fprintf(STDCTR, "(%s): after leed_ms_nd: Rpm:", LEED_NAME);
  matshow(Rpm);
  fprintf(STDCTR, "(%s): after leed_ms_nd: Rmp:", LEED_NAME);
  matshow(Rmp);
#endif

}

static inline void
print_layer_doubling_vectors(const cleed_real *vec, int i_layer,
    const char *type, const char *before_function) {

  if (i_layer >= 0) {
    CONTROL_MSG(CONTROL_FLOW, "%s%d before %s (%.2f %.2f %.2f)\n",
                type, i_layer, before_function,
                vec[1]*BOHR, vec[2]*BOHR, vec[3]*BOHR);
  } else {
    CONTROL_MSG(CONTROL_FLOW, "%s before %s (%.2f %.2f %.2f)\n",
           type, before_function, vec[1]*BOHR, vec[2]*BOHR, vec[3]*BOHR);
  }
}

/*!
 * Monster function which calculates the LEED beams at each energy
 *
 * \param model Pointer to the model parameters for the LEED calculations.
 */
void leed_model_calculate_energies(leed_model *model) {

  leed_crystal *bulk = model->bulk;           /* stores bulk crystal parameters */
  leed_crystal *over = model->over;           /* stores overlayer crystal parameters */
  leed_phase *phs_shifts = model->phs_shifts; /* stores phase shift information */
  leed_beam *beams_all = model->beams_all;    /* all diffracted beams */
  leed_beam *beams_out = model->beams_out;    /* all outward beams */
  leed_var *v_par = model->v_par;
  leed_energy *eng = model->energy;
  size_t n_set = model->n_set;                /* total number of beam sets */
  leed_args *args = model->args;

  leed_beam *beams_now = NULL;
  leed_beam *beams_set = NULL;       /* beams for current set */

  mat Tpp = NULL, Tmm = NULL;        /* Transmission matrices for ++ and -- */
  mat Rpm = NULL, Rmp = NULL;        /* Reflection matrices for +- and -+ */
  mat Tpp_s, Tmm_s, Rpm_s, Rmp_s;
  mat R_bulk = NULL, R_tot = NULL;
  mat Amp = NULL;

  int i_c, iaux;
  unsigned long int ienergy;
  size_t n_beams_now, n_beams_set;
  size_t i_set, offset;
  size_t i_layer;                    /* counter for crystallographic layers */

  cleed_real energy = 0.;
  cleed_real vec[4] = {0., 0., 0., 0.};

  char linebuffer[STRSZ];

  /* preset variables */
  Tpp_s =  Tmm_s =  Rpm_s =  Rmp_s = NULL;

  /* Prepare some often used parameters. */
  mk_cg_coef (2*v_par->l_max);
  mk_ylm_coef(2*v_par->l_max);

  /*===================================================================
   * Energy Loop
   *===================================================================*/
  leed_omp_init();

  /* OpenMP control loops can only iterate using integers, so energy
   * variables need to be converted using a multiplication factor.
   * The energy loop is inverted to try to sync finish when using multiple threads
   * as the higher energies take much longer to calculate than the lower ones */
#if USE_OPENMP
#  pragma omp parallel for
#endif
  for (ienergy = (unsigned long int)(eng->final*IFAC);
       ienergy > (unsigned long int)((eng->initial - E_TOLERANCE)*IFAC);
       ienergy -= (unsigned long int)(eng->step*IFAC))
  {

    /* convert back integer representation of energy */
    energy = (cleed_real) ienergy / IFAC;

    leed_par_update_nd(v_par, phs_shifts, energy);
    iaux = leed_beam_get_selection(&beams_now, beams_all, v_par, bulk->dmin);
    n_beams_now = (iaux > 0) ? (size_t)iaux : 0;

    CONTROL_MSG(CONTROL, "\n\t => E = %.1f eV (%lu beams used) <=\n\n",
        v_par->eng_v*HART, n_beams_now);

    /* BULK:
     * Loop over beam sets
     *
     * Create matrix R_bulk that will eventually contain the bulk
     * reflection matrix
     */
    R_bulk = matalloc(R_bulk, n_beams_now, n_beams_now, NUM_COMPLEX);
    for(offset = 1, i_set = 0; i_set < n_set; i_set ++)
    {
      if ( (iaux = leed_beam_set(&beams_set, beams_now, i_set)) > 0)
        n_beams_set = (size_t)iaux;
      else
        n_beams_set = 0;


      /* Loop over periodic bulk layers */

      /* Compute scattering matrices for bottom-most bulk layer:
       * - single Bravais layer or composite layer
       */
      CONTROL_MSG(CONTROL_FLOW, "bulk layer %u/%lu, set %lu/%lu\n",
          0, bulk->n_layers - 1, i_set, n_set - 1);

      if( (bulk->layers + 0)->n_atoms == 1)
      {
        leed_ms_nd( &Tpp, &Tmm, &Rpm, &Rmp,
            v_par, (bulk->layers + 0), beams_set);
      }
      else
      {
        leed_ms_compl_nd( &Tpp, &Tmm, &Rpm, &Rmp,
            v_par, (bulk->layers + 0), beams_set);
      }

      /* provide debug info on transmission and reflection matrices */
      print_scattering_matrices(Tpp, Tmm, Rpm, Rmp);

      /* Loop over the other bulk layers */
      for(i_layer = 1;
          ((bulk->layers+i_layer)->periodic == 1) && (i_layer < bulk->n_layers);
          i_layer ++)
      {
        CONTROL_MSG(CONTROL_FLOW, "[periodic] bulk layer %lu/%lu, set %lu/%lu\n",
                    i_layer, bulk->n_layers - 1, i_set, n_set - 1);

        /*
         * Compute scattering matrices R/T_s for a single bulk layer
         * - single Bravais layer or composite layer
         */
        if( (bulk->layers + i_layer)->n_atoms == 1)
        {
          leed_ms_nd ( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                        v_par, (bulk->layers + i_layer), beams_set);
        }
        else
        {
          leed_ms_compl_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                              v_par, (bulk->layers + i_layer), beams_set);
        }

        /* Add the single layer matrices to the rest by layer doubling
         * - inter layer vector is the vector between layers
         * (i_layer - 1) and (i_layer):
         * (bulk->layers + i_layer)->vec_from_last
         */
        print_layer_doubling_vectors(bulk->layers[i_layer].vec_from_last,
                                     (int)i_layer, "bulk", "leed_ld_2lay vec:");

        leed_ld_2lay( &Tpp,  &Tmm,  &Rpm,  &Rmp,
                       Tpp,   Tmm,   Rpm,   Rmp,
                       Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                       beams_set,
                       (bulk->layers + i_layer)->vec_from_last);

      } /* for i_layer (bulk) */

      /* Layer doubling for all periodic bulk layers until convergence is
       * reached:
       * - inter layer vector is (bulk->layers + 0)->vec_from_last
       */
      print_layer_doubling_vectors(bulk->layers[0].vec_from_last,
                                   -1, "bulk", "leed_ld_2n vec:");

      Rpm = leed_ld_2n( Rpm, Tpp, Tmm, Rpm, Rmp,
          beams_set, (bulk->layers + 0)->vec_from_last);

      /* Compute scattering matrices for top-most bulk layer if it is
       * not periodic.
       * - single Bravais layer or composite layer
       */
      if( i_layer == bulk->n_layers - 1 )
      {
        CONTROL_MSG(CONTROL_FLOW,
                    "(%s not periodic): bulk layer %lu/%lu, set %lu/%lu\n",
                    LEED_NAME, i_layer, bulk->n_layers - 1, i_set, n_set - 1);

        if( (bulk->layers + i_layer)->n_atoms == 1)
        {
          leed_ms_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
              v_par, (bulk->layers + i_layer), beams_set);
        }
        else
        {
          leed_ms_compl_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
              v_par, (bulk->layers + i_layer), beams_set);
        }

        /* Add the single layer matrices of the top-most layer to the rest
         * by layer doubling:
         * - inter layer vector is the vector between layers
         *    (i_layer - 1) and (i_layer):
         *    (bulk->layers + i_layer)->vec_from_last
         */
        Rpm = leed_ld_2lay_rpm(Rpm, Rpm, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
            beams_set, (bulk->layers + i_layer)->vec_from_last);

      }  /* if( i_layer == bulk->nlayers - 1 ) */

      /* Insert reflection matrix for this beam set into R_bulk. */
      R_bulk = matins(R_bulk, Rpm, offset, offset);
      offset += n_beams_set;

      /* Write cpu time to output */
      sprintf(linebuffer, "(%s): bulk layers set %lu, E = %.1f",
              LEED_NAME, i_set, energy*HART);
      leed_cpu_time(STDCPU, linebuffer);
    }  /* for i_set */

    /* OVERLAYER: Loop over all overlayer layers */
    for(i_layer = 0; i_layer < over->n_layers; i_layer ++)
    {
      CONTROL_MSG(CONTROL_FLOW, "overlayer %lu/%lu\n", i_layer, over->n_layers-1);

      /* Calculate scattering matrices for a single overlayer layer
       * - only single Bravais layer */
      if( (over->layers + i_layer)->n_atoms == 1)
      {
        leed_ms_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
            v_par, (over->layers + i_layer), beams_now);
      }
      else
      {
        leed_ms_compl_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
            v_par, (over->layers + i_layer), beams_now);
      }

      CONTROL_MSG(CONTROL_X, "overlayer %lu  ...\n", i_layer);
      print_scattering_matrices(Tpp_s, Tmm_s, Rpm_s, Rmp_s);

      /* Add the single layer matrices to the rest by layer doubling:
       * - if the current layer is the bottom-most (i_layer == 0),
       * the inter layer vector is calculated from the vectors between
       * top-most bulk layer and origin
       * ( (bulk->layers + nlayers)->vec_to_next )
       * and origin and bottom-most overlayer
       * (over->layers + 0)->vec_from_last.
       *
       * - inter layer vector is the vector between layers
       * (i_layer - 1) and (i_layer): (over->layers + i_layer)->vec_from_last
       */

      if (i_layer == 0)
      {
        for(i_c = 1; i_c <= 3; i_c ++)
        {
          vec[i_c] = (bulk->layers + bulk->n_layers - 1)->vec_to_next[i_c] +
              (over->layers + 0)->vec_from_last[i_c];
        }

        print_layer_doubling_vectors(vec, 0, "over", "leed_ld_2lay_rpm vec:");

        R_tot = leed_ld_2lay_rpm(R_tot, R_bulk, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                                 beams_now, vec);
      }
      else
      {
        print_layer_doubling_vectors(over->layers[i_layer].vec_from_last,
                               (int)i_layer, "over", "leed_ld_2lay_rpm vec:");

        R_tot = leed_ld_2lay_rpm(R_tot, R_tot, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
            beams_now, (over->layers + i_layer)->vec_from_last);
      }

      /* Write cpu time to output */
      sprintf(linebuffer, "(%s): overlayer %lu, E = %.1f",
              LEED_NAME, i_layer, energy * HART);
      leed_cpu_time(STDCPU, linebuffer);

    }  /* for i_layer (overlayer) */

    /* Add propagation towards the potential step. */
    vec[1] = vec[2] = 0.;
    vec[3] = 1.25 / BOHR;

    /* No scattering at potential step */
    Amp = leed_ld_potstep0(Amp, R_tot, beams_now, v_par->eng_v, vec);
    leed_output_intensities(Amp, beams_now, beams_out, v_par, args->res_stream);

    /* Write cpu time to output */
    sprintf(linebuffer, "  %.1f   %lu  ", energy * HART, n_beams_now);
    leed_cpu_time(STDWAR, linebuffer);

  } /* end of energy loop */

  fclose(args->res_stream);

  CONTROL_MSG(CONTROL, "\n\n(%s):\tCORRECT TERMINATION", LEED_NAME);

  /* Write cpu time to output */
  leed_cpu_time(STDCPU, "");
}
