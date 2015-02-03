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
  cleed_vector *psum = CLEED_VECTOR_ALLOC(ndim);
  real amotry();

  char old_file[FILENAME_MAX];
  char new_file[FILENAME_MAX];
  time_t result;

  *nfunk = 0;

  /* get sum of parameters */
  for (i=0; i < ndim; i++)
  {
    for (sum=0.0, j=0; j < mpts; j++)
      sum += CLEED_BASIC_MATRIX_GET(p, i, j, mpts, ndim);
    CLEED_VECTOR_SET(psum, j, sum);
  }

  for (;;)
  {
    ilo = 0;
    ihi = CLEED_VECTOR_GET(y, 0) > CLEED_VECTOR_GET(y, 1) ?
      (inhi = 1,0) : (inhi = 0,1);
    for (i=0; i < mpts; i++)
    {
      if (CLEED_VECTOR_GET(y, i) < CLEED_VECTOR_GET(y, ilo)) ilo=i;
      if (CLEED_VECTOR_GET(y, i) > CLEED_VECTOR_GET(y, ihi))
      {
        inhi = ihi;
        ihi = i;
      } else if (CLEED_VECTOR_GET(y, i) > CLEED_VECTOR_GET(y, inhi))
        if (i != ihi) inhi=i;
    }
    /* relative deviation:
     * rtol=2.0*fabs(CLEED_VECTOR_GET(y, ihi)-y[ilo])/(fabs(CLEED_VECTOR_GET(y, ihi))+fabs(y[ilo]));
     */
    /* absolute deviation: */
    rtol = 2.0*fabs(CLEED_VECTOR_GET(y, ihi) - CLEED_VECTOR_GET(y, ilo));
    if (rtol < ftol) break;
    if (*nfunk >= MAX_ITER_AMOEBA)
    {
      ERROR_MSG("Too many interations in AMOEBA\n");
		  exit(1);
    }
    ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, -ALPHA);
    if (ytry <= y[ilo])
      ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, GAMMA);
    else if (ytry >= y[inhi])
    {
      ysave = CLEED_VECTOR_GET(y, ihi);
      ytry = amotry(p, y, psum, ndim, funk, ihi, nfunk, BETA);
      if (ytry >= ysave)
      {
        for (i=0; i < mpts; i++)
        {
          if (i != ilo)
          {
            for (j=0; j < ndim; j++)
            {
              CLEED_VECTOR_SET(psum, j, 0.5*(
                             CLEED_BASIC_MATRIX_GET(p, i, j, ndim+1, ndim) +
                             CLEED_BASIC_MATRIX_GET(p, ilo, j, ndim+1, ndim)));
              CLEED_BASIC_MATRIX_SET(p, i, j, ndim+1, ndim,
                                          CLEED_VECTOR_GET(psum, j));
            }
            CLEED_VECTOR_SET(y, i, (*funk)(psum));
          }
        }
        *nfunk += ndim;

        /* recalculate sum of parameters */
        for (i=0; i < ndim; i++)
        {
          for (sum=0.0, j=0; j < mpts; j++)
            sum += CLEED_BASIC_MATRIX_GET(p, i, j, mpts, ndim);
          CLEED_VECTOR_SET(psum, j, sum);
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
      CLEED_VECTOR_FREE(psum);
      exit(1);
    }

    strcpy(ver_file, new_file);
    ver_stream = fopen(ver_file,"w");
    fprintf(ver_stream, "%u %u %s\n", ndim, mpts, sr_project);
    for (i=0; i < mpts; i++)
    {
      fprintf(ver_stream, "%e ", CLEED_VECTOR_GET(y, i));
      for(j=0; j< ndim; j++) fprintf(ver_stream, "%e ", p[i][j]);
      fprintf(ver_stream, "\n");
    }

    /* add date */
    result = time(NULL);
    fprintf(ver_stream, "%s\n", asctime(localtime(&result)));

    fclose(ver_stream);

  }
  free_vector(psum,1);
}  /* end of function sr_amoeba */

real amotry(cleed_basic_matrix *p, cleed_vector *y, cleed_vector *psum,
            size_t ndim, cleed_real (*funk)(), size_t ihi,
            size_t *nfunk, cleed_real fac)
{
  size_t j;
  real fac1, fac2, ytry, faux;
  cleed_vector *ptry = CLEED_VECTOR_ALLOC(ndim);

  fac1 = (1.0 - fac)/ndim;
  fac2 = fac1 - fac;
  for (j=0; j < ndim; j++)
    CLEED_VECTOR_SET(ptry, j, CLEED_VECTOR_GET(psum, j)*fac1 -
      CLEED_BASIC_MATRIX_GET(p, ihi, j, ndim+1, ndim)*fac2);
  ytry = (*funk)(ptry);
  ++(*nfunk);
  if (ytry < CLEED_VECTOR_GET(y, ihi))
  {
    CLEED_VECTOR_SET(y, ihi, ytry);
    for (j=0; j < ndim; j++)
    {
      faux = CLEED_VECTOR_GET(psum, j);
      CLEED_VECTOR_SET(psum, j, faux + CLEED_VECTOR_GET(ptry, j) -
                       CLEED_BASIC_MATRIX_GET(p, ihi, j, ndim+1, ndim));
      CLEED_BASIC_MATRIX_SET(p, ihi, j, ndim+1, ndim, CLEED_VECTOR_GET(ptry, j));
    }
  }
  CLEED_VECTOR_FREE(ptry);
  return ytry;
}  /* end of function amotry */
