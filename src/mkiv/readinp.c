/*********************************************************************
 *                        READINP.C
 *
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *    CS/07.09.93 - creation
 *********************************************************************/

/*! \file
 *
 * Implements readinp() , getdomain() and getindices() functions.
 */

#include <ctype.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include "mkiv.h"
#include "inputs.h"

/*!
 * Reads some necessary data statements from the file mkiv.inp
 * if variables don't occur in the input file, defaults are taken instead
 *
 * \param[out] verb Flag for verbose output.
 * \param[in,out] inp_path
 * \param[in,out] param_path
 * \param[out] ref_name
 * \param[out] mask_name
 * \param[out] nstart
 * \param[out] nstop
 * \param[out] e_step Energy step.
 * \param[out] cen Center of screen.
 * \param[out] router Outer radius of screen.
 * \param[out] rinner Inner radius of screen.
 * \param[out] bg Flag for background.
 * \param[out] smooth Flag for smoothing.
 * \param kpl_10 Length of the k_parallel vector in the <1,0> direction.
 * \param[out] ndom Number of superstructure domains in \p superlat .
 * \param[out] superlat Array of superstructure domains.
 * \param[out] ndesi Number of desired reflexes in \p desi .
 * \param[out] desi Array of desired reflexes.
 * \param[out] nref Number of reference reflexes in \p ref .
 * \param[out] ref Array of reference reflexes.
 * \param[out] nexcl Number of excluded reflexes in \p excl .
 * \param[out] excl Array of excluded reflexes.
 *  * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if unsuccessful.
 */
int readinp(int verb, char *inp_path, char *param_path, char *ref_name, 
            char *mask_name, size_t *nstart, size_t *nstop, float *e_step,
            mkiv_position *cen, float *router, float *rinner, int *bg,
            int *smooth, float *kpl_10, size_t *ndom, mkiv_domain *superlat,
            size_t *ndesi, mkiv_index *desi, size_t *nref, mkiv_index *ref,
            size_t *nexcl, mkiv_index *excl )
{
  int i;
  size_t nndom = 0;
  float fac, interim;
  char buf[STRSZ], var[STRSZ];
  FILE *fp;

  /* Preset variables */
  *nstart	= 40;
  *nstop = 400;
  *e_step	= 1;
  cen->xx	= 250;
  cen->yy	= 250;
  *router	= 230;
  *rinner	= 0;
  *bg = 2;
  *smooth = -1;
  *kpl_10 = 2.6771;
  *ndom = 0;

  /* Open statement */
  if (inp_path == NULL) strcpy(inp_path, "mkiv.inp");
  if ( (fp = fopen(inp_path, "r")) == NULL)
  {
    fprintf(stderr, "***error (readinp): failed to open '%s'\n",
            inp_path);

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  }
  else printf("\nreading input from \"%s\":\n", inp_path);
     
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
      case  1 :  sscanf( buf, "%s %s", var, ref_name); break;
      case  2 :  sscanf( buf, "%s %s", var, mask_name); break;
      case  3 :  sscanf( buf, "%s %d", var, nstart); break;
      case  4 :  sscanf( buf, "%s %d", var, nstop); break;
      case  5 :  sscanf( buf, "%s %f", var, e_step); break;
      case  6 :  sscanf( buf, "%s %d", var, &(cen->xx)); break;
      case  7 :  sscanf( buf, "%s %d", var, &(cen->yy)); break;
      case  8 :  sscanf( buf, "%s %f", var, router); break;
      case  9 :  sscanf( buf, "%s %f", var, rinner); break;
      case 10 :  sscanf( buf, "%s %d", var, bg); break;
      case 11 :  sscanf( buf, "%s %d", var, smooth); break;
      case 12 :  sscanf( buf, "%s %f", var, kpl_10); break;
      case 13 :  sscanf( buf, "%s %d", var, ndom); break;
      case 14 :  getdomain( buf, &superlat[nndom++]);	break;
      case 15 :  getindices( fp, buf, ndesi, desi, nref,
                             ref, nexcl, excl); break;
      default :
      {
        buf[strlen(buf)-1] = '\0';
        fprintf(stderr, "***error (readinp):"
                " wrong arguments in line '%s' \n", buf);
      }
    }
  }
  fclose(fp);

  if (param_path == NULL) strcpy(param_path, "mkiv.param");
  if ( (fp = fopen(param_path, "w")) == NULL)
  {
    fprintf(stderr, "***error (readvar): unable to write to '%s'\n",
            param_path);
  }
  else
  {
    VPRINT("writing parameter input-values to '%s'\n\n", param_path);
    fprintf(fp, "ref_name = %s \n", ref_name);
    fprintf(fp, "mask_name = %s \n", mask_name);
    fprintf(fp, "nstart = %d \n", *nstart);
    fprintf(fp, "nstop = %d \n", *nstop);
    fprintf(fp, "n_step = %f \n", *e_step);
    fprintf(fp, "center = (%d,%d) \n", cen->xx, cen->yy);
    fprintf(fp, "router = %.0f \n",*router);
    fprintf(fp, "rinner = %.0f \n", *rinner);
    fprintf(fp, "bg = %s \n", ((*var&&(*bg==2))?"YES":"NO"));
    fprintf(fp, "smoothcurrent = %s \n",
                ((*var&&(*smooth==-1))?"YES":"NO"));
    fprintf(fp, "kp_len_10 = %f \n", *kpl_10);
    fprintf(fp, "ndom = %d \n", *ndom);
    for (i=0; i < *ndom; i++)
    {
      fprintf(fp, "%f %f %f %f \n",
                superlat[i].lind11, superlat[i].lind12,
                superlat[i].lind21, superlat[i].lind22);
    }
    fprintf(fp, "ndesi = %d \n", *ndesi);

    for (i=0; i< * ndesi && i < 9; i++)
    {
      fprintf(fp, "%f %f \n", desi[i].lind1, desi[i].lind2);
    }
    if (i < *ndesi) fprintf(fp, "\t.\n\t.\n\t.\n\n");
    fclose(fp);
  }

  printf("Total of %d spots.\n",*ndesi);
  if (*nref > 2)  printf("%d spots used for recalibration;\n", *nref);
  else            printf("Recalibration with most intense spots;\n");
  printf(" No maximum search for %d spots.\n", *nexcl);

  /* inversion of superlattice matrices */
  for (i=0; i < *ndom; i++)
  {
    fac = superlat[i].lind11 * superlat[i].lind22 -
          superlat[i].lind12 * superlat[i].lind21;
    interim = superlat[i].lind11;
    superlat[i].lind11 = superlat[i].lind22 / fac;
    superlat[i].lind22 = interim / fac;
    interim = superlat[i].lind12;
    superlat[i].lind12 = -superlat[i].lind21 / fac;
    superlat[i].lind21 = -interim / fac;
  }
    
  return(0);
}


/*!
 * Reads the superstructure domain information from a string.
 *
 * \param[in] buffer Pointer to buffer string to read.
 * \param[out] superlat Pointer to superstructure domain.
 *  * \return Value returned by sscanf() if an error occurs.
 * \retval 0 Otherwise
 */
int getdomain(const char *buffer, mkiv_domain *superlat)
{
  char var[STRSZ];
  int aux;

  aux = sscanf( buffer, "%s %f %f %f %f", var,
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
 * \param[out] ndesi Number of desired reflexes in \p desi .
 * \param[out] desi Array of desired reflexes.
 * \param[out] nref Number of reference reflexes in \p ref .
 * \param[out] ref Array of reference reflexes.
 * \param[out] nexcl Number of excluded reflexes in \p excl .
 * \param[out] excl Array of excluded reflexes.
 *  * \return C style return code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if unsuccessful.
 */
int getindices(FILE *fp, char *buffer, size_t *ndesi, mkiv_index *desi,
               size_t *nref, mkiv_index *ref, size_t *nexcl, mkiv_index *excl)
{
  char var[STRSZ];
  size_t j;

  if (sscanf( buffer, "%s %u", var, ndesi) < 2)
  {
    fprintf(stderr, "***error (getindices): malformed input '%s' for number "
        "of desired reflexes\n", buffer);

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  }
  for ( j=0; fgets(buffer, STRSZ, fp) != NULL && j<*ndesi; j++)
  {
    if ( buffer[0] == 'r' )
    {
      if (sscanf( ++buffer, "%f %f", &ref[*nref].lind1, &ref[*nref].lind2) < 2)
      {
        fprintf(stderr, "***error (getindices): malformed (h, k) index "
                "input '%s' for referenced mkiv_reflex\n", buffer);

        #ifdef EXIT_ON_ERROR
        exit(1);
        #elif RETURN_ON_ERROR
        return(-1);
        #endif
      }
      ++*nref;
    }
    if ( buffer[0] == 'i' )
    {
      if (sscanf(++buffer, "%f %f",
                 &excl[*nexcl].lind1, &excl[*nexcl].lind2) < 2)
      {
        fprintf(stderr, "***error (getindices): malformed (h, k) index "
                "input '%s' for ignored mkiv_reflex\n", buffer);

        #ifdef EXIT_ON_ERROR
        exit(1);
        #elif RETURN_ON_ERROR
        return(-1);
        #endif
      }
      ++*nexcl;
    }
    if (sscanf( buffer, "%f %f", &desi[j].lind1, &desi[j].lind2))
    {
      fprintf(stderr, "***error (getindices): "
              "malformed (h, k) index input '%s' for mkiv_reflex\n", buffer);

      #ifdef EXIT_ON_ERROR
      exit(1);
      #elif RETURN_ON_ERROR
      return(-1);
      #endif
    }
  }

  return(0);
} /* end of function getindices */
