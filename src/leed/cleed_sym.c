/************************************************************************
 * <FILENAME>
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *27.09.00
  file contains functions:

  main (27.07.95)
     Main program for symmetrised LEED calculations

Changes:
 GH/27.07.95 - Creation
 GH/20.11.95 - set exit status explicitly to 0 upon correct termination.
 GH/03.09.97 - start to include symmetrisation (ROTATION).
 WB/24.03.98 - include function select_rot.
 WB/26.03.98 - prototype of function select_sym
                    search for rotation symmetry
                    set registry shifts
 WB/02.06.98 - new argument rot in leed_output_int (look row 696)
 WB/01.09.98 - mirrorsym

version 1.1
 GH/27.09.00 - version 1.1
 LD/21.04.14 - added --help and --version arguments
 *********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gh_stddef.h"
#include "leed.h"
#include "leed_def.h"
#include "leed_ver_sym.h" /* defines LEED_VERSION and LEED_NAME */
#include "leed_help.h"

enum {
  FLAG_NONE=0,
  FLAG_READ,
  FLAG_WRITE
};


int main(int argc, char *argv[])

/*********************************************************************
 Perform a LEED calculation for a general case

 INPUT:

 DESIGN:

 *********************************************************************/
{
  leed_crystal *bulk;
  leed_crystal *over;
  leed_phase *phs_shifts;
  leed_beam *beams_all;
  leed_beam *beams_out;
  leed_beam *beams_now;
  leed_beam *beams_set;
  leed_var *v_par;
  leed_energy *eng;

  mat Tpp,   Tmm,   Rpm,   Rmp;
  mat Tpp_s, Tmm_s, Rpm_s, Rmp_s;
  mat R_bulk, R_tot;
  mat Amp;

  int ctr_flag;  /****i wegen control****/
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

  FILE *pro_stream;
  FILE *res_stream;

  res_stream = pro_stream = NULL;

  Tpp =  Tmm =  Rpm =  Rmp = NULL;
  Tpp_s =  Tmm_s =  Rpm_s =  Rmp_s = NULL;
  R_bulk = R_tot = NULL;
  Amp = NULL;

  bulk = over = NULL;
  phs_shifts = NULL;
  beams_all = NULL;
  beams_now = NULL;
  beams_set = NULL;
  beams_out = NULL;
  v_par = NULL;
  eng = NULL;

  n_set = 0;

  /*********************************************************************
  Preset parameters set by arguments
   *********************************************************************/

  /**************************
   Write cpu time to output
   ***************************/
  sprintf(linebuffer,"(%s): start program", LEED_NAME);
  leed_cpu_time(STDCPU,linebuffer);

  strncpy(bul_file,"---", STRSZ);

  strncpy(par_file,"---", STRSZ);
  strncpy(res_file,"leed.res", STRSZ);
  strncpy(pro_name,"leed.pro", STRSZ);

  ctr_flag = FLAG_NONE;

  /*********************************************************************
  Decode arguments:

    -b <bul_file> - (optional input file) bulk and non-geometrical 
                    parameters.
    -i <par_file> - (mandatory input file) overlayer parameters of all 
                    parameters (if bul_file does not exist).
    -o <res_file> - (output file) IV output.
    -r <pro_name> - (input file) to read parameters and scattering
                    matrices from.
    -w <pro_name> - (output file) top write parameters and scattering
                    matrices to.
   *********************************************************************/

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
#if ERROR_LOG
      ERROR_MSG("\tusage: \t%s -i <par_file> -o <res_file>"
          " [-b <bul_file> -r <pro_name> -w <pro_name> -h -V]\n", LEED_PROG);
#endif
      exit(1);
    }
    else
    {

      /* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
          strcmp(argv[i_arg], "--help") == 0)
      {
        usage_sym(stdout);
        exit(0);
      }  

      /* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 || 
          strcmp(argv[i_arg], "--version") == 0)
      {
        info_sym(); 
        exit(0);
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-b", 2) == 0)
      {
        i_arg++;
        strncpy(bul_file, argv[i_arg], STRSZ);
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        i_arg++;
        strncpy(par_file, argv[i_arg], STRSZ);
      }

      /* Read and open results file */
      if(strncmp(argv[i_arg], "-o", 2) == 0)
      {
        i_arg++;
        strncpy(res_file, argv[i_arg], STRSZ);
        if ((res_stream = fopen(res_file,"w")) == NULL)
        {
#ifdef ERROR_LOG
          fprintf(STDERR,
              "*** error (%s): could not open output file \"%s\"\n",
              LEED_NAME, res_file);
#endif
          exit(1);
        }
      }

      /* Read project name for reading matrices */
      if(strncmp(argv[i_arg], "-r", 2) == 0)
      {
        ctr_flag = FLAG_READ;
        i_arg++;
        strncpy(pro_name, argv[i_arg], STRSZ);
        if ((pro_stream = fopen(pro_name,"r")) == NULL)
        {
#ifdef ERROR_LOG
          fprintf(STDERR,
              "*** error (%s): could not open project file \"%s\" for reading\n",
              LEED_NAME, pro_name);
#endif
          exit(1);
        }
      }

      /* Read project name for writing matrices */
      if(strncmp(argv[i_arg], "-w", 2) == 0)
      {
        ctr_flag = FLAG_WRITE;
        i_arg++;
        strncpy(pro_name, argv[i_arg], STRSZ);
        if ((pro_stream = fopen(pro_name,"w")) == NULL)
        {
#ifdef ERROR_LOG
          fprintf(STDERR,
              "*** error (%s): could not open project file \"%s\" for writing\n",
              LEED_NAME, pro_name);
#endif
          exit(1);
        }
      }
    } /* else */
  }  /* for i_arg */

  /*********************************************************************
  Check arguments:
  - check existence of par_file.
  - if bul_file is not specified, use par_file instead.
  - check existence of res_file.
   *********************************************************************/

  if(strncmp(par_file, "---", 3) == 0)
  {
#ifdef ERROR_LOG
    fprintf(STDERR,
        "*** error (%s): no parameter input file (option -i) specified\n", LEED_NAME);
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
    fprintf(STDWAR,
        "* warning (%s): no output file (option -o) specified\n", LEED_NAME);
    fprintf(STDWAR,"\toutput will be written to file \"%s\"\n", res_file);
#endif

    if ((res_stream = fopen(res_file,"w")) == NULL)
    {
#ifdef ERROR_LOG
      fprintf(STDERR,
          "*** error (%s): could not open output file \"%s\"\n",
          LEED_NAME, res_file);
#endif
      exit(1);
    }
  }

  /*********************************************************************
  Read input parameters
   *********************************************************************/

  switch(ctr_flag)
  {

    case(FLAG_NONE):
     {
      leed_inp_read_bul_sym(&bulk, &phs_shifts, bul_file);
      leed_inp_leed_read_par(&v_par, &eng, bulk, bul_file);
      leed_read_overlayer_sym(&over, &phs_shifts, bulk, par_file);
      n_set = leed_beam_gen_sym(&beams_all, bulk, over, v_par, eng->final);
      break;
     }

    case(FLAG_WRITE):
     {
      leed_inp_read_bul_sym(&bulk, &phs_shifts, bul_file);
      leed_inp_leed_read_par(&v_par, &eng, bulk, bul_file);
      leed_read_overlayer_sym(&over, &phs_shifts, bulk, par_file);
      n_set = leed_beam_gen_sym(&beams_all, bulk, over, v_par, eng->final);

#ifdef CONTROL_IO
      fprintf(STDCTR, "(%s): Write parameters to file \"%s\"\n", LEED_NAME, pro_name);
#endif
      leed_write_par(bulk, phs_shifts, v_par, eng, beams_all, pro_stream);
      fflush(pro_stream);
      break;
     }

    case(FLAG_READ):
     {
#ifdef CONTROL_IO
      fprintf(STDCTR, "(%s): Read parameters from file \"%s\"\n",
          LEED_NAME, pro_name);
#endif
      leed_read_par(&bulk, &phs_shifts, &v_par, &eng, &beams_all, pro_stream);
      leed_read_overlayer_sym(&over, &phs_shifts, bulk, par_file);

      break;
     }

    default:
    {
#ifdef ERROR_LOG
      fprintf(STDERR, "*** error (%s): unsupported ctr_flag\n", LEED_NAME);
#endif
      exit(1);
    }
  }  /* switch */

  /**** leed_inp_show_beam_op(bulk, over, phs_shifts);***/
  leed_output_header(res_stream);
  leed_output_beam_list(&beams_out, beams_all, eng, res_stream);

  /*********************************************************************
 Prepare some often used parameters.
   *********************************************************************/

  mk_cg_coef (2*v_par->l_max);
  mk_ylm_coef(2*v_par->l_max);

#ifdef CONTROL
  fprintf(STDCTR, "(%s): E_ini = %.1f, E_fin = %.1f, E_stp %.1f\n",
      LEED_NAME, eng->initial*HART, eng->final*HART, eng->step*HART);
#endif

  /*********************************************************************
 Energy Loop
   *********************************************************************/

  for(energy = eng->initial;
      energy <= eng->final + E_TOLERANCE;
      energy += eng->step)
  {
    leed_par_update(v_par, phs_shifts, energy);
    n_beams_now = leed_beam_get_selection(&beams_now, beams_all,
        v_par, bulk->dmin);

#ifdef CONTROL
fprintf(STDCTR, "(%s):\n\t => E = %.1f eV (%d beams used) <=\n\n",
    LEED_NAME, v_par->eng_v*HART, n_beams_now);
#endif


/*********************************************************************
  BULK:
  Loop over beam sets
 *********************************************************************/

if( ctr_flag == FLAG_READ )
{

  /****   Read matrix R_bulk. *****/

  R_bulk = matread(R_bulk, pro_stream);
#ifdef CONTROL_IO
  fprintf(STDCTR, "(%s): Read bulk matrix from file \"%s\"\n",
      LEED_NAME, pro_name);
  matshowpar(R_bulk);
#endif
}
else        /* (FLAG_NONE, FLAG_WRITE) */
{
  /************************************************************

   Create matrix R_bulk that will eventually contain the bulk 
   reflection matrix 

   ***************************************************************/

  R_bulk = matalloc(R_bulk, n_beams_now, n_beams_now, NUM_COMPLEX);
  for(offset = 1, i_set = 0; i_set < n_set; i_set ++)
  {
    n_beams_set = leed_beam_set(&beams_set, beams_now, i_set);

    /*********************************************************************
    Loop over periodic bulk layers
     *********************************************************************/

    /*********************************************************
    Calculate scattering matrices for bottom-most bulk layer:
     - single Bravais layer or composite layer
     **********************************************************/
#ifdef CONTROL_FLOW
    fprintf(STDCTR, "(%s periodic): bulk layer %d/%d, set %d/%d\n",
        LEED_NAME, 0, bulk->n_layers - 1, i_set, n_set - 1);
#endif
    if( (bulk->layers + 0)->n_atoms == 1)
    {
      leed_ms_sym( &Tpp, &Rpm,
          v_par, (bulk->layers + 0), beams_set);
      Tmm = matcopy(Tmm, Tpp);
      Rmp = matcopy(Rmp, Rpm);
    }
    else
    {
      leed_ms_compl_sym( &Tpp, &Tmm, &Rpm, &Rmp,
          v_par, (bulk->layers + 0), beams_set);
    }

    /* calculate scattering matrices for bottom-most bulk layer */
    for(i_layer = 1;
        ( (bulk->layers+i_layer)->periodic == 1) &&
            (i_layer < bulk->n_layers);
        i_layer ++)
    {
#ifdef CONTROL_FLOW
      fprintf(STDCTR, "(%s periodic): bulk layer %d/%d, set %d/%d\n",
          LEED_NAME, i_layer, bulk->n_layers - 1, i_set, n_set - 1);
#endif

      /***************************************************************
      Calculate scattering matrices R/T_s for a single bulk layer 
       - single Bravais layer or composite layer
       ****************************************************************/
      if( (bulk->layers + i_layer)->n_atoms == 1)
      {
        leed_ms_sym( &Tpp_s, &Rpm_s,
            v_par, (bulk->layers + i_layer), beams_set);
        Tmm_s = matcopy(Tmm_s, Tpp_s);
        Rmp_s = matcopy(Rmp_s, Rpm_s);
      }
      else
      {
        leed_ms_compl_sym( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
            v_par, (bulk->layers + i_layer), beams_set);
      }

      /****************************************************************
       Add the single layer matrices to the rest by layer doubling 
       - inter layer vector is the vector between layers
         (i_layer - 1) and (i_layer): 
         (bulk->layers + i_layer)->vec_from_last
       *****************************************************************/

      leed_ld_2lay( &Tpp,  &Tmm,  &Rpm,  &Rmp,
          Tpp,   Tmm,   Rpm,   Rmp,
          Tpp_s, Tmm_s, Rpm_s, Rmp_s,
          beams_set, (bulk->layers + i_layer)->vec_from_last);


    } /* for i_layer (bulk) */

    /*******************************************************************
      Layer doubling for all periodic bulk layers until convergence is 
      reached:
       - inter layer vector is (bulk->layers + 0)->vec_from_last
     ********************************************************************/

    Rpm = leed_ld_2n( Rpm, Tpp, Tmm, Rpm, Rmp,
        beams_set, (bulk->layers + 0)->vec_from_last);

    /*******************************************************************
     Calculate scattering matrices for top-most bulk layer if it is
     not periodic
      - single Bravais layer or composite layer
     ********************************************************************/
    if( i_layer == bulk->n_layers - 1 )
    {
#ifdef CONTROL_FLOW
      fprintf(STDCTR, "(%s): bulk layer %d/%d, set %d/%d\n",
          LEED_NAME, i_layer, bulk->n_layers - 1, i_set, n_set - 1);
#endif
      if( (bulk->layers + i_layer)->n_atoms == 1)
      {
        leed_ms_sym( &Tpp_s, &Rpm_s,
            v_par, (bulk->layers + i_layer), beams_set);
        Tmm_s = matcopy(Tmm_s, Tpp_s);
        Rmp_s = matcopy(Rmp_s, Rpm_s);
      }
      else
      {
        leed_ms_compl_sym( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
            v_par, (bulk->layers + i_layer), beams_set);
      }
      /********************************************************************
       Add the single layer matrices of the top-most layer to the rest 
       by layer doubling:
       - inter layer vector is the vector between layers
         (i_layer - 1) and (i_layer): 
         (bulk->layers + i_layer)->vec_from_last
       ********************************************************************/
      Rpm = leed_ld_2lay_rpm(Rpm, Rpm, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
          beams_set, (bulk->layers + i_layer)->vec_from_last);
    }  /* if( i_layer == bulk->n_layers - 1 ) */

    /********************************************************
     Insert reflection matrix for this beam set into R_bulk.
     *********************************************************/
    R_bulk = matins(R_bulk, Rpm, offset, offset);
    offset += n_beams_set;

    /**************************
     Write cpu time to output
     ****************************/
    sprintf(linebuffer,"(%s): bulk layers set %d, E = %.1f",
        LEED_NAME, i_set, energy*HART);
    leed_cpu_time(STDCPU,linebuffer);
  }  /* for i_set */

  if( ctr_flag == FLAG_WRITE)
  {
    matwrite(R_bulk, pro_stream);
    fflush(pro_stream);
#ifdef CONTROL_IO
    fprintf(STDCTR, "(%s): Write bulk matrix to file \"%s\"\n",
        LEED_NAME, pro_name);
#endif
  }

} /* else (FLAG_NONE, FLAG_WRITE) */


#ifdef CONTROL_MAT
fprintf(STDCTR, "(%s): Bulk matrix:\n", LEED_NAME);
matshowabs(R_bulk);
matshow(R_bulk);
#endif

/*****************************************************************
  OVERLAYER
  Loop over all overlayer layers
 *********************************************************************/

for(i_layer = 0; i_layer < over->n_layers; i_layer ++)
{
#ifdef CONTROL_FLOW
  fprintf(STDCTR, "(%s): overlayer %d/%d\n",
      LEED_NAME, i_layer, over->n_layers - 1);
#endif
  /***********************************************************
     Calculate scattering matrices for a single overlayer layer
      - single Bravais layer or composite layer
   ************************************************************/
  if( (over->layers + i_layer)->n_atoms == 1)
  {
    leed_ms_sym( &Tpp_s, &Rpm_s,
        v_par, (over->layers + i_layer), beams_now);
    Tmm_s = matcopy(Tmm_s, Tpp_s);
    Rmp_s = matcopy(Rmp_s, Rpm_s);
  }
  else
  {
    leed_ms_compl_sym( &Tpp_s, &Tmm_s, &Rpm_s, &Rmp_s,
        v_par, (over->layers + i_layer), beams_now);
  }

  /**********************************************************************
     Add the single layer matrices to the rest by layer doubling:
     - if the current layer is the bottom-most (i_layer == 0),
       the inter layer vector is calculated from the vectors between
       top-most bulk layer and origin 
       ( (bulk->layers + n_layers)->vec_to_next )
       and origin and bottom-most overlayer
       (over->layers + 0)->vec_from_last.

     - inter layer vector is the vector between layers
       (i_layer - 1) and (i_layer): (over->layers + i_layer)->vec_from_last
   *************************************************************************/
  if (i_layer == 0)
  {
    for(i_c = 1; i_c <= 3; i_c ++)
    {
      vec[i_c] = (bulk->layers + bulk->n_layers - 1)->vec_to_next[i_c]
                                                                  + (over->layers + 0)->vec_from_last[i_c];
    }

    R_tot = leed_ld_2lay_rpm(R_tot, R_bulk, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
        beams_now, vec);
  }
  else
  {
    R_tot = leed_ld_2lay_rpm(R_tot, R_tot, Tpp_s, Tmm_s, Rpm_s, Rmp_s,
        beams_now, (over->layers + i_layer)->vec_from_last);
  }

  /**************************
     Write cpu time to output
   ***************************/
  sprintf(linebuffer,"(%s): overlayer %d, E = %.1f",
      LEED_NAME, i_layer, energy * HART);
  leed_cpu_time(STDCPU,linebuffer);

}  /* for i_layer (overlayer) */

/*********************************************
   Add propagation towards the potential step.
 **********************************************/
vec[1] = vec[2] = 0.;
vec[3] = 1.25 / BOHR;

/**** No scattering at pot. step ****/
Amp = leed_ld_potstep0(Amp, R_tot, beams_now, v_par->eng_v, vec);
leed_output_intensities(Amp, beams_now, beams_out, v_par, res_stream, 1);

/**Write cpu time to output**/
sprintf(linebuffer,"  %.1f   %d  ",energy * HART,n_beams_now);
leed_cpu_time(STDERR,linebuffer);


  } /* end of energy loop */

  /*****************************************************************************************/


#ifdef CONTROL_IO
  fprintf(STDCTR, "(%s): end of energy loop: close files\n", LEED_NAME);
#endif
  if( (ctr_flag == FLAG_WRITE) || (ctr_flag == FLAG_READ) )
    fclose(pro_stream);

  fclose(res_stream);

#ifdef CONTROL
  fprintf(STDCTR, "\n\n(%s):\tCORRECT TERMINATION", LEED_NAME);
#endif

  /* Write cpu time to output */
  leed_cpu_time(STDCPU,"");

  /* set exit status explicitly */
  exit(0);

} /* end of main */
