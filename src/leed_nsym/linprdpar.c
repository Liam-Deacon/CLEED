/*********************************************************************
 *                      LINPRDPAR.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/26.01.95 - Creation: copy from leed_inp_read_bul
 *   GH/07.03.95 - Add angles of incidence.
 *   GH/07.07.95 - Read output file.
 *   GH/28.07.95 - complete redesign.
 *********************************************************************/

/*! \file
 *
 * Reads most of the parameters necessary to control the program.
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

#include "leed.h"
#include "leed_def.h"


#ifndef WAVE_TOLERANCE         /* should be defined in "leed_def.h" */
#define WAVE_TOLERANCE 1.e-4
#endif

#ifndef R_FOR_LMAX             /* should be defined in "leed_def.h" */
#define R_FOR_LMAX 2.5
#endif

/*!
 * Reads most of the parameters necessary to control the program.
 *
 * Letters 'e' - 'l' ans 've' are reserved as identifiers for parameter input
 * through function leed_inp_leed_read_par() .
 *
 * This function currently reads from input file:
 *
 * ei: \p eng_par->initial = initial energy
 * ef: \p eng_par->final = final energy
 * es: \p eng_par->step = energy step
 *
 * it: \p var_par->theta = polar angle of incidence (default: 0.).
 * ip: \p var_par->phi   = azimuthal angle of incidence (default: 0.).
 * ep: \p var_par->epsilon = convergence criterion for wave functions (default:
 *                      WAVE_TOLERANCE, typically 1.e-4)
 * lm: \p var_par->l_max = max. l quantum number to be used in the program. If
 *                      no value is provided, the function calculates a value
 *                      from the largest energy according to:
 *                      l_max = R * k_max
 *
 * ve: \p var_par->vi_exp = exponent for the imaginary part of optical potential.
 *
 * The other values of the structure #leed_var are preset as follows:
 *
 * @leed_var::eng_r   ->  0.
 * @leed_var::eng_i   ->  0.
 * @leed_var::eng_v   ->  0.
 * @leed_var::vr      ->  bulk_par->vr
 * @leed_var::vi_pre  ->  bulk_par->vi
 * @leed_var::vi_exp  ->  (set in leed_inp_leed_read_par)
 * @leed_var::theta   ->  (set in leed_inp_leed_read_par)
 * @leed_var::phi     ->  (set in leed_inp_leed_read_par)
 * @leed_var::k_in ->  0., 0., 0., 0.
 * @leed_var::epsilon ->  (set in leed_inp_leed_read_par)
 * @leed_var::l_max   ->  (set in leed_inp_leed_read_par)
 * @leed_var::p_tl    ->  NULL
 *
 * \param[out] p_var_par Double pointer to matrix of #leed_var
 * \param[out] p_eng_par Double pointer to matrix of #leed_var
 * \param[out] bulk_par Pointer to #leed_crystal structure containing bulk
 * crystal parameters (must contain valid values for vr and vi).
 * Function leed_inp_read_bul() must be called before leed_inp_leed_read_par() .
 * \param[in] in_file Name of the input file.
 * \return Error code indicating function success.
 * \retval 0 if successful.
 * \retval #leed_error if failed and #EXIT_ON_ERROR not defined.
 * \see leed_inp_leed_read_par()
 */
int leed_inp_leed_read_par(leed_var **p_var_par, leed_energy **p_eng_par,
                           leed_crystal *bulk_par, const char *in_file)
{
  leed_var *var_par;
  leed_energy *eng_par;

  int i_str;                      /* counter variables */
  int i_c;                        /* dummy variables */

  real faux;

  FILE *inp_stream;
  char linebuffer[STRSZ];         /* input buffer */

  /* If *p_var_par or *p_eng_par are NULL: allocate memory. */
  if (*p_var_par == NULL)
  {
    #ifdef CONTROL_X
    fprintf(STDCTR, "(leed_inp_leed_read_par): allocate var_par\n");
    #endif

    var_par = *p_var_par = (leed_var *)malloc( sizeof(leed_var) );
  }
  else var_par = *p_var_par;

  if (*p_eng_par == NULL)
  {
    eng_par = *p_eng_par = (leed_energy *)malloc( sizeof(leed_energy) );
  }
  else eng_par = *p_eng_par;

  /* Preset elements of var_par */
  /* not to be read in this function: */
  var_par->eng_r = 0.;
  var_par->eng_i = 0.;
  var_par->eng_v = 0.;

  var_par->vi_pre = bulk_par->vi;
  var_par->vi_exp = 0.;
  var_par->vr     = bulk_par->vr;

  for( i_c = 0; i_c <=3; i_c ++) var_par->k_in[i_c] = 0.;

  var_par->p_tl = NULL;

  /* to be read in this function: */
  var_par->theta = var_par->phi = 0.;
  var_par->epsilon = WAVE_TOLERANCE;
  var_par->l_max = 0;

  eng_par->initial = eng_par->final = 0.;
  eng_par->step = 4./HART;
  
  /* START INPUT
   * Open and Read input file
   */
  if( (inp_stream = fopen(in_file, "r")) == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (leed_inp_leed_read_par): "
            "could not open file \"%s\"\n", in_file);
    #endif

    #ifdef EXIT_ON_ERROR
    exit(LEED_FILE_IO_ERROR);
    #else
    return(LEED_FILE_IO_ERROR);
    #endif
  }

  #ifdef CONTROL
  fprintf(STDCTR, "(leed_inp_leed_read_par): Reading file \"%s\"\n", in_file);
  #endif

  while ( fgets(linebuffer, STRSZ, inp_stream) != NULL)
  {
    #ifdef CONTROL_X
    fprintf(STDCTR, "%s", linebuffer);
    #endif

    /* find first non blank character */
    for( i_str = 0;  *(linebuffer+i_str) == ' '; i_str ++);
    switch( *(linebuffer+i_str) )
    {
      case ('e'): case ('E'):
      /* ei, ef, es: input of energy parameters
       * (initial, final energy and energy step)
       * ep: epsilon
       */
      {
        switch( *(linebuffer+i_str+1) )
        {
          case('f'): /* final energy */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &faux);
            #else
            sscanf(linebuffer+i_str+3, "%f", &faux);
            #endif

            eng_par->final = faux/HART;
            break;
          }

          case('i'): /* initial energy */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &faux);
            #else
            sscanf(linebuffer+i_str+3, "%f", &faux);
            #endif

            eng_par->initial = faux/HART;
            break;
          }

          case('p'): /* epsilon */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &(var_par->epsilon) );
            #else
            sscanf(linebuffer+i_str+3, "%f", &(var_par->epsilon) );
            #endif

            break;
          }

          case('s'): /* energy step */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &faux);
            #else
            sscanf(linebuffer+i_str+3, "%f", &faux);
            #endif

            eng_par->step = faux/HART;
            break;
          }
        }

        break;
      } /* case 'e' */

      case ('i'): case ('I'):
      /* it, ip: input of angles of incidence (theta, phi) */
      {
        switch( *(linebuffer+i_str+1) )
        {
          case('t'): /* theta */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &faux);
            #else
            sscanf(linebuffer+i_str+3, "%f", &faux);
            #endif

            var_par->theta = DEG_TO_RAD * faux;
            break;
          }

          case('p'): /* phi */
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &faux);
            #else
            sscanf(linebuffer+i_str+3 ,"%f", &faux);
            #endif

            var_par->phi = DEG_TO_RAD * faux;
            break;
          }

        } /* switch */

        break;
      } /* case 'i' */

      case ('l'): case ('L'):
      /* lm: input of l_max */
      {
        switch( *(linebuffer+i_str+1) )
        {
          case('m'):
          {
            sscanf(linebuffer+i_str+3, "%d", &(var_par->l_max) );
            break;
          }
        }
        break;
      } /* case 'l' */

      case ('v'): case ('V'):
      /* input of exponent for imaginary part of optical potential */
      {
        switch( *(linebuffer+i_str+1) )
        {
          case('e'):
          {
            #ifdef REAL_IS_DOUBLE
            sscanf(linebuffer+i_str+3, "%lf", &(var_par->vi_exp));
            #else
            sscanf(linebuffer+i_str+3, "%f", &(var_par->vi_exp));
            #endif
            break;
          }
           
        }
        break;
      } /* case 'l' */


      /* ignored lines: */
      case ('#'): case ('\n'): case('\r'):
      /* comments not to be stored and new line characters */
      case ('a'): case ('A'):
      case ('b'): case ('B'):
      case ('c'): case ('C'):
      case ('m'): case ('M'):
      case ('p'): case ('P'):
      case ('s'): case ('S'):
      /* identifiers for leed_inp_read_bul() and leed_read_overlayer() */
      { break; }

      default:
      /* default: print warning for unrecognized key words */
      {
        #ifdef WARNING
        fprintf(STDWAR, "* warning (leed_inp_leed_read_par): "
            "could not interpret line: \n\t%s\t(in file \"%s\")\n",
            linebuffer, in_file);
        #endif
        break;
      }
    } /* switch linebuffer */
  } /* while .... */

  fclose(inp_stream);

  /* END OF INPUT
   * Start controlling and processing input data.
   */

  /* ENERGIES:
   * - error message if eng_par->ini <= 0.
   * - warning if eng_par->fin <= eng_par->ini => only 1 energy step.
   * - warning if eng_par->stp <= 0. => only 1 energy step.
   */
  if (eng_par->initial <= 0.)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (leed_inp_leed_read_par): "
        "no initial energy available (Eini = %.1f)\n", eng_par->initial * HART);
    #endif

    #ifdef EXIT_ON_ERROR
    exit(LEED_INVALID_ENERGY);
    #else
    return(LEED_INVALID_ENERGY);
    #endif
  }

  if (eng_par->final <= eng_par->initial)
  {
    #ifdef WARNING
    fprintf(STDWAR, "* warning (leed_inp_leed_read_par): "
        "final energy (%.1f) <= initial energy (%.1f)\n",
        eng_par->final * HART, eng_par->initial * HART);
    fprintf(STDWAR, "*         only one energy step will be performed.\n");
    #endif
    eng_par->final = eng_par->initial;
  }

  if (eng_par->step <= 0.)
  {
    #ifdef WARNING
    fprintf(STDWAR, "* warning (leed_inp_leed_read_par): "
        "energy <= 0. (%.1f)\n", eng_par->step * HART);
    fprintf(STDWAR, "*         only one energy step will be performed.\n");
    #endif
    eng_par->step = eng_par->final - eng_par->initial;
  }

  #ifdef CONTROL
  fprintf(STDCTR, "******************************"
      "(leed_inp_leed_read_par)*****************************\n");
  fprintf(STDCTR, "energy loop:\n");
  fprintf(STDCTR, "\tstart:\t%.1f eV\n",eng_par->initial * HART);
  fprintf(STDCTR, "\tend:\t%.1f eV\n",eng_par->final * HART);
  fprintf(STDCTR, "\tstep:\t%.1f eV\n",eng_par->step * HART);
  #endif

  /* LMAX
   * - warning if var_par->l_max = 0 => calculate l_max = k_max * R
   */
  if (var_par->l_max <= 0)
  {
    #ifdef WARNING
    fprintf(STDWAR, "* warning (leed_inp_leed_read_par): "
        "l_max = %d <= 0\n", var_par->l_max);
    #endif

    faux = R_sqrt(2. * eng_par->final) * R_FOR_LMAX;
    var_par->l_max = (int)R_nint(faux);

    #ifdef WARNING
    fprintf(STDWAR, "*         calculate value %d from Efin = %.1f eV and "
        "assumed Rmax = %.1f A.\n", var_par->l_max,
        eng_par->final * HART, R_FOR_LMAX * BOHR);
    #endif
  }

  /* Write eng_par and var_par back to their pointers and return. */
  #ifdef CONTROL
  fprintf(STDCTR, "\nparameter structure:\n");
  fprintf(STDCTR, "\tvr:\t%.2f eV,\tvi:\t%.2f eV (pref), (expt: %.2f)\n",
          var_par->vr*HART, var_par->vi_pre*HART, var_par->vi_exp);
  fprintf(STDCTR, "\ttheta:\t%.1f deg,\tphi:\t%.1f deg\n",
          var_par->theta*RAD_TO_DEG, var_par->phi*RAD_TO_DEG);
  fprintf(STDCTR, "\teps:\t%.1e,\tl_max:\t%d\n",
          var_par->epsilon, var_par->l_max);
  fprintf(STDCTR, "******************************"
      "(leed_inp_leed_read_par)*****************************\n");
  #endif

  *p_eng_par = eng_par;
  *p_var_par = var_par;

  return(0);
}  /* end of function (leed_inp_leed_read_par) */
