/*********************************************************************
 *                        mkiv_read_var.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *    CS/30.07.93 - creation
 *********************************************************************/

/*! \file
 *
 * Contains mkiv_read_var() function for reading variables from a file.
 */

#include <ctype.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"
#include "variables.h"
#include "file_functs.h"

void mkiv_params_free(mkiv_params *params)
{
  if (params != NULL)
  {
    free(params);
  }
}

mkiv_params *mkiv_params_init()
{
  mkiv_params *params = (mkiv_params *) calloc(1, sizeof(mkiv_params));

  if (params == NULL)
  {
    ERROR_MSG("Unable to allocate memory for params\n");
    ERROR_RETURN(NULL);
  }

  /* Preset variables, default values */
  params->cos_min = .6;
  params->cos_max = .9;
  params->verh = 1.5;
  params->acci = .9;
  params->accb = .7;
  params->distance = 120;
  params->update_freq = 10;
  params->step = 2;
  params->trip_max = 599;
  params->scale_min = 3.;
  params->range_min = 3.;
  params->s2n_good = .9;
  params->s2n_bad = .2;
  params->ref_min = 10;
  params->sec_range = 3.;
  params->s2n_ref = 1.2;
  params->range = .045;
  params->scale.xx = .055;
  params->scale.yy = .055;
  params->angle = 0.;
  params->ratio = 0.22;
  params->ref_dev = 2.5;
  params->bas_rat = .7;
  params->bas_rat = .7;

  return (params);
}

/*!
 * Reads some necessary data statements from the file "mkiv.var".
 *
 * If variables don't occur in the input file, defaults are taken instead.
 *
 * \param verb Flag for verbosity level.
 * \param var_path Path to *.var file for reading variables from. The default is
 * "mkiv.var" if \c var_path is an invalid path.
 * \param param_path Path to *.param file for reading variables from.
 * The default is "mkiv.param" if \c param_path is an invalid path.
 * \param cos_min
 * \param cos_max
 * \param verh
 * \param acci
 * \param accb
 * \param distance
 * \param update
 * \param step
 * \param trip_max
 * \param scale_min
 * \param range_min
 * \param s2n_good
 * \param s2n_bad
 * \param ref_min
 * \param sec_range
 * \param s2n_ref
 * \param ref_dev
 * \param bas_rat
 * \param range
 * \param scale
 * \param angle
 * \param ratio
 *  * \return
 */
mkiv_params *mkiv_params_read(const mkiv_args *args)
{
  mkiv_params *params = mkiv_params_init();

  size_t i;
  char buf[STRSZ], var[STRSZ];
  FILE *fvar, *fparam;

  if (params == NULL) ERROR_RETURN(NULL);

  /* Open statement */
  if (args->var_path == NULL)
  {
    ERROR_MSG("No variable filename given\n");
    ERROR_RETURN(NULL);
  }
  if ( (fvar = fopen(args->var_path, "r")) == NULL)
  {
    ERROR_MSG("cannot open variable file '%s'\n", args->var_path);
    ERROR_RETURN(NULL);
  }
  else
  {
    printf("\nreading variable input from \"%s\":\n", args->var_path);
  }
  /* Read data */
  while ( (fgets(buf, STRSZ, fvar) != NULL) && (*buf != '.') )
  {
    if ( ispunct(*buf) )  continue;
    sscanf(buf, "%s", var);

    for (i=0; i < VAR_MAX; i++)
    {
      if ( ! strncasecmp( var, VAR[i], VAR_LEN ) ) break;
    }

    switch (i+1)
    {
      case  1 :  sscanf( buf, "%s %lf", var, &params->cos_min);	break;
      case  2 :  sscanf( buf, "%s %lf", var, &params->cos_max);	break;
      case  3 :  sscanf( buf, "%s %lf", var, &params->verh); break;
      case  4 :  sscanf( buf, "%s %lf", var, &params->acci); break;
      case  5 :  sscanf( buf, "%s %lf", var, &params->accb); break;
      case  6 :  sscanf( buf, "%s %d", var, &params->distance); break;
      case  7 :  sscanf( buf, "%s %d", var, &params->update_freq); break;
      case  8 :  sscanf( buf, "%s %d", var, &params->step); break;
      case  9 :  sscanf( buf, "%s %d", var, &params->trip_max); break;
      case 10 :  sscanf( buf, "%s %lf", var, &params->scale_min); break;
      case 11 :  sscanf( buf, "%s %lf", var, &params->range_min); break;
      case 12 :  sscanf( buf, "%s %lf", var, &params->s2n_good); break;
      case 13 :  sscanf( buf, "%s %lf", var, &params->s2n_bad); break;
      case 14 :  sscanf( buf, "%s %d", var, &params->ref_min); break;
      case 15 :  sscanf( buf, "%s %lf", var, &params->sec_range); break;
      case 16 :  sscanf( buf, "%s %lf", var, &params->s2n_ref); break;
      case 17 :  sscanf( buf, "%s %lf", var, &params->ref_dev); break;
      case 18 :  sscanf( buf, "%s %lf", var, &params->bas_rat); break;
      case 19 :  sscanf( buf, "%s %lf", var, &params->range); break;
      case 20 :  sscanf( buf, "%s %lf", var, &(params->scale.xx)); break;
      case 21 :  sscanf( buf, "%s %lf", var, &(params->scale.yy)); break;
      case 22 :  sscanf( buf, "%s %lf", var, &params->angle); break;
      case 23 :  sscanf( buf, "%s %lf", var, &params->ratio); break;

      default :
      {
        ERROR_MSG("invalid argument '%s'\n", var);
      }
    }
  }
  fclose(fvar);

  /* read parameter file */
  if (args->param_path == NULL)
  {
    ERROR_MSG("No parameter filename given\n");
    ERROR_RETURN(NULL);
  }
  if ( (fparam = fopen(args->param_path, "a")) == NULL)
  {
    ERROR_MSG("unable to open parameter file '%s'", args->param_path);
    ERROR_RETURN(-1);
  }
  else
  {
    if (args->verbose & VERBOSE) printf("writing input-values to '%s' \n\n", args->param_path);
    fprintf(fparam, "cos_min = %.2f \n", params->cos_min);
    fprintf(fparam, "cos_max = %.2f \n", params->cos_max);
    fprintf(fparam, "verh = %.2f \n", params->verh);
    fprintf(fparam, "acci = %.2f \n", params->acci);
    fprintf(fparam, "accb = %.2f \n", params->accb);
    fprintf(fparam, "distance = %d \n", params->distance);
    fprintf(fparam, "update = %d \n", params->update_freq);
    fprintf(fparam, "step = %d \n", params->step);
    fprintf(fparam, "trip_max = %d \n", params->trip_max);
    fprintf(fparam, "scale_min = %.1f \n", params->scale_min);
    fprintf(fparam, "range_min = %.1f \n", params->range_min);
    fprintf(fparam, "s2n_good = %.3f \n", params->s2n_good);
    fprintf(fparam, "s2n_bad = %.3f \n", params->s2n_bad);
    fprintf(fparam, "ref_min = %d \n", params->ref_min);
    fprintf(fparam, "sec_range = %.1f \n", params->sec_range);
    fprintf(fparam, "s2n_ref = %.2f \n", params->s2n_ref);
    fprintf(fparam, "ref_dev = %.1f \n", params->ref_dev);
    fprintf(fparam, "bas_rat = %.2f \n", params->bas_rat);
    fprintf(fparam, "range = %.3f \n", params->range);
    fprintf(fparam, "scale_a = %.3f \n", params->scale.xx);
    fprintf(fparam, "scale_b = %.3f \n", params->scale.yy);
    fprintf(fparam, "angle = %.1f \n", params->angle);
    fprintf(fparam, "ratio = %.2f \n", params->ratio);
    fclose(fparam);
    if (QQ(args->verbose)) {
      char *param_content = file_content(args->param_path);
      printf("%s:\n%s\n", args->param_path, param_content);
    }
  }

  return(0);
}
