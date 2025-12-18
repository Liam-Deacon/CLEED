/***********************************************************************
GH/29.12.95
 File contains:

  sr_sa(int ndim, char *bak_file, char *log_file)
 Perform a search according to the SIMULATED ANNEALING (SIMPLEX) METHOD
 Driver for routine sr_amebsa

Changes
GH/20.09.95 - Creation (copy from srsx.c)
GH/29.12.95 - insert dpos in parameter list: initial displacement
              can be specified through a command line option.

***********************************************************************/

/**
 * @file srsa.c
 * @brief SEARCH driver for simulated annealing (simplex).
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
#include "sr_simplex.h"

#define START_TEMP     3.5
#define EPSILON        0.25
#define ALPHA          4
#define MAX_ITER_SA  200
#define ITER_SA      100

/**********************************************************************/

long sa_idum = -1;                /* seed for random number generator */

static FILE *sr_sa_open_log_append(const char *log_file)
{
  FILE *log_stream = fopen(log_file, "a");
  if (log_stream == NULL) {
    OPEN_ERROR(log_file);
  }
  return log_stream;
}

static void sr_sa_init_simplex_or_exit(sr_simplex_buffers *b,
                                       real dpos,
                                       const char *bak_file,
                                       const char *log_file)
{
  FILE *log_stream = sr_sa_open_log_append(log_file);

  if (strncmp(bak_file, "---", 3) == 0)
  {
    fprintf(log_stream, "=> Set up vertex:\n");
    fclose(log_stream);

    if (sr_simplex_build_initial(b, dpos, sr_evalrf) != 0) {
      fprintf(STDERR, "*** error (sr_sa): failed to initialise simplex\n");
      exit(1);
    }
    return;
  }

  fprintf(log_stream, "=> Read vertex from \"%s\":\n", bak_file);
  fclose(log_stream);

  if (sr_simplex_read_vertex(b, bak_file) != 0) {
    fprintf(STDERR, "*** error (sr_sa): failed to read vertex file\n");
    exit(1);
  }
}

static int sr_sa_run_temperature_loop(sr_simplex_buffers *b, int ndim, real *out_rmin)
{
  real rmin = 100.;
  int nfunc = 0;

  for (real temp = START_TEMP; temp > R_TOLERANCE; temp *= (1. - EPSILON) )
  {
#ifdef CONTROL
    fprintf(STDCTR, "(sr_sa): temperature = %.4f\n", temp);
#endif
    int budget = MAX_ITER_SA;
    sr_amebsa_cfg cfg;
    cfg.ftol = temp;
    cfg.funk = sr_evalrf;
    cfg.temptr = temp;
    (void)sr_amebsa(b->p, b->y, ndim, b->x, &rmin, &cfg, &budget);
    nfunc += budget;
  }

  if (out_rmin) *out_rmin = rmin;
  return nfunc;
}

static void sr_sa_write_results(const char *log_file, int nfunc, int ndim, const real *best, real rmin)
{
  FILE *log_stream = sr_sa_open_log_append(log_file);
  fprintf(log_stream, "\n=> No. of iterations in sr_powell: %3d\n", nfunc);
  fprintf(log_stream, "=> Optimum parameter set and function value:\n");

  for (int j_par = 1; j_par <= ndim; j_par++ ) {
    fprintf(log_stream, "%.6f ", (double)best[j_par]);
  }
  fprintf(log_stream, "\nrmin = %.6f\n", (double)rmin);
  fclose(log_stream);
}

void sr_sa(int ndim, real dpos, const char *bak_file, const char *log_file)
{
  sr_simplex_buffers b;
  if (sr_simplex_buffers_alloc(&b, ndim) != 0)
  {
    sr_simplex_buffers_free(&b);
    fprintf(STDERR, "*** error (sr_sa): allocation failure\n");
    exit(1);
  }

  /***********************************************************************
    SIMULATED ANNEALING (SIMPLEX METHOD)
  ***********************************************************************/

  FILE *log_stream = sr_sa_open_log_append(log_file);
  fprintf(log_stream, "=> SIMULATED ANNEALING:\n\n");
  fclose(log_stream);

  sr_sa_init_simplex_or_exit(&b, dpos, bak_file, log_file);

  /***********************************************************************
    Enter temperature loop
  ***********************************************************************/

#ifdef CONTROL
  fprintf(STDCTR, "(sr_sa): Enter temperature loop\n");
#endif

  log_stream = sr_sa_open_log_append(log_file);
  fprintf(log_stream, "=> Start search (abs. tolerance = %.3e)\n", R_TOLERANCE);
  fclose(log_stream);

  real rmin = 0.0;
  int nfunc = sr_sa_run_temperature_loop(&b, ndim, &rmin);

  /***********************************************************************
    Write final results to log file
  ***********************************************************************/

#ifdef CONTROL
  fprintf(STDCTR, "(sr_sa): %d function evaluations in sr_amebsa\n", nfunc);
#endif
  sr_sa_write_results(log_file, nfunc, ndim, b.x, rmin);

  sr_simplex_buffers_free(&b);

} /* end of function sr_sa */

/***********************************************************************/
