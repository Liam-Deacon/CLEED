/***********************************************************************
GH/19.09.95
 File contains:

  sr_po(int ndim, char *bak_file, char *log_file)
 Perform a search according to POWELL'S METHOD

 Driver for routine AMOEBA (From numerical recipes)
 Modified:
 GH/23.08.95
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "search.h"
#include "gh_stddef.h"
#include "sr_alloc.h"

/**********************************************************************/

static FILE *sr_po_open_log_append(const char *log_file)
{
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    OPEN_ERROR(log_file);
  }
  return log_stream;
}

static void sr_po_init_direction_set(int ndim, real *p, real **xi)
{
  for (int i = 1; i <= ndim; i++) {
    p[i] = 0.0;
    for (int j = 1; j <= ndim; j++) {
      xi[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }
}

static void sr_po_write_results(FILE *log_stream, int ndim, int nfunc, real rmin, const real *p)
{
  fprintf(log_stream, "\n=> No. of iterations in sr_powell: %3d\n", nfunc);
  fprintf(log_stream, "=> Optimum parameter set:\n");
  for (int j = 1; j <= ndim; j++) {
    fprintf(log_stream, "%.6f ", p[j]);
  }
  fprintf(log_stream, "\n");

  fprintf(log_stream, "=> Optimum function value:\n");
  fprintf(log_stream, "rmin = %.6f\n", rmin);
}

typedef struct sr_po_buffers {
  real *p;
  real **xi;
} sr_po_buffers;

static int sr_po_alloc_buffers(sr_po_buffers *b, int ndim)
{
  b->p = sr_alloc_vector((size_t)ndim);
  b->xi = sr_alloc_matrix((size_t)ndim, (size_t)ndim);
  return (b->p != NULL && b->xi != NULL) ? 0 : -1;
}

static void sr_po_free_buffers(sr_po_buffers *b)
{
  sr_free_matrix(b->xi);
  sr_free_vector(b->p);
}

static void sr_po_initialise(FILE *log_stream, int ndim, const char *bak_file, sr_po_buffers *b)
{
  if (strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up initial direction set:\n");
    sr_po_init_direction_set(ndim, b->p, b->xi);
  }
  else
  {
    /* Read direction set from file: not implemented (legacy behaviour). */
  }
}

void sr_po(int ndim, const char *bak_file, const char *log_file)
{
  int nfunc = 0;
  real rmin = 0.0;

  sr_po_buffers b;
  b.p = NULL;
  b.xi = NULL;
  if (sr_po_alloc_buffers(&b, ndim) != 0)
  {
    sr_po_free_buffers(&b);
    fprintf(STDERR, "*** error (sr_po): allocation failure\n");
    exit(1);
  }

/***********************************************************************
  POWELL'S METHOD
***********************************************************************/

  FILE *log_stream = sr_po_open_log_append(log_file);
  fprintf(log_stream, "=> POWELL'S METHOD\n\n");

/***********************************************************************
  Set up initial direction set
***********************************************************************/
  sr_po_initialise(log_stream, ndim, bak_file, &b);

/***********************************************************************
  Enter search function sr_powell
***********************************************************************/

  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  if (sr_powell(b.p, b.xi, ndim, R_TOLERANCE, &nfunc, &rmin, sr_evalrf) != 0)
  {
    fprintf(STDERR, "*** error (sr_po): Powell minimiser failed\n");
  }

/***********************************************************************
  Write final results to log file
***********************************************************************/

  log_stream = sr_po_open_log_append(log_file);
  sr_po_write_results(log_stream, ndim, nfunc, rmin, b.p);
  fclose(log_stream);

  sr_po_free_buffers(&b);
}

/***********************************************************************/
