/*********************************************************************
 *                        CLEED_NSYM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/05.10.00 - early return option (-e).
 *   LD/27.03.14 - added OMP parallel for loop for energy and inverted
 *                 (use '-D_USE_OPENMP' & '-fopenmp' flags when compiling)
 *   LD/02.04.14 - added '--help', '-h' & '-V' options for usage and info,
 *                 respectively (added functions usage() & info() )
 *********************************************************************/

/*! \file
 *
 * Main program for LEED calculations (only for bravaislayer() )
 * using non-symmetrised code.
 *
 * \note Experimental multi-core calculations of the energy loop can be
 * enabled by defining #USE_OPENMP when compiling and linking with the
 * '-fopenmp' flag. The environment variable \c OMP_NUM_THREADS should be
 * set prior to executing the program, otherwise the maximum number of threads
 * will be used as the default setting. This can be disabled by defining
 * \c OMP_DEFAULT_IS_SERIAL when compiling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gh_stddef.h"
#include "leed.h"
#include "leed_ver.h"   /* version information */
#include "leed_help.h"

#ifdef USE_OPENMP
#include <omp.h>        /* compile with '-fopenmp' */
#endif


enum { CTR_NORMAL = 998, CTR_EARLY_RETURN = 999 };


/*!
 * Perform a LEED calculation for anisotropic vibrations (general case)
 */
int main(int argc, char *argv[])
{
  leed_crystal *bulk = NULL;         /* stores bulk crystal parameters */
  leed_crystal *over = NULL;         /* stores overlayer crystal parameters */
  leed_phase *phs_shifts = NULL;     /* stores phase shift information */
  leed_beam *beams_all = NULL;       /* all diffracted beams */
  leed_beam *beams_out = NULL;       /* all outward beams */
  leed_beam *beams_now = NULL;
  leed_beam *beams_set = NULL;       /* beams for current set */
  leed_var *v_par = NULL;
  leed_energy *eng = NULL;

  mat Tpp = NULL, Tmm = NULL;        /* Transmission matrices for ++ and -- */
  mat Rpm = NULL, Rmp = NULL;        /* Reflection matrices for +- and -+ */
  mat Tpp_s, Tmm_s, Rpm_s, Rmp_s;
  mat R_bulk = NULL, R_tot = NULL;
  mat Amp = NULL;

  int ctr_flag = CTR_NORMAL;
  int i_c, i_arg, iaux;
  size_t n_beams_now, n_beams_set;
  size_t i_set, offset;
  size_t i_layer;                    /* counter for crystallographic layers */
  size_t n_set = 0;                  /* total number of beam sets */

  real energy = 0.;
  real vec[4] = {0., 0., 0., 0.};

  char linebuffer[STRSZ];

  char bul_file[STRSZ] = "---";      /* file path to bulk input file */
  char par_file[STRSZ] = "---";      /* file path to overlayer input file */
  char pro_name[STRSZ] = "leed.pro";
  char res_file[STRSZ] = "leed.res";

  FILE *pro_stream = NULL;
  FILE *res_stream = NULL;          /* results stream */

  /* preset variables */
  Tpp_s =  Tmm_s =  Rpm_s =  Rmp_s = NULL;

  /* Decode arguments:
   *
   * -b <bul_file> - (optional input file) bulk and non-geometrical
   *                 parameters.
   * -i <par_file> - (mandatory input file) overlayer parameters of all
   *                 parameters (if bul_file does not exist).
   * -o <res_file> - (output file) IV output.
   */

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
      #ifdef ERROR_LOG
      leed_usage(stderr);
      #endif
      exit(1);
    }
    else
    {

      /* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
         strcmp(argv[i_arg], "--help") == 0) 
      {
        leed_usage(stdout);
        exit(0);
      }

      /* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 || 
         strcmp(argv[i_arg], "--version") == 0)
      {
        leed_info();
        exit(0);
      }
      
      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-b", 2) == 0)
      {
        i_arg++;
        strncpy(bul_file, argv[i_arg], STRSZ);
      } /* -b */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        i_arg++;
        strncpy(par_file, argv[i_arg], STRSZ);
      } /* -i */

      /* Read and open results file */
      if(strncmp(argv[i_arg], "-o", 2) == 0)
      {
        i_arg++;
        strncpy(res_file, argv[i_arg], STRSZ);
        if ((res_stream = fopen(res_file,"w")) == NULL)
        {
          #ifdef ERROR_LOG
          fprintf(STDERR, "*** error (%s): "
                  "could not open output file \"%s\"\n", LEED_NAME, res_file);
          #endif
          exit(1);
        }
      }  /* -o */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-e", 2) == 0)
      {
        ctr_flag = CTR_EARLY_RETURN;
      } /* -e */
      /* Read project name for reading matrices */
      if(strncmp(argv[i_arg], "-r", 2) == 0)
      {
        i_arg++;
        strncpy(pro_name, argv[i_arg], STRSZ);
        if ((pro_stream = fopen(pro_name, "r")) == NULL)
        {
          #ifdef ERROR_LOG
          fprintf(STDERR, "*** error (%s): "
                  "could not open project file \"%s\" for reading\n",
                  LEED_NAME, pro_name);
          #endif
          exit(1);
        }
      } /* -r */

      /* Read project name for writing matrices */
      if(strncmp(argv[i_arg], "-w", 2) == 0)
      {
        i_arg++;
        strncpy(pro_name, argv[i_arg], STRSZ);
        if ((pro_stream = fopen(pro_name,"w")) == NULL)
        {
          #ifdef ERROR_LOG
          fprintf(STDERR, "*** error (%s): "
                  "could not open project file \"%s\" for writing\n",
                  LEED_NAME, pro_name);
          #endif
          exit(1);
        }
      } /* -w */

    }  /* else */
  }  /* for i_arg */

  /* Check arguments:
   * - check existence of par_file.
   * - if bul_file is not specified, use par_file instead.
   * - check existence of res_file.
   */
  if(strncmp(par_file, "---", 3) == 0)
  {
    #ifdef ERROR_LOG
    fprintf(STDERR, "***error (%s): "
            "no parameter input file (option -i) specified\n", LEED_NAME);
    #endif
    exit(1);
  }

  if(strncmp(bul_file, "---", 3) == 0)
  {
    strncpy(bul_file, par_file, STRSZ);
  }

  if(strncmp(res_file, "leed.res", 8) == 0)
  {
    #ifdef WARNING_LOG
    fprintf(STDWAR, "*warning (%s): "
            "no output file (option -o) specified\n", LEED_NAME);
    fprintf(STDWAR, "\toutput will be written to file \"%s\"\n", res_file);
    #endif

    if ((res_stream = fopen(res_file,"w")) == NULL)
    {
      #ifdef ERROR_LOG
      fprintf(STDERR, "***error (%s): "
              "could not open output file \"%s\"\n", LEED_NAME, res_file);
      #endif
      exit(1);
    } 
  }

  /*============================================================
   * Read input parameters
   *============================================================*/
  leed_inp_read_bul_nd(&bulk, &phs_shifts, bul_file);
  leed_inp_leed_read_par(&v_par, &eng, bulk, bul_file);
  leed_read_overlayer_nd(&over, &phs_shifts, bulk, par_file);

  if ( (iaux = leed_beam_gen(&beams_all, bulk, v_par, eng->final)) > 0)
    n_set = (size_t)iaux;
  else n_set = 0;
   
  leed_inp_show_beam_op(bulk, over, phs_shifts);

  if( ctr_flag == CTR_EARLY_RETURN )
  {
    fprintf(STDCTR, "(%s): EARLY RETURN \n", LEED_NAME);
    exit(0);
  }

  leed_output_header(res_stream);
  leed_output_beam_list(&beams_out, beams_all, eng, res_stream);

  /* Prepare some often used parameters. */
  mk_cg_coef (2*v_par->l_max);
  mk_ylm_coef(2*v_par->l_max);


  #ifdef CONTROL
  fprintf(STDCTR, "(%s): E_ini = %.1f, E_fin = %.1f, E_stp %.1f\n", 
          LEED_NAME, eng->initial*HART, eng->final*HART, eng->step*HART);

  fprintf(STDCTR, "(%s): n_set = %d\n", LEED_NAME, n_set);
  #endif

  /*===================================================================
   * Energy Loop
   *===================================================================*/
  #ifdef USE_OPENMP
  /*
   * OpenMP control loops can only iterate using integers, so energy
   * variables need to be converted using a multiplication factor
   */

  /*!
   * Check if \c OMP_NUM_THREADS environment variable is set if compiled with
   * \c USE_OPENMP defined; the default is to have maximum parallization
   * (but minimum efficiency as speed up is not linear with added threads).
   * This behavior can be disabled by defining \c OMP_DEFAULT_IS_SERIAL .
   */
  #ifndef OMP_DEFAULT_IS_SERIAL
  if (getenv(OMP_NUM_THREADS) == NULL)
    omp_set_num_threads ( omp_get_max_threads() );
  #endif

  #define IFAC 1e6
  unsigned long int ienergy, istep, istop;
  istep = (unsigned long int) eng->step * IFAC;
  istop = (unsigned long int) (eng->initial - E_TOLERANCE) * IFAC;
  
  #pragma omp parallel for 
  for( ienergy = eng->final*IFAC; ienergy > istop;
       ienergy -= istep) /* energy loop inverted to try to sync finish */
  {
    energy = (real) ienergy/IFAC; /* convert back integer representation of energy */
  #else /* USE_OPENMP */
  for( energy = eng->initial;
       energy > eng->final + E_TOLERANCE;
       energy += eng->step)
  {
  #endif

    leed_par_update_nd(v_par, phs_shifts, energy);
    iaux = leed_beam_get_selection(&beams_now, beams_all, v_par, bulk->dmin);
    n_beams_now = (iaux > 0) ? (size_t)iaux : 0;

    #ifdef CONTROL
    fprintf(STDCTR, "(CLEED_NSYM):\n\t => E = %.1f eV (%d beams used) <=\n\n",
              v_par->eng_v*HART, n_beams_now);
    #endif
  
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
      #ifdef CONTROL_FLOW
      fprintf(STDCTR, "(%s): bulk layer %d/%d, set %d/%d\n",
              LEED_NAME, 0, bulk->n_layers - 1, i_set, n_set - 1);
      #endif
        
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
       
      /* Loop over the other bulk layers */
      for(i_layer = 1; 
          ( (bulk->layers+i_layer)->periodic == 1) && 
          (i_layer < bulk->n_layers);
          i_layer ++)
      {
        #ifdef CONTROL_FLOW
        fprintf(STDCTR, "(%s): [periodic] bulk layer %d/%d, set %d/%d\n", 
                LEED_NAME, i_layer, bulk->n_layers - 1, i_set, n_set - 1);
        #endif

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
        #ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                       "(%s): before leed_ld_2lay vec_from...(%.2f %.2f %.2f)\n", 
                       LEED_NAME,
                       (bulk->layers + i_layer)->vec_from_last[1] * BOHR,
                       (bulk->layers + i_layer)->vec_from_last[2] * BOHR,
                       (bulk->layers + i_layer)->vec_from_last[3] * BOHR); 
        #endif

        leed_ld_2lay( &Tpp,  &Tmm,  &Rpm,  &Rmp,
                 Tpp,   Tmm,   Rpm,   Rmp,
                 Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                 beams_set, (bulk->layers + i_layer)->vec_from_last);

      } /* for i_layer (bulk) */

      /* Layer doubling for all periodic bulk layers until convergence is
       * reached:
       * - inter layer vector is (bulk->layers + 0)->vec_from_last
       */
      #ifdef CONTROL_FLOW
      fprintf(STDCTR, "(%s): before leed_ld_2n vec_from...(%.2f %.2f %.2f)\n",
                       LEED_NAME,
                      (bulk->layers + 0)->vec_from_last[1] * BOHR,
                      (bulk->layers + 0)->vec_from_last[2] * BOHR,
                      (bulk->layers + 0)->vec_from_last[3] * BOHR);
      #endif

      Rpm = leed_ld_2n( Rpm, Tpp, Tmm, Rpm, Rmp, 
                   beams_set, (bulk->layers + 0)->vec_from_last);

      /* Compute scattering matrices for top-most bulk layer if it is
       * not periodic.
       * - single Bravais layer or composite layer
       */
      if( i_layer == bulk->n_layers - 1 )
      {
        #ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(%s not periodic): bulk layer %d/%d, set %d/%d\n", 
                LEED_NAME, i_layer, bulk->n_layers - 1, i_set, n_set - 1);
        #endif
    
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
      sprintf(linebuffer,"(%s): bulk layers set %d, E = %.1f", 
              LEED_NAME, i_set, energy*HART);
      leed_cpu_time(STDCPU,linebuffer);
    }  /* for i_set */
      
    /* OVERLAYER: Loop over all overlayer layers */
    for(i_layer = 0; i_layer < over->n_layers; i_layer ++)
    {
      #ifdef CONTROL_FLOW
      fprintf(STDCTR, "(%s): overlayer %d/%d\n",
              LEED_NAME, i_layer, over->n_layers - 1);
      #endif

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

      #ifdef CONTROL_X
      fprintf(STDCTR, "\n(%s): overlayer %d  ...\n", LEED_NAME, i_layer);
      fprintf(STDCTR, "\n(%s): Tpp:\n", LEED_NAME);
      matshowabs(Tpp_s);
      fprintf(STDCTR, "\n(%s): Tmm:\n", LEED_NAME);
      matshowabs(Tmm_s);
      fprintf(STDCTR, "\n(%s): Rpm:\n", LEED_NAME);
      matshowabs(Rpm_s);
      fprintf(STDCTR, "\n(%s): Rmp:\n", LEED_NAME);
      matshowabs(Rmp_s);
      #endif

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
          vec[i_c] = (bulk->layers + bulk->n_layers - 1)->vec_to_next[i_c]
                     + (over->layers + 0)->vec_from_last[i_c];
        }

        #ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(%s):over0 before leed_ld_2lay_rpm vec..(%.2f %.2f %.2f)\n",
                LEED_NAME, vec[1] * BOHR,vec[2] * BOHR, vec[3] * BOHR);
        #endif

        R_tot = leed_ld_2lay_rpm(R_tot, R_bulk, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                            beams_now, vec);
      }
      else
      {

        #ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(%s): over%d  before leed_ld_2lay_rpm vec.."
                "(%.2f %.2f %.2f)\n", LEED_NAME,
                i_layer,(over->layers + i_layer)->vec_from_last[1] * BOHR,
                        (over->layers + i_layer)->vec_from_last[2] * BOHR,
                        (over->layers + i_layer)->vec_from_last[3] * BOHR); 
        #endif

        R_tot = leed_ld_2lay_rpm(R_tot, R_tot, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                          beams_now, (over->layers + i_layer)->vec_from_last);
      }

      /* Write cpu time to output */
      sprintf(linebuffer, "(%s): overlayer %d, E = %.1f", 
              LEED_NAME, i_layer, energy * HART);
      leed_cpu_time(STDCPU,linebuffer);

    }  /* for i_layer (overlayer) */

    /* Add propagation towards the potential step. */
    vec[1] = vec[2] = 0.;
    vec[3] = 1.25 / BOHR;

    /* No scattering at pot. step */
    Amp = leed_ld_potstep0(Amp, R_tot, beams_now, v_par->eng_v, vec);
    leed_output_intensities(Amp, beams_now, beams_out, v_par, res_stream);

    /* Write cpu time to output */
    sprintf(linebuffer, "  %.1f   %d  ", energy * HART, n_beams_now);
    leed_cpu_time(STDWAR,linebuffer);

  } /* end of energy loop */


  #ifdef CONTROL_IO
  fprintf(STDCTR, "(%s): end of energy loop: close files\n", LEED_NAME);
  #endif

  fclose(res_stream);

  #ifdef CONTROL
  fprintf(STDCTR, "\n\n(%s):\tCORRECT TERMINATION", LEED_NAME);
  #endif

  /* Write cpu time to output */
  leed_cpu_time(STDCPU,"");

  /* set exit status explicitly  */
  exit(0);
  return(0);

} /* end of main */
