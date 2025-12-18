/***********************************************************************
GH/29.12.95
 File contains:

  sr_sx(int ndim, real dpos, char *bak_file, char *log_file)
 Perform a search according to the SIMPLEX METHOD
 Driver for routine AMOEBA (From numerical recipes)

 Modified:
GH/23.08.95
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.

***********************************************************************/

/**
 * @file srsx.c
 * @brief SEARCH driver for the Nelder–Mead simplex minimiser.
 */

// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "search.h"
#include "sr_alloc.h"
#include "sr_simplex.h"
#include "sr_vertex_stats.h"

/**********************************************************************/

static FILE *sr_sx_open_log_append(const char *log_file)
{
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    OPEN_ERROR(log_file);
  }
  return log_stream;
}

static void sr_sx_log_final_simplex(FILE *log_stream, const sr_simplex_buffers *b, int nfunc)
{
  fprintf(log_stream, "\n=> No. of function evaluations in sr_amoeba: %3d\n", nfunc);
  fprintf(log_stream, "=> Vertices and function values of final 3-d simplex:\n");

  for (int i_par = 1; i_par <= b->mpar; i_par++ )
  {
    if (i_par == 1) fprintf(log_stream, "%3d:", i_par);
    else fprintf(log_stream, "%3d ", i_par);

    for (int j_par = 1; j_par <= b->ndim; j_par++ ) {
      fprintf(log_stream, "%7.4f ", (double)b->p[i_par][j_par]);
    }
    fprintf(log_stream, "%7.4f\n", (double)b->y[i_par]);
  }

  real avg_y = 0.0;
  real *avg_p = sr_alloc_vector((size_t)b->ndim);
  if (avg_p == NULL) return;

  sr_vertex_avg(b->y, b->p, b->ndim, b->mpar, &avg_y, avg_p);

  fprintf(log_stream, "\navg:");
  for (int j_par = 1; j_par <= b->ndim; j_par++) {
    fprintf(log_stream, "%7.4f ", (double)avg_p[j_par]);
  }
  fprintf(log_stream, "%7.4f\n", (double)avg_y);

  sr_free_vector(avg_p);
}

void sr_sx(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  int nfunc = 0;
  sr_simplex_buffers b;

  if (sr_simplex_buffers_alloc(&b, ndim) != 0)
  {
    sr_simplex_buffers_free(&b);
    fprintf(STDERR, "*** error (sr_sx): allocation failure\n");
    exit(1);
  }

  FILE *log_stream = sr_sx_open_log_append(log_file);
  fprintf(log_stream, "=> SIMPLEX SEARCH:\n\n");

  /***********************************************************************
    Set up vertex if no vertex file was specified, read vertex otherwise.
  ***********************************************************************/

  if (strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up vertex:\n");
    fclose(log_stream);

    if (sr_simplex_build_initial(&b, dpos, sr_evalrf) != 0) {
      sr_simplex_buffers_free(&b);
      fprintf(STDERR, "*** error (sr_sx): failed to initialise simplex\n");
      exit(1);
    }
  }
  else
  {
    fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
    fclose(log_stream);

    if (sr_simplex_read_vertex(&b, bak_file) != 0) {
      sr_simplex_buffers_free(&b);
      fprintf(STDERR, "*** error (sr_sx): failed to read vertex file\n");
      exit(1);
    }
  }

  /***********************************************************************
    Enter amoeba
  ***********************************************************************/

#ifdef CONTROL
  fprintf(STDCTR, "(sr_sx): Enter amoeba\n");
#endif

  log_stream = sr_sx_open_log_append(log_file);
  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  if (sr_amoeba(b.p, b.y, ndim, R_TOLERANCE, sr_evalrf, &nfunc) != 0)
  {
    fprintf(STDERR, "*** error (sr_sx): simplex minimiser failed\n");
  }

  /***********************************************************************
    Write final results to log file
  ***********************************************************************/

#ifdef CONTROL
  fprintf(STDCTR, "(sr_sx): %d function evaluations in sr_amoeba\n", nfunc);
#endif

  log_stream = sr_sx_open_log_append(log_file);
  sr_sx_log_final_simplex(log_stream, &b, nfunc);
  fclose(log_stream);

  sr_simplex_buffers_free(&b);
} /* end of function sr_sx */

/***********************************************************************/
