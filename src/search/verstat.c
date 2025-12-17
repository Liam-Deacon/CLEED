/***********************************************************************
GH/28.09.95
 Check some statistics in the vertex file
 Modified:
 GH/28.09.95
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "search.h"
#include "sr_alloc.h"
#include "sr_vertex_stats.h"

#define CONTROL
#define ERROR

#define EXIT_ON_ERROR

/* only for compatibility with SEARCH project */

struct sratom_str *sr_atoms;
struct search_str *sr_search;
char *sr_project;

/**********************************************************************/

// cppcheck-suppress constParameter
static int verstat_parse_args(int argc, char *const argv[], char inp_file[STRSZ])
{
  strncpy(inp_file, "---", STRSZ);
  for (int i_arg = 1; i_arg < argc; i_arg++)
  {
    if (*argv[i_arg] != '-')
    {
      fprintf(STDERR, "*** error (verstat):\tsyntax error:\n");
      fprintf(STDERR, "\tusage: \tverstat -i <inp_file>\n");
      return 1;
    }
    if (strncmp(argv[i_arg], "-i", 2) == 0)
    {
      i_arg++;
      if (i_arg >= argc)
      {
        fprintf(STDERR, "*** error (verstat): no input file specified\n");
        return 1;
      }
      strncpy(inp_file, argv[i_arg], STRSZ);
    }
  }
  if (strncmp(inp_file, "---", 3) == 0)
  {
    fprintf(STDERR,
            "*** error (verstat): no parameter input file (option -i) specified\n");
    return 1;
  }
  return 0;
}

static int verstat_read_ndim(const char *inp_file, int *out_ndim)
{
  FILE *inp_stream = fopen(inp_file, "r");
  if (inp_stream == NULL)
  {
    fprintf(STDERR, " *** error (verstat): could not open file \"%s\"\n", inp_file);
    return 1;
  }

  char linebuffer[STRSZ];
  while (fgets(linebuffer, STRSZ, inp_stream) != NULL && linebuffer[0] == '#') {;}
  if (sscanf(linebuffer, "%d", out_ndim) != 1)
  {
    fclose(inp_stream);
    fprintf(STDERR, " *** error (verstat): failed to read ndim from \"%s\"\n", inp_file);
    return 1;
  }

  fclose(inp_stream);
  return 0;
}

static void verstat_print_vector(const char *label, real y, const real *p, int ndim)
{
  fprintf(STDOUT, "%s: %7.4f :", label, (double)y);
  for (int j = 1; j <= ndim; j++)
  {
    fprintf(STDOUT, " %6.3f", (double)p[j]);
  }
  fprintf(STDOUT, "\n");
}

static int verstat_load_vertices(const char *inp_file, int ndim, real **out_y, real ***out_p)
{
  const int mpar = ndim + 1;
  real *y = sr_alloc_vector((size_t)mpar);
  real **p = sr_alloc_matrix((size_t)mpar, (size_t)ndim);
  if (y == NULL || p == NULL)
  {
    sr_free_vector(y);
    sr_free_matrix(p);
    return 1;
  }

  if (sr_rdver(inp_file, y, p, ndim) != 1)
  {
    sr_free_vector(y);
    sr_free_matrix(p);
    return 1;
  }

  *out_y = y;
  *out_p = p;
  return 0;
}

static int verstat_alloc_stats(int ndim, real **out_min_p, real **out_max_p, real **out_avg_p, real **out_dev_p)
{
  real *min_p = sr_alloc_vector((size_t)ndim);
  real *max_p = sr_alloc_vector((size_t)ndim);
  real *avg_p = sr_alloc_vector((size_t)ndim);
  real *dev_p = sr_alloc_vector((size_t)ndim);
  if (min_p == NULL || max_p == NULL || avg_p == NULL || dev_p == NULL)
  {
    sr_free_vector(min_p);
    sr_free_vector(max_p);
    sr_free_vector(avg_p);
    sr_free_vector(dev_p);
    return 1;
  }

  *out_min_p = min_p;
  *out_max_p = max_p;
  *out_avg_p = avg_p;
  *out_dev_p = dev_p;
  return 0;
}

static void verstat_print_stats(int ndim,
                                real min_y, const real *min_p,
                                real max_y, const real *max_p,
                                real avg_y, const real *avg_p,
                                real dev_y, const real *dev_p)
{
  fprintf(STDOUT, "\n");
  verstat_print_vector("min", min_y, min_p, ndim);
  verstat_print_vector("max", max_y, max_p, ndim);
  fprintf(STDOUT, "\n");
  verstat_print_vector("avg", avg_y, avg_p, ndim);
  verstat_print_vector("dev", dev_y, dev_p, ndim);
}

int main(int argc, char *argv[])
{
  char inp_file[STRSZ];
  if (verstat_parse_args(argc, argv, inp_file) != 0) return 1;

  int ndim = 0;
  if (verstat_read_ndim(inp_file, &ndim) != 0) return 1;

  real *y = NULL;
  real **p = NULL;
  if (verstat_load_vertices(inp_file, ndim, &y, &p) != 0)
  {
    fprintf(STDERR, "*** error (verstat): failed to read vertex file\n");
    return 1;
  }

  real *min_p = NULL;
  real *max_p = NULL;
  real *avg_p = NULL;
  real *dev_p = NULL;
  if (verstat_alloc_stats(ndim, &min_p, &max_p, &avg_p, &dev_p) != 0)
  {
    sr_free_vector(y);
    sr_free_matrix(p);
    fprintf(STDERR, "*** error (verstat): allocation failure\n");
    return 1;
  }

  real min_y = 0.0, max_y = 0.0, avg_y = 0.0, dev_y = 0.0;
  const int mpar = ndim + 1;
  sr_vertex_minmax(y, p, ndim, mpar, &min_y, &max_y, min_p, max_p);
  sr_vertex_avg(y, p, ndim, mpar, &avg_y, avg_p);
  sr_vertex_dev(y, p, ndim, mpar, avg_y, avg_p, &dev_y, dev_p);

  verstat_print_stats(ndim, min_y, min_p, max_y, max_p, avg_y, avg_p, dev_y, dev_p);

  sr_free_vector(min_p);
  sr_free_vector(max_p);
  sr_free_vector(avg_p);
  sr_free_vector(dev_p);
  sr_free_vector(y);
  sr_free_matrix(p);

  return 0;
}  /* end of main */

/**********************************************************************/
