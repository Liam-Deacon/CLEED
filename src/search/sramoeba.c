/*********************************************************************
 *                        SRAMOEBA.C
 *
 * Changes:
 *   NR/23.07.93 - Copy from Num. Rec.
 *   BN/   07.93 - Copy vertex to *.ver after each iteration.
 *   GH/12.09.95 - Criterion of termination is absolute deviation in
 *                 vertex rather than relative deviation.
 * LD/30.04.14 - Removed dependence on 'cp' and 'date' system calls
 *********************************************************************/

/*!
 * Finds the minimum by simplex algorithm. (From Num. Rec.)
 */

#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>

#include "csearch.h"
#include "copy_file.h"

static const real ALPHA = 1.0;
static const real BETA = 0.5;
static const real GAMMA = 2.0;

extern char *sr_project;

void sr_amoeba(cleed_basic_matrix *p, cleed_vector *y, size_t ndim,
               cleed_real ftol, cleed_real (*funk)(), size_t *nfunk)
{
  FILE *ver_stream;
  char ver_file[STRSZ];

  size_t i, j, ilo, ihi, inhi;
  size_t mpts = ndim + 1;
  real ytry, ysave, sum, rtol;
  cleed_vector *psum = cleed_vector_alloc(ndim);
  real amotry();

  char old_file[FILENAME_MAX];
  char new_file[FILENAME_MAX];
  time_t result;

  *nfunk = 0;

  /* get sum of parameters */
  for (i=0; i < ndim; i++)
  {
    for (sum=0.0, j=0; j < mpts; j++)
      sum += cleed_basic_matrix_get(p, i, j, ndim);
    cleed_vector_set(psum, j, sum);
  }

  for (;;)
  {
    ilo = 0;
    ihi = cleed_vector_get(y, 0) > cleed_vector_get(y, 1) ?
      (inhi = 1u,0u) : (inhi = 0u,1u);
    for (i=0; i < mpts; i++)
    {
      if (cleed_vector_get(y, i) < cleed_vector_get(y, ilo)) ilo=i;
      if (cleed_vector_get(y, i) > cleed_vector_get(y, ihi))
      {
        inhi = ihi;
        ihi = i;
      } else if (cleed_vector_get(y, i) > cleed_vector_get(y, inhi))
        if (i != ihi) inhi=i;
    }
    /* relative deviation:
     * rtol=2.0*fabs(cleed_vector_get(y, ihi)-y[ilo])/(fabs(cleed_vector_get(y, ihi))+fabs(y[ilo]));
     */
    /* absolute deviation: */
    rtol = 2.0*fabs(cleed_vector_get(y, ihi) - cleed_vector_get(y, ilo));
    if (rtol < ftol) break;
    if (*nfunk >= MAX_ITER_AMOEBA)
    {
      ERROR_MSG("Too many interations in AMOEBA\n");
		  exit(1);
    }
    ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, -ALPHA);
    if (ytry <= cleed_vector_get(y, ilo))
      ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, GAMMA);
    else if (ytry >= cleed_vector_get(y, inhi))
    {
      ysave = cleed_vector_get(y, ihi);
      ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, BETA);
      if (ytry >= ysave)
      {
        for (i=0; i < mpts; i++)
        {
          if (i != ilo)
          {
            for (j=0; j < ndim; j++)
            {
              cleed_vector_set(psum, j, 0.5*(
                             cleed_basic_matrix_get(p, i, j, ndim) +
                             cleed_basic_matrix_get(p, ilo, j, ndim)));
              cleed_basic_matrix_set(p, i, j, ndim, cleed_vector_get(psum, j));
            }
            cleed_vector_set(y, i, (*funk)(psum));
          }
        }
        *nfunk += ndim;

        /* recalculate sum of parameters */
        for (i=0; i < ndim; i++)
        {
          for (sum=0.0, j=0; j < mpts; j++)
            sum += cleed_basic_matrix_get(p, i, j, ndim);
          cleed_vector_set(psum, j, sum);
        }
      }
    }

    /*****************************
     * Write y/p to backup file
     *****************************/

    /* remove 'cp' system call dependence */
    sprintf(old_file, "%s%s", sr_project, ".ver");
    sprintf(new_file, "%s%s", sr_project, ".vbk");
    if (copy_file(old_file, new_file))
    {
      ERROR_MSG("failed to copy file \"%s\" -> \"%s\"", old_file, new_file);
      cleed_vector_free(psum);
      exit(1);
    }

    strcpy(ver_file, new_file);
    ver_stream = fopen(ver_file,"w");
    fprintf(ver_stream, "%u %u %s\n", ndim, mpts, sr_project);
    for (i=0; i < mpts; i++)
    {
      fprintf(ver_stream, "%e ", cleed_vector_get(y, i));
      for(j=0; j < ndim; j++)
        fprintf(ver_stream, "%e ", cleed_basic_matrix_get(p, i, j, ndim));
      fprintf(ver_stream, "\n");
    }

    /* add date */
    result = time(NULL);
    fprintf(ver_stream, "%s\n", asctime(localtime(&result)));

    fclose(ver_stream);

  }
  cleed_vector_free(psum);
}  /* end of function sr_amoeba */

real amotry(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum,
            size_t ndim, cleed_real (*funk)(), size_t ihi,
            size_t *nfunk, cleed_real fac)
{
  size_t j;
  real fac1, fac2, ytry, faux;
  cleed_vector *ptry = cleed_vector_alloc(ndim);

  fac1 = (1.0 - fac)/ndim;
  fac2 = fac1 - fac;
  for (j=0; j < ndim; j++)
    cleed_vector_set(ptry, j, cleed_vector_get(psum, j)*fac1 -
      cleed_basic_matrix_get(p, ihi, j, ndim)*fac2);
  ytry = (*funk)(ptry);
  ++(*nfunk);
  if (ytry < cleed_vector_get(y, ihi))
  {
    cleed_vector_set(y, ihi, ytry);
    for (j=0; j < ndim; j++)
    {
      faux = cleed_vector_get(psum, j);
      cleed_vector_set(psum, j, faux + cleed_vector_get(ptry, j) -
                       cleed_basic_matrix_get(p, ihi, j, ndim));
      cleed_basic_matrix_set(p, ihi, j, ndim, cleed_vector_get(ptry, j));
    }
  }
  cleed_vector_free(ptry);
  return ytry;
}  /* end of function amotry */
