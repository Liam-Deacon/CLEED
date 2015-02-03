/*********************************************************************
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
 *31.03.03
  file contains function:

  real sr_evalrf(real *par)

 Calculate IV curves and evaluate R factor

 Changes:
GH/15.08.95 - Creation (copy from srcalcrf.c)
GH/11.09.95 - Variable shift (with fixed range and step)
GH/20.11.95 - correct command line for R factor calculation.
GH/01.12.95 - Default program names: cleed, crfac (no full path)
GH/02.09.97 - used environment variables "CSEARCH_LEED" and "CSEARCH_RFAC"
              for program names.
GH/27.11.01 - make R factor range independent of previous shift (needed for SA).
SRP/27.09.02 - inserted a 'flag' for testing purposes, writes a brief message 
               to the control file.
SRP/31.03.03 - Inserted a routine to write the angles to the log file for the angle search
GH/30.12.04 - calculate original and mirrored geometry at the same time and average IV curves.


***********************************************************************/
#include <stdio.h>
#include <strings.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "csearch.h"
#include "copy_file.h"

#define SYS_ERROR_TO_LOG(x)   log_stream = fopen(log_file, "a"); \
     fprintf(log_stream,"*** system call \"%s\" failed ***", x); \
     fclose(log_stream); exit(1)

#define COPY_ERROR_TO_LOG(x,y) log_stream = fopen(log_file, "a"); \
     fprintf(log_stream,"*** copying \"%s\" to \"%s\" failed ***", x, y); \
     fclose(log_stream); exit(1)
     
extern search_atom *sr_atoms;
extern search *sr_search;
extern char *sr_project;

real sr_evalrfac_mir(real *par)

/***********************************************************************

 - check geometry
 - set up inputfile for IV program ( ... )
 - calculate IV curves (program "cleed")
 - calculate R-factor (program "crfac")

***********************************************************************/
{
  static real rfac_min = 100.;
  static real rfac_max = 0.;
  static real shift = 0.;
  static int n_eval  = 0;
  static int n_calc  = 0;

  int iaux;
  size_t i_par;
  real faux;
  real rgeo, rfac;

  struct tm *l_time;
  time_t t_time;

  char fmt_buffer[STRSZ];
  char line_buffer[STRSZ];
  char log_file[STRSZ];
  char par_file[STRSZ];

  FILE *io_stream, *log_stream;

  char *old_path;
  char *new_path;

  /* Set initial values */
  n_eval ++;
  sprintf(log_file, "%s.log", sr_project);
  sprintf(par_file, "%s.par", sr_project);

  /* Check whether environment variables CSEARCH_LEED and CSEARCH_RFAC exist */
  if( (getenv("CSEARCH_LEED") == NULL) || (getenv("CSEARCH_RFAC") == NULL) )
  {
    ERROR_MSG("CSEARCH_LEED or CSEARCH_RFAC environment variables not defined\n");
    exit(SR_ENVIRONMENT_VARIABLE_ERROR);
  }

  /* Geometry assessment */
#ifdef SHORTCUT
  fprintf(STDCTR, "(sr_evalrf %d) SHORTCUT:", n_eval);
#endif

  rgeo = sr_ckgeo( par );

#ifdef CONTROL
# ifdef SHORTCUT
    fprintf(STDCTR," rgeo = %.4f", rgeo);
# else
    fprintf(STDCTR," rgeo = %.4f\n", rgeo);
# endif
#endif

  /* Return (rfac_max + rgeo), if the test geometry is seriously
   * outside the limits.
   * Calculate IV curves otherwise.
   */
  if( (rgeo > 1.) && (n_calc > 1) )
  {
#ifdef CONTROL
    fprintf(STDCTR, " return rfac_max: %.4f rtot = %.4f\n",
             rfac_max, rgeo + rfac_max);
#endif
    log_stream = fopen(log_file, "a");

    fprintf(log_stream, "#%3d par:", n_eval);
    for(i_par=1; i_par <= sr_search->n_par; i_par++)
      fprintf(log_stream, " %.3f", par[i_par]);
    fprintf(log_stream, " **rfmax: %.4f** rg:%.4f rt:%.4f ",
            rfac_max, rgeo, rfac_max + rgeo);

    t_time = time(NULL);
    l_time = localtime(&t_time);
    fprintf(log_stream, " dt: %s", asctime(l_time) );

    fclose(log_stream);
    return(rfac_max + rgeo);
  }

  /* Calculate IV curves */
  n_calc ++;
  sr_mkinp_mir(par_file, par, n_calc);

#ifdef SHORTCUT
  rfac = 0.;
  for (i_par = 1; i_par <= sr_search->n_par; i_par ++)
    rfac += 1 - cos(PI*(par[i_par] -2.3)) + R_fabs(par[i_par] - 2.3);
  rfac /= sr_search->n_par;

# ifdef CONTROL
  fprintf(STDCTR, " rfac = %.4f rtot = %.4f\n", rfac, rgeo + rfac);
# endif

  rfac_min = MIN(rfac, rfac_min);
  rfac_max = MAX(rfac, rfac_max);

#else

  /* calculate IV curves for the original geometry */
  sprintf(line_buffer,
         "\"%s\" -b \"%s.bul_org\" -i \"%s\" -o \"%s.res_org\" > \"%s.out\"",
         getenv("CSEARCH_LEED"),      /* LEED program name */
         sr_project,                  /* project name for bulk file */
         par_file,                    /* parameter file for overlayer */
         sr_project,                  /* project name for results file */
         sr_project);                 /* project name for output file */

  CONTROL_MSG(CONTROL, "[%d] - calculate IV curves (original):\n %s\n",
                      n_eval, line_buffer);

  if (system (line_buffer)) {SYS_ERROR_TO_LOG(line_buffer);}

  /* calculate IV curves for the mirrored geometry */
  sprintf(line_buffer,
         "\"%s\" -b \"%s.bul_mir\" -i \"%s\" -o \"%s.res_mxx\" > \"%s.out\"",
         getenv("CSEARCH_LEED"),      /* LEED program name */
         sr_project,                  /* project name for bulk file */
         par_file,                    /* parameter file for overlayer */
         sr_project,                  /* project name for results file */
         sr_project);                 /* project name for output file */

       
  CONTROL_MSG(CONTROL, "[%d] - calculate IV curves (mirror):\n %s\n",
              n_eval, line_buffer);

  if (system (line_buffer)) {SYS_ERROR_TO_LOG(line_buffer);}

  /* change indices in IV curves for the mirrored geometry */
  sprintf(line_buffer,
         "\"%s/bin/c_miry\" -i \"%s.res_mxx\" -o \"%s.res_mir\" > \"%s.out\"",
         getenv("CLEED_HOME"),
         sr_project,                  /* project name for res file */
         sr_project,                  /* project name for res file */
         sr_project);                 /* project name for out file */
 
  if (system (line_buffer)) {SYS_ERROR_TO_LOG(line_buffer);}

  /* average IV curves for mirrored and original geometry */
  sprintf(line_buffer,
         "\"%s/bin/c_mixtiv\" -t1 \"%s.res_org\" -t2 \"%s.res_mir\" -o \"%s.res\" > \"%s.out\"",
         getenv("CLEED_HOME"),
         sr_project,                  /* project name for res_org file */
         sr_project,                  /* project name for res_mir file */
         sr_project,                  /* project name for res file */
         sr_project);                 /* project name for out file */

  if (system (line_buffer)) {SYS_ERROR_TO_LOG(line_buffer);}

  /* Calculate R factor:
   * changed for Sim. Ann.: range is independent of previous shift.
   */
  sprintf(line_buffer,
         "\"%s\" -t \"%s.res\" -c \"%s.ctr\" -r \"%s\" -s %.2f,%.2f,%.2f> \"%s.dum\"", 
         getenv("CSEARCH_RFAC"),      /* R factor program name */
         sr_project,                  /* project name for the. file */
         sr_project,                  /* project name for control file */
         RFAC_TYP,                    /* type of R factor */
         - RFAC_SHIFT_RANGE,          /* initial shift */
         + RFAC_SHIFT_RANGE,          /* final shift */
         RFAC_SHIFT_STEP,             /* step of shift */
         sr_project);                 /* project name for output file */

  CONTROL_MSG(CONTROL, "[%d] - calculate R factor:\n %s\n", n_eval, line_buffer);

  if (system (line_buffer)) {SYS_ERROR_TO_LOG(line_buffer);}

  /* Read R factor value from output file */
  sprintf(line_buffer, "%s.dum", sr_project);
  io_stream = fopen(line_buffer, "r");

  sprintf(fmt_buffer, "%%%sf %%%sf %%%sf",
          CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
  while( fgets(line_buffer, STRSZ, io_stream) != NULL)
  {
    if( (iaux = sscanf(line_buffer, fmt_buffer, &rfac, &faux, &shift) ) == 3)
    {
      break;
    }
  }

  /* Stop with error message if reading error */
  if( iaux != 3)
  {
    log_stream = fopen(log_file, "a");
    fprintf(log_stream, "***error (%s): failed to read output from '%s'\n",
            __func__, getenv("CSEARCH_RFAC"));
    fclose(log_stream);
    exit(1);
  }

  fclose (io_stream);

# if CONTROL
  fprintf(STDCTR," rfac = %.4f\n", rfac);
 #endif

  /* If this is the minimum R factor copy *.res file to *.rmin */
  if(rfac < rfac_min)
  {
 
    /* remove dependence on 'cp' system call */
    old_path = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
    new_path = (char *) malloc(sizeof(char) * (strlen(sr_project)+5));
   
    strcpy(old_path, sr_project);
    strcpy(new_path, sr_project);
    strcat(old_path, ".res");
    strcat(new_path, ".rmin");
    if (copy_file(old_path, new_path)) {COPY_ERROR_TO_LOG(old_path, new_path);}
   
    strcpy(old_path, sr_project);
    strcpy(new_path, sr_project);
    strcat(old_path, ".par");
    strcat(new_path, ".pmin");
    if (copy_file(old_path, new_path)) {COPY_ERROR_TO_LOG(old_path, new_path);}

    rfac_min = rfac;
  }

  rfac_max = MAX(rfac, rfac_max);
#endif /* SHORTCUT */

  /* Write parameters, R factor and time to *.log file */
  log_stream = fopen(log_file, "a");

  fprintf(log_stream, "#%3d par:", n_eval);
  for(i_par=1; i_par <= sr_search->n_par_geo; i_par++)
    fprintf(log_stream, " %.3f", par[i_par]);
 

  /* Inserted a routine to print the angles in the log-file for
   * the angle search: */
  if(sr_search->sr_angle)
  { 
    fprintf(log_stream, " theta:%.2f",(par[sr_search->i_par_theta]*FAC_THETA));
    fprintf(log_stream, " phi:%.2f",  (par[sr_search->i_par_phi  ]*FAC_PHI)  );
  }


  fprintf(log_stream, " rf:%.4f sh: %.1f rg:%.4f rt:%.4f ",
          rfac, shift, rgeo, rfac + rgeo);

  t_time = time(NULL);
  l_time = localtime(&t_time);
  fprintf(log_stream, " dt: %s", asctime(l_time) );

  fclose(log_stream);

  /* Clean up and return R factor */
  free(old_path);
  free(new_path);

  return (rfac + rgeo);
}





