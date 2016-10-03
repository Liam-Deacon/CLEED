/*********************************************************************
 *                        mkiv_read_inp.C
 *
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Limiddlesed under GNU General Public Limiddlese 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @limiddlese GPL-3.0+ <http://spdx.org/limiddleses/GPL-3.0+>
 *
 * Changes:
 *    CS/07.09.93 - creation
 *********************************************************************/

/*! \file
 *
 * Implements mkiv_read_inp() , getdomain() and getindices() functions.
 */

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"
#include "inputs.h"

void mkiv_input_free(mkiv_input *inp)
{
  if (inp != NULL)
  {
    free(inp);
  }
}

mkiv_input *mkiv_input_init()
{
  mkiv_input *inp = (mkiv_input *)calloc(1, sizeof(mkiv_input));

  if (inp == NULL)
  {
    ERROR_MSG("Unable to allocate memory for input parameters\n");
    ERROR_RETURN(NULL);
  }

  /* Preset variables */
  inp->n_start  = 40;
  inp->n_stop = 400;
  inp->e_step = 1;
  inp->center.xx  = 250;
  inp->center.yy = 250;
  inp->r_outer  = 230;
  inp->r_inner  = 0;
  inp->bkgnd = 2;
  inp->smooth = -1;
  inp->kpl_10 = 2.6771;
  inp->n_dom = 0;

  return (inp);
}

/*!
 * Reads some necessary data statements from the file mkiv.inp
 * if variables don't occur in the input file, defaults are taken instead
 *
 * \param[in,out] args Pointer to arguments needed for reading input files.
 *  * \return Pointer to #mkiv_input structure on success.
 * \retval Pointer to #mkiv_input struct if successful.
 * \retval \c NULL if unsuccessful.
 */
mkiv_input *mkiv_input_read(mkiv_args *args)
{
  mkiv_input *inp = mkiv_input_init();

  if (inp == NULL)
  {
    ERROR_RETURN(NULL);
  }

  size_t i;
  size_t nn_dom = 0;
  double fac, interim;
  char buf[STRSZ], var[STRSZ];
  FILE *fp = NULL;

  /* Open statement */
  if (args->inp_path == NULL) strcpy(args->inp_path, "mkiv.inp");
  if ( (fp = fopen(args->inp_path, "r")) == NULL)
  {
    ERROR_MSG("failed to open '%s' (%s)\n", 
              args->inp_path, strerror(errno));
    ERROR_RETURN(-1);
  }
  else printf("\nreading input from '%s':\n", args->inp_path);
     
  /* Read data */
  while ( fgets(buf, STRSZ, fp) != NULL && *buf!='.' )
  {
    if ( ispunct(*buf) ) continue;
    sscanf(buf, "%s", var);

    for (i=0; i < INP_MAX; i++)
    {
      if ( ! strncasecmp( var, INP[i], INP_LEN ) ) break;
    }
    switch (i+1)
    {
      case  1 :  sscanf( buf, "%s %s", var, inp->ref_name); break;
      case  2 :  sscanf( buf, "%s %s", var, args->mask_path); break;
      case  3 :  sscanf( buf, "%s %u", var, &inp->n_start); break;
      case  4 :  sscanf( buf, "%s %u", var, &inp->n_stop); break;
      case  5 :  sscanf( buf, "%s %lf", var, &inp->e_step); break;
      case  6 :  sscanf( buf, "%s %d", var, &(inp->center.xx)); break;
      case  7 :  sscanf( buf, "%s %d", var, &(inp->center.yy)); break;
      case  8 :  sscanf( buf, "%s %lf", var, &inp->r_outer); break;
      case  9 :  sscanf( buf, "%s %lf", var, &inp->r_inner); break;
      case 10 :  sscanf( buf, "%s %d", var, &inp->bkgnd); break;
      case 11 :  sscanf( buf, "%s %d", var, &inp->smooth); break;
      case 12 :  sscanf( buf, "%s %lf", var, &inp->kpl_10); break;
      case 13 :  sscanf( buf, "%s %u", var, &inp->n_dom); break;
      case 14 :  mkiv_read_domain( buf, &(inp->superlat[nn_dom++]));	break;
      case 15 :  mkiv_read_indices( fp, buf,
                                    &inp->n_desi, inp->desi, &inp->n_ref,
                                    inp->ref, &inp->n_excl, inp->excl); break;
      default :
      {
        buf[strlen(buf)-1] = '\0';
        ERROR_MSG("wrong arguments in line '%s'\n", buf);
      }
    }
  }
  fclose(fp);

  if (args->param_path == NULL) strcpy(args->param_path, "mkiv.param");
  if ( (fp = fopen(args->param_path, "w")) == NULL)
  {
    ERROR_MSG("unable to write to '%s'\n", args->param_path);
  }
  else
  {
    if (args->verbose & VERBOSE) printf("writing parameter input-values to '%s'\n\n", args->param_path);
    fprintf(fp, "ref_name = %s \n", inp->ref_name);
    fprintf(fp, "mask_name = %s \n", args->mask_path);
    fprintf(fp, "n_start = %d \n", inp->n_start);
    fprintf(fp, "n_stop = %d \n", inp->n_stop);
    fprintf(fp, "n_step = %f \n", inp->e_step);
    fprintf(fp, "center = (%d,%d) \n", inp->center.xx, inp->center.yy);
    fprintf(fp, "r_outer = %.0f \n", inp->r_outer);
    fprintf(fp, "r_inner = %.0f \n", inp->r_inner);
    fprintf(fp, "bg = %s \n", ((*var&&(inp->bkgnd==2))?"YES":"NO"));
    fprintf(fp, "smoothcurrent = %s \n",
                ((*var&&(inp->smooth==-1))?"YES":"NO"));
    fprintf(fp, "kp_len_10 = %f \n", inp->kpl_10);
    fprintf(fp, "n_dom = %d \n", inp->n_dom);
    for (i=0; i < inp->n_dom; i++)
    {
      fprintf(fp, "%lf %lf %lf %lf \n",
              inp->superlat[i].lind11, inp->superlat[i].lind12,
              inp->superlat[i].lind21, inp->superlat[i].lind22);
    }
    fprintf(fp, "n_desi = %d \n", inp->n_desi);

    for (i=0; i < inp->n_desi && i < 9; i++)
    {
      fprintf(fp, "%lf %lf \n", inp->desi[i].lind1, inp->desi[i].lind2);
    }
    if (i < inp->n_desi) fprintf(fp, "\t.\n\t.\n\t.\n\n");
    fclose(fp);
  }

  printf("Total of %d spots.\n", inp->n_desi);
  if (inp->n_ref > 2)  printf("%d spots used for recalibration;\n", inp->n_ref);
  else            printf("Recalibration with most intense spots;\n");
  printf(" No maximum search for %d spots.\n", inp->n_excl);

  /* inversion of superlattice matrices */
  for (i=0; i < inp->n_dom; i++)
  {
    fac = inp->superlat[i].lind11 * inp->superlat[i].lind22 -
          inp->superlat[i].lind12 * inp->superlat[i].lind21;
    interim = inp->superlat[i].lind11;
    inp->superlat[i].lind11 = inp->superlat[i].lind22 / fac;
    inp->superlat[i].lind22 = interim / fac;
    interim = inp->superlat[i].lind12;
    inp->superlat[i].lind12 = -inp->superlat[i].lind21 / fac;
    inp->superlat[i].lind21 = -interim / fac;
  }
    
  return(inp);
}


/*!
 * Reads the superstructure domain information from a string.
 *
 * \param[in] buffer Pointer to buffer string to read.
 * \param[out] superlat Pointer to superstructure domain.
 *  * \return Value returned by sscanf() if an error occurs.
 * \retval 0 Otherwise
 */
int mkiv_read_domain(const char *buffer, mkiv_domain *superlat)
{
  char var[STRSZ];
  int aux;

  aux = sscanf( buffer, "%s %lf %lf %lf %lf", var,
                        &superlat->lind11,
                        &superlat->lind12,
                        &superlat->lind21,
                        &superlat->lind22 );

  if (aux == 5) return(0);
  else return(aux+1);

  return(0);
} /* end of function getdomain */

/*!
 * Reads the list of indices.
 *
 * \param fp File pointer to input file.
 * \param buffer Line buffer string for input.
 * \param[out] n_desi Number of desired reflexes in \p desi .
 * \param[out] desi Array of desired reflexes.
 * \param[out] n_ref Number of reference reflexes in \p ref .
 * \param[out] ref Array of reference reflexes.
 * \param[out] n_excl Number of excluded reflexes in \p excl .
 * \param[out] excl Array of excluded reflexes.
 *  * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if unsuccessful.
 */
int mkiv_read_indices(FILE *fp, char *buffer, size_t *n_desi, mkiv_index *desi,
                size_t *n_ref, mkiv_index *ref, size_t *n_excl, mkiv_index *excl)
{
  char var[STRSZ];
  size_t j;

  if (sscanf( buffer, "%s %u", var, n_desi) < 2)
  {
    ERROR_MSG("malformed input '%s' for number of desired reflexes\n", buffer);
    ERROR_RETURN(-1);
  }
  for ( j=0; fgets(buffer, STRSZ, fp) != NULL && j < *n_desi; j++)
  {
    if ( buffer[0] == 'r' )
    {
      if (sscanf( ++buffer, "%lf %lf", &ref[*n_ref].lind1, &ref[*n_ref].lind2) < 2)
      {
        ERROR_MSG("malformed (h, k) index input '%s' for referenced "
                  "mkiv_reflex\n", buffer);
        ERROR_RETURN(-1);
      }
      ++*n_ref;
    }
    if ( buffer[0] == 'i' )
    {
      if (sscanf(++buffer, "%lf %lf",
                 &excl[*n_excl].lind1, &excl[*n_excl].lind2) < 2)
      {
        ERROR_MSG("malformed (h, k) index "
                  "input '%s' for ignored mkiv_reflex\n", buffer);
        ERROR_RETURN(-1);
      }
      ++*n_excl;
    }
    if (sscanf( buffer, "%lf %lf", &desi[j].lind1, &desi[j].lind2))
    {
      ERROR_MSG("malformed (h, k) index input '%s' for mkiv_reflex\n", buffer);
      ERROR_RETURN(-1);
    }
  }

  return(0);
} /* end of function getindices */
