/*********************************************************************
GH/05.10.00
  file contains functions:

  main 
    Main program for LEED calculations (only for bravaislayer)
    using non-symmetrised code.
    
Changes:

GH/05.10.00 - early return option (-e).
LD/27.03.14 - added OMP parallel for loop for energy and inverted
              (use '-D_USE_OPENMP' & '-fopenmp' flags when compiling)
LD/02.04.14 - added '--help', '-h' & '-V' options for usage and info,
              respectively (added functions usage() & info() )

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gh_stddef.h"
#include "leed.h"
#include "leed_ver.h"   /* version information */
#include "proghelp.h"

#ifdef _USE_OPENMP
#include <omp.h>        /* compile with '-fopenmp' */
#endif

#define CONTROL_FLOW

#define CONTROL_IO

#ifndef CONTROL 
#define CONTROL
#endif

#define WARNING
#define ERROR

#define CTR_NORMAL       998
#define CTR_EARLY_RETURN 999

/*======================================================================*/

int main(int argc, char *argv[])

/*********************************************************************
 Perform a LEED calculation for anisotropic vibrations a general case
 
 INPUT:

 DESIGN:
 
*********************************************************************/
{
leed_cryst_t *bulk;
leed_cryst_t *over;
leed_phs_t *phs_shifts;
leed_beam_t *beams_all;
leed_beam_t *beams_out;
leed_beam_t *beams_now;
leed_beam_t *beams_set;
leed_var_t *v_par;
leed_energy_t *eng;

mat Tpp,   Tmm,   Rpm,   Rmp;
mat Tpp_s, Tmm_s, Rpm_s, Rmp_s;
mat R_bulk, R_tot;
mat Amp;

int ctr_flag;
int i_c, i_arg;
int n_beams_now, n_beams_set;
int i_set, n_set, offset;
int i_layer;

real energy;
real vec[4];

char linebuffer[STRSZ];

char bul_file[STRSZ];                 /* input/output files */
char par_file[STRSZ];
char pro_name[STRSZ];
char res_file[STRSZ];

FILE *res_stream;

  Tpp   =  Tmm   =  Rpm   =  Rmp   = NULL;
  Tpp_s =  Tmm_s =  Rpm_s =  Rmp_s = NULL;
  R_bulk = R_tot = NULL;
  Amp = NULL;

  res_stream = NULL;
  bulk = over = NULL;
  phs_shifts  = NULL;
  beams_all   = NULL;
  beams_now   = NULL;
  beams_set   = NULL;
  beams_out   = NULL;
  v_par = NULL;
  eng   = NULL;


/*********************************************************************
  Preset parameters (file names) set by arguments
*********************************************************************/

  ctr_flag = CTR_NORMAL;

  strncpy(bul_file,"---", STRSZ);

  strncpy(par_file,"---", STRSZ);
  strncpy(res_file,"leed.res", STRSZ);
  strncpy(pro_name,"leed.pro", STRSZ);

/*********************************************************************
  Decode arguments:

    -b <bul_file> - (optional input file) bulk and non-geometrical 
                    parameters.
    -i <par_file> - (mandatory input file) overlayer parameters of all 
                    parameters (if bul_file does not exist).
    -o <res_file> - (output file) IV output.
*********************************************************************/

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
#ifdef ERROR
      fprintf(STDERR,"*** error (CLEED_NSYM):\tsyntax error:\n");
      fprintf(STDERR,"\tusage: \tcleed -i <par_file> -o <res_file>");
      fprintf(STDERR," [-b <bul_file> -e]\n");
#endif
      exit(1);
    }
    else
    {

/* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
         strcmp(argv[i_arg], "--help") == 0) 
      {
        usage(stdout);
        exit(0);
      }

/* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 || 
         strcmp(argv[i_arg], "--version") == 0)
      {
        info(); 
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
#ifdef ERROR
          fprintf(STDERR,
          "*** error (CLEED_NSYM): could not open output file \"%s\"\n",
          res_file);
#endif
          exit(1);
        }
      }  /* -o */

/* Read parameter input file */
      if(strncmp(argv[i_arg], "-e", 2) == 0)
      {
        ctr_flag = CTR_EARLY_RETURN;
      } /* -e */


    }  /* else */
  }  /* for i_arg */

/*********************************************************************
  Check arguments:
  - check existence of par_file.
  - if bul_file is not specified, use par_file instead.
  - check existence of res_file.
*********************************************************************/

  if(strncmp(par_file, "---", 3) == 0)
  {
#ifdef ERROR
    fprintf(STDERR,
    "*** error (CLEED_NSYM): no parameter input file (option -i) specified\n");
#endif
    exit(1);
  }

  if(strncmp(bul_file, "---", 3) == 0)
  {
    strncpy(bul_file, par_file, STRSZ);
  }

  if(strncmp(res_file, "leed.res", 8) == 0)
  {
#ifdef WARNING
    fprintf(STDWAR,
            "* warning (CLEED_NSYM): no output file (option -o) specified\n");
    fprintf(STDWAR,"\toutput will be written to file \"%s\"\n", res_file);
#endif

    if ((res_stream = fopen(res_file,"w")) == NULL)
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (CLEED_NSYM): could not open output file \"%s\"\n",
      res_file);
#endif
     exit(1);
    } 
  }

/*********************************************************************
  Read input parameters
*********************************************************************/

  leed_inp_read_bul_nd(&bulk, &phs_shifts, bul_file);
  leed_inp_leed_read_par(&v_par, &eng, bulk, bul_file);
  leed_read_overlayer_nd(&over, &phs_shifts, bulk, par_file);
  n_set = leed_beam_gen(&beams_all, bulk, v_par, eng->fin);
   
  leed_inp_show_beam_op(bulk, over, phs_shifts);

  if( ctr_flag == CTR_EARLY_RETURN )
  {
    fprintf(STDCTR, "(CLEED_NSYM): EARLY RETURN \n");
    exit(0);
  }

  leed_out_head(res_stream);
  leed_output_beam_list(&beams_out, beams_all, eng, res_stream);

/*********************************************************************
 Prepare some often used parameters.
*********************************************************************/

  mk_cg_coef (2*v_par->l_max);
  mk_ylm_coef(2*v_par->l_max);

#ifdef CONTROL
  fprintf(STDCTR, "(CLEED_NSYM): E_ini = %.1f, E_fin = %.1f, E_stp %.1f\n", 
          eng->ini*HART, eng->fin*HART, eng->stp*HART);

  fprintf(STDCTR, "(CLEED_NSYM): n_set = %d\n", n_set);
#endif

/*********************************************************************
 Energy Loop
*********************************************************************/

#ifdef _USE_OPENMP
  
  /* OpenMP loop index must be a signed integral type on MSVC. */
  const real energy_start = eng->ini;
  const real energy_stop = eng->fin + E_TOLERANCE;
  const real energy_step = eng->stp;

  int n_steps = 0;
  if (energy_step < 0)
  {
    const real delta = energy_start - energy_stop;
    if (delta > 0)
    {
      const real steps_real = ceil(delta / (-energy_step));
      if (steps_real < 2147483647.0)
      {
        n_steps = (int)steps_real;
      }
    }
  }

  #pragma omp parallel for
  for (int step_index = 0; step_index < n_steps; step_index++)
  {
    energy = energy_start + (real)step_index * energy_step;
#else
  for( energy = eng->ini; energy > eng->fin + E_TOLERANCE; energy += eng->stp)
  {
#endif

    leed_par_update_nd(v_par, phs_shifts, energy);
    n_beams_now = leed_beam_get_selection(&beams_now, beams_all, v_par, bulk->dmin);

#ifdef CONTROL
    fprintf(STDCTR, "(CLEED_NSYM):\n\t => E = %.1f eV (%d beams used) <=\n\n",
              v_par->eng_v*HART, n_beams_now);
#endif
  

/*********************************************************************
  BULK:
  Loop over beam sets

  Create matrix R_bulk that will eventually contain the bulk 
  reflection matrix 
*********************************************************************/

    R_bulk = matalloc(R_bulk, n_beams_now, n_beams_now, NUM_COMPLEX);
    for(offset = 1, i_set = 0; i_set < n_set; i_set ++)
    {
      n_beams_set = leed_beam_set(&beams_set, beams_now, i_set);

/*********************************************************************
    Loop over periodic bulk layers
*********************************************************************/

    /**********************************************************
     Compute scattering matrices for bottom-most bulk layer:
     - single Bravais layer or composite layer
    **********************************************************/

#ifdef CONTROL_FLOW
      fprintf(STDCTR, "(CLEED_NSYM periodic): bulk layer %d/%d, set %d/%d\n", 
                        0, bulk->nlayers - 1, i_set, n_set - 1);
#endif
        
      if( (bulk->layers + 0)->natoms == 1)
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
      fprintf(STDCTR, "(CLEED_NSYM): after leed_ms_nd: Tpp:");
      matshow(Tpp);
      fprintf(STDCTR, "(CLEED_NSYM): after leed_ms_nd: Tmm:");
      matshow(Tmm);
      fprintf(STDCTR, "(CLEED_NSYM): after leed_ms_nd: Rpm:");
      matshow(Rpm);
      fprintf(STDCTR, "(CLEED_NSYM): after leed_ms_nd: Rmp:");
      matshow(Rmp);
#endif
       
    /**********************************************************
      Loop over the other bulk layers 
    **********************************************************/

      for(i_layer = 1; 
          ( (bulk->layers+i_layer)->periodic == 1) && 
          (i_layer < bulk->nlayers); 
          i_layer ++)
      {
#ifdef CONTROL_FLOW
        fprintf(STDCTR, "(CLEED_NSYM periodic): bulk layer %d/%d, set %d/%d\n", 
                        i_layer, bulk->nlayers - 1, i_set, n_set - 1);
#endif

    /************************************************************** 
      Compute scattering matrices R/T_s for a single bulk layer 
       - single Bravais layer or composite layer
    ***************************************************************/

        if( (bulk->layers + i_layer)->natoms == 1)
        {
          leed_ms_nd ( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                        v_par, (bulk->layers + i_layer), beams_set);
        }
        else
        {
          leed_ms_compl_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                       v_par, (bulk->layers + i_layer), beams_set);
        }

    /*************************************************************************** 
       Add the single layer matrices to the rest by layer doubling 
       - inter layer vector is the vector between layers
         (i_layer - 1) and (i_layer): 
         (bulk->layers + i_layer)->vec_from_last
    ****************************************************************************/ 
#ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(CLEED_NSYM): before leed_ld_2lay vec_from...(%.2f %.2f %.2f)\n",
                       (bulk->layers + i_layer)->vec_from_last[1] * BOHR,
                       (bulk->layers + i_layer)->vec_from_last[2] * BOHR,
                       (bulk->layers + i_layer)->vec_from_last[3] * BOHR); 
#endif

        leed_ld_2lay( &Tpp,  &Tmm,  &Rpm,  &Rmp,
                 Tpp,   Tmm,   Rpm,   Rmp,
                 Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                 beams_set, (bulk->layers + i_layer)->vec_from_last);

      } /* for i_layer (bulk) */

   /********************************************************************* 
      Layer doubling for all periodic bulk layers until convergence is 
      reached:
       - inter layer vector is (bulk->layers + 0)->vec_from_last
   **********************************************************************/
#ifdef CONTROL_FLOW
      fprintf(STDCTR, "(CLEED_NSYM): before leed_ld_2n vec_from...(%.2f %.2f %.2f)\n",
                      (bulk->layers + 0)->vec_from_last[1] * BOHR,
                      (bulk->layers + 0)->vec_from_last[2] * BOHR,
                      (bulk->layers + 0)->vec_from_last[3] * BOHR);
#endif

      Rpm = leed_ld_2n( Rpm, Tpp, Tmm, Rpm, Rmp, 
                   beams_set, (bulk->layers + 0)->vec_from_last);

   /*******************************************************************
     Compute scattering matrices for top-most bulk layer if it is
     not periodic.
      - single Bravais layer or composite layer
   **********************************************************************/

      if( i_layer == bulk->nlayers - 1 )
      {
#ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(CLEED_NSYM not periodic): bulk layer %d/%d, set %d/%d\n", 
                i_layer, bulk->nlayers - 1, i_set, n_set - 1);
#endif
    
        if( (bulk->layers + i_layer)->natoms == 1)
        {
          leed_ms_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                    v_par, (bulk->layers + i_layer), beams_set);
        }
        else
        {
          leed_ms_compl_nd( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
                       v_par, (bulk->layers + i_layer), beams_set);
        }
   
    /**************************************************************************
       Add the single layer matrices of the top-most layer to the rest 
       by layer doubling:
       - inter layer vector is the vector between layers
         (i_layer - 1) and (i_layer): 
         (bulk->layers + i_layer)->vec_from_last
    ***************************************************************************/

        Rpm = leed_ld_2lay_rpm(Rpm, Rpm, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                          beams_set, (bulk->layers + i_layer)->vec_from_last);

      }  /* if( i_layer == bulk->nlayers - 1 ) */

   /*******************************************************
     Insert reflection matrix for this beam set into R_bulk.
   ********************************************************/

      R_bulk = matins(R_bulk, Rpm, offset, offset);
      offset += n_beams_set;

   /*************************
     Write cpu time to output
   **************************/

      sprintf(linebuffer,"(CLEED_NSYM): bulk layers set %d, E = %.1f", 
              i_set, energy*HART);
      leed_cpu_time(STDCPU,linebuffer);
    }  /* for i_set */
      
/*********************************************************************
  OVERLAYER
  Loop over all overlayer layers
*********************************************************************/

    for(i_layer = 0; i_layer < over->nlayers; i_layer ++)
    {
#ifdef CONTROL_FLOW
      fprintf(STDCTR, "(CLEED_NSYM): overlayer %d/%d\n", i_layer, over->nlayers - 1);
#endif
   /***********************************************************
     Calculate scattering matrices for a single overlayer layer
      - only single Bravais layer 
   ************************************************************/
      
      if( (over->layers + i_layer)->natoms == 1)
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
 fprintf(STDCTR, "\n(CLEED_NSYM):overlayer %d  ...\n",i_layer);
 fprintf(STDCTR, "\n(CLEED_NSYM): Tpp:\n");
 matshowabs(Tpp_s);
 fprintf(STDCTR, "\n(CLEED_NSYM): Tmm:\n");
 matshowabs(Tmm_s);
 fprintf(STDCTR, "\n(CLEED_NSYM): Rpm:\n");
 matshowabs(Rpm_s);
 fprintf(STDCTR, "\n(CLEED_NSYM): Rmp:\n");
 matshowabs(Rmp_s);
#endif

  /****************************************************************
     Add the single layer matrices to the rest by layer doubling:
     - if the current layer is the bottom-most (i_layer == 0),
       the inter layer vector is calculated from the vectors between
       top-most bulk layer and origin 
       ( (bulk->layers + nlayers)->vec_to_next )
       and origin and bottom-most overlayer
       (over->layers + 0)->vec_from_last.

     - inter layer vector is the vector between layers
       (i_layer - 1) and (i_layer): (over->layers + i_layer)->vec_from_last
  **********************************************************************/

      if (i_layer == 0)
      {
        for(i_c = 1; i_c <= 3; i_c ++)
        {
          vec[i_c] = (bulk->layers + bulk->nlayers - 1)->vec_to_next[i_c]
                     + (over->layers + 0)->vec_from_last[i_c];
        }

#ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(LEED):over0 before leed_ld_2lay_rpm vec..(%.2f %.2f %.2f)\n",
                vec[1] * BOHR,vec[2] * BOHR, vec[3] * BOHR);
#endif

        R_tot = leed_ld_2lay_rpm(R_tot, R_bulk, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                            beams_now, vec);
      }
      else
      {
#ifdef CONTROL_FLOW
        fprintf(STDCTR, 
                "(LEED):over%d  before leed_ld_2lay_rpm vec..(%.2f %.2f %.2f)\n",
                i_layer,(over->layers + i_layer)->vec_from_last[1] * BOHR,
                        (over->layers + i_layer)->vec_from_last[2] * BOHR,
                        (over->layers + i_layer)->vec_from_last[3] * BOHR); 
#endif

        R_tot = leed_ld_2lay_rpm(R_tot, R_tot, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
                          beams_now, (over->layers + i_layer)->vec_from_last);
      }

   /**************************
     Write cpu time to output
   **************************/

      sprintf(linebuffer,"(LEED): overlayer %d, E = %.1f", 
              i_layer, energy * HART);
      leed_cpu_time(STDCPU,linebuffer);

    }  /* for i_layer (overlayer) */

/*********************************************
   Add propagation towards the potential step.
**********************************************/

    vec[1] = vec[2] = 0.;
    vec[3] = 1.25 / BOHR;

/********************************************
    No scattering at pot. step 
********************************************/

    Amp = leed_ld_potstep0(Amp, R_tot, beams_now, v_par->eng_v, vec);
    leed_output_int(Amp, beams_now, beams_out, v_par, res_stream);

/********************************************
    Write cpu time to output 
********************************************/

    sprintf(linebuffer,"  %.1f   %d  ",energy * HART,n_beams_now);
    leed_cpu_time(STDWAR,linebuffer);


  } /* end of energy loop */


#ifdef CONTROL_IO
  fprintf(STDCTR, "(LEED): end of energy loop: close files\n");
#endif

  fclose(res_stream);

#ifdef CONTROL
  fprintf(STDCTR, "\n\n(LEED):\tCORRECT TERMINATION");
#endif

/********************************************
    Write cpu time to output 
********************************************/

  leed_cpu_time(STDCPU,"");

/********************************************
    set exit status explicitly 
********************************************/

  exit(0);
  return(0);

} /* end of main */
