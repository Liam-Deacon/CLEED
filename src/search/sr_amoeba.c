/*********************************************************************
 *                       SR_AMOEBA.C
 *
 *  GPL-3.0-or-later
 *
 *  Open implementation of a Nelder–Mead (downhill simplex) minimiser.
 *
 *  The API matches legacy SEARCH code conventions:
 *  - simplex vertices are stored in p[1..ndim+1][1..ndim]
 *  - function values are stored in y[1..ndim+1]
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>
// cppcheck-suppress missingIncludeSystem
#include <time.h>

#include "copy_file.h"
#include "search.h"

#ifndef MAX_ITER_AMOEBA
#define MAX_ITER_AMOEBA 2000
#endif

static int sr_write_vertex_file(const real **p, const real *y, int ndim)
{
  if (sr_project == NULL || sr_project[0] == '\0') return 0;

  char ver_file[STRSZ];
  char vbk_file[STRSZ];

  /* Best-effort backup of the previous vertex file. */
  (void)snprintf(ver_file, sizeof(ver_file), "%s.ver", sr_project);
  (void)snprintf(vbk_file, sizeof(vbk_file), "%s.vbk", sr_project);

  (void)copy_file(ver_file, vbk_file);

  FILE *fp = fopen(ver_file, "w");
  if (fp == NULL) return -1;

  int mpts = ndim + 1;
  fprintf(fp, "%d %d %s\n", ndim, mpts, sr_project);
  for (int i = 1; i <= mpts; i++) {
    fprintf(fp, "%e ", (double)y[i]);
    for (int j = 1; j <= ndim; j++) {
      fprintf(fp, "%e ", (double)p[i][j]);
    }
    fputc('\n', fp);
  }

  time_t now = time(NULL);
  const struct tm *tm_info = localtime(&now);
  if (tm_info != NULL) {
    char timebuf[64];
    if (strftime(timebuf, sizeof(timebuf), "%c", tm_info) > 0) {
      fprintf(fp, "%s\n", timebuf);
    }
  }

  fclose(fp);
  return 0;
}

static void sr_copy_point(real *dst, const real *src, int ndim)
{
  for (int j = 1; j <= ndim; j++) dst[j] = src[j];
}

static void sr_centroid_excluding(const real **p, real *centroid, int ndim, int exclude_index)
{
  int mpts = ndim + 1;
  for (int j = 1; j <= ndim; j++) centroid[j] = 0.0;

  for (int i = 1; i <= mpts; i++) {
    if (i == exclude_index) continue;
    for (int j = 1; j <= ndim; j++) centroid[j] += p[i][j];
  }

  /* There are ndim points when excluding one vertex. */
  for (int j = 1; j <= ndim; j++) centroid[j] /= (real)ndim;
}

static void sr_simplex_extremes(const real *y, int ndim, int *ilo, int *ihi, int *inhi)
{
  int mpts = ndim + 1;

  *ilo = 1;
  *ihi = (y[1] > y[2]) ? 1 : 2;
  *inhi = (y[1] > y[2]) ? 2 : 1;

  for (int i = 1; i <= mpts; i++) {
    if (y[i] < y[*ilo]) *ilo = i;
    if (y[i] > y[*ihi]) {
      *inhi = *ihi;
      *ihi = i;
    } else if (i != *ihi && y[i] > y[*inhi]) {
      *inhi = i;
    }
  }
}

// cppcheck-suppress functionLength
// cppcheck-suppress functionComplexity
int sr_amoeba(real **p, real *y, int ndim, real ftol, real (*funk)(real *), int *nfunk)
{
  if (p == NULL || y == NULL || ndim <= 0 || funk == NULL || nfunk == NULL) {
    return -1;
  }

  const real alpha = 1.0;  /* reflection */
  const real gamma = 2.0;  /* expansion */
  const real rho = 0.5;    /* contraction */
  const real sigma = 0.5;  /* shrink */

  int mpts = ndim + 1;
  *nfunk = 0;

  real *centroid = (real *)calloc((size_t)ndim + 1, sizeof(real));
  real *trial = (real *)calloc((size_t)ndim + 1, sizeof(real));
  real *trial2 = (real *)calloc((size_t)ndim + 1, sizeof(real));
  if (centroid == NULL || trial == NULL || trial2 == NULL) {
    free(centroid);
    free(trial);
    free(trial2);
    return -1;
  }

  for (;;) {
    int ilo, ihi, inhi;
    sr_simplex_extremes(y, ndim, &ilo, &ihi, &inhi);

    /* Absolute convergence on function range (matches historical SEARCH behaviour). */
    if (fabs((double)(y[ihi] - y[ilo])) < (double)ftol) {
      break;
    }

    if (*nfunk >= MAX_ITER_AMOEBA) {
      free(centroid);
      free(trial);
      free(trial2);
      return -2;
    }

    /* Centroid excluding worst. */
    sr_centroid_excluding((const real **)p, centroid, ndim, ihi);

    /* Reflection: xr = c + alpha*(c - xh) */
    for (int j = 1; j <= ndim; j++) {
      trial[j] = centroid[j] + alpha * (centroid[j] - p[ihi][j]);
    }
    real fr = (*funk)(trial);
    (*nfunk)++;

    if (fr < y[ilo]) {
      /* Expansion: xe = c + gamma*(xr - c) */
      for (int j = 1; j <= ndim; j++) {
        trial2[j] = centroid[j] + gamma * (trial[j] - centroid[j]);
      }
      real fe = (*funk)(trial2);
      (*nfunk)++;

      if (fe < fr) {
        sr_copy_point(p[ihi], trial2, ndim);
        y[ihi] = fe;
      } else {
        sr_copy_point(p[ihi], trial, ndim);
        y[ihi] = fr;
      }
    } else if (fr < y[inhi]) {
      /* Accept reflection. */
      sr_copy_point(p[ihi], trial, ndim);
      y[ihi] = fr;
    } else {
      /* Contraction. */
      int outside = (fr < y[ihi]);
      if (outside) {
        /* Outside contraction: xc = c + rho*(xr - c) */
        for (int j = 1; j <= ndim; j++) {
          trial2[j] = centroid[j] + rho * (trial[j] - centroid[j]);
        }
      } else {
        /* Inside contraction: xc = c + rho*(xh - c) */
        for (int j = 1; j <= ndim; j++) {
          trial2[j] = centroid[j] + rho * (p[ihi][j] - centroid[j]);
        }
      }

      real fc = (*funk)(trial2);
      (*nfunk)++;

      if (fc < y[ihi]) {
        sr_copy_point(p[ihi], trial2, ndim);
        y[ihi] = fc;
      } else {
        /* Shrink towards best. */
        for (int i = 1; i <= mpts; i++) {
          if (i == ilo) continue;
          for (int j = 1; j <= ndim; j++) {
            p[i][j] = p[ilo][j] + sigma * (p[i][j] - p[ilo][j]);
            trial[j] = p[i][j];
          }
          y[i] = (*funk)(trial);
          (*nfunk)++;
        }
      }
    }

    (void)sr_write_vertex_file((const real **)p, y, ndim);
  }

  free(centroid);
  free(trial);
  free(trial2);

  return 0;
}
