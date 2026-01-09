/***********************************************************************
GH/29.12.95
CSEARCH main program for search

 Modified:

version 0.1
 GH/23.08.95
 GH/29.12.95 - include option d (initial displacement).
               print version number to log file
 LD/03.04.14 - added double quotes around pathnames to enable spaces
***********************************************************************/

/* Driver for routine AMOEBA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// cppcheck-suppress missingIncludeSystem
#include <limits.h>
// cppcheck-suppress missingIncludeSystem
#include <errno.h>
#include <math.h>
#include "search.h"
#include "search_optimizer.h"

/**********************************************************************/

static const char *sr_consume_arg(int argc, char *argv[], int *i_arg,
                                  const char *missing_msg)
{
  (*i_arg)++;
  if (*i_arg < argc) return argv[*i_arg];
#ifdef ERROR
  fprintf(STDERR,"%s", missing_msg);
#endif
  exit(1);
}

static int sr_parse_positive_int(const char *value, const char *invalid_msg)
{
  char *end = NULL;
  long parsed = strtol(value, &end, 10);
  if (end == value || parsed <= 0 || parsed > INT_MAX) {
#ifdef ERROR
    fprintf(STDERR,"%s", invalid_msg);
#endif
    exit(1);
  }
  return (int)parsed;
}

static uint64_t sr_parse_seed(const char *value, const char *invalid_msg)
{
  char *end = NULL;
  unsigned long long parsed;

  errno = 0;
  parsed = strtoull(value, &end, 10);
  if (end == value || errno == ERANGE) {
#ifdef ERROR
    fprintf(STDERR,"%s", invalid_msg);
#endif
    exit(1);
  }
  return (uint64_t)parsed;
}

int main(int argc, char *argv[])
{

  int i_arg;
  int i_par;
  int i_atoms;
  int ndim;
  const sr_optimizer_def *optimizer;
  sr_optimizer_config opt_cfg;

  real delta;

  char inp_file[STRSZ];
  char bak_file[STRSZ];
  char log_file[STRSZ];

  FILE *log_stream;

/*********************************************************************
  Preset program parameters and
  Decode arguments:

    -d <delta> - (optional) initial displacement for simplex search. Will
                be ignored with -v option.
    -i <inp_file> - (mandatory input file) overlayer parameters of all 
                    parameters.
    -v <bak_file> - (optional input file) vertex.

    -s <search_type> - (optional) default is "simplex"
*********************************************************************/

  sr_project = (char *) malloc(STRSZ * sizeof(char) );
  if (sr_project == NULL) {
    fprintf(STDERR, "*** error (SEARCH): allocation error (sr_project)\n");
    exit(1);
  }
  (void)snprintf(sr_project, STRSZ, "%s", "search");

  delta = DPOS;
  (void)snprintf(inp_file, sizeof(inp_file), "%s", "---");
  (void)snprintf(bak_file, sizeof(bak_file), "%s", "---");

  optimizer = sr_optimizer_by_type(SR_SIMPLEX);
  sr_optimizer_config_init(&opt_cfg);
  sr_optimizer_config_from_env(&opt_cfg);

  if (!argc) {search_usage(STDERR);exit(1);}
  
  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if (argv[i_arg][0] != '-')
    {
      #ifdef ERROR
      fprintf(STDERR,"*** error (SEARCH):\tsyntax error:\n");
      search_usage(STDERR);exit(1);
      #endif
      exit(1);
    }

    /* Read initial displacement */
    if (strncmp(argv[i_arg], "-d", 2) == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): initial displacement value not given\n");
      delta = (real)atof(value);
      continue;
    }

    /* Read parameter input file */
    if (strncmp(argv[i_arg], "-i", 2) == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): no input file specified\n");
      (void)snprintf(inp_file, sizeof(inp_file), "%s", value);
      continue;
    }

    /* Read vertex file */
    if (strncmp(argv[i_arg], "-v", 2) == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): no vertex file specified\n");
      (void)snprintf(bak_file, sizeof(bak_file), "%s", value);
      continue;
    }

    /* Read search type */
    if (strncmp(argv[i_arg], "-s", 2) == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): no search algorithm specified\n");
      optimizer = sr_optimizer_by_name(value);
      if (!optimizer) {
        #ifdef ERROR
        fprintf(STDERR,
           "*** error (SEARCH): unknown search type \"%s\" (option -s)\n",
           value);
        #endif
        exit(1);
      }
      continue;
    }

    if (strcmp(argv[i_arg], "--max-evals") == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): max evals value not given\n");
      opt_cfg.max_evals = sr_parse_positive_int(value,
          "*** error (SEARCH): invalid max evals value\n");
      continue;
    }

    if (strcmp(argv[i_arg], "--max-iters") == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): max iters value not given\n");
      opt_cfg.max_iters = sr_parse_positive_int(value,
          "*** error (SEARCH): invalid max iters value\n");
      continue;
    }

    if (strcmp(argv[i_arg], "--seed") == 0) {
      const char *value = sr_consume_arg(argc, argv, &i_arg,
          "*** error (SEARCH): seed value not given\n");
      opt_cfg.seed = sr_parse_seed(value,
          "*** error (SEARCH): invalid seed value\n");
      continue;
    }

    /* help */
    if ((strcmp(argv[i_arg], "-h") == 0) ||
        (strcmp(argv[i_arg], "--help") == 0)) {
      search_usage(STDOUT);
      exit(0);
    }

    /* version information */
    if ((strcmp(argv[i_arg], "-V") == 0) ||
        (strcmp(argv[i_arg], "--version") == 0)) {
      search_info();
      exit(0);
    }
  
  }  /* for i_arg */

/*********************************************************************
  Check arguments:
  - check existence of inp_file.
*********************************************************************/

  if(strncmp(inp_file, "---", 3) == 0)
  {
    #ifdef ERROR
    fprintf(STDERR,
      "*** error (SEARCH): no parameter input file (option -i) specified\n");
    #endif
    exit(1);
  }

/***********************************************************************
  Read input and assign external variables.
  Build name of log file.
***********************************************************************/

  fprintf(STDCTR,"(SEARCH): sr_project  = %s\n", sr_project);

  sr_rdinp(inp_file);

  fprintf(STDCTR,"(SEARCH): sr_project  = %s\n", sr_project);

  /* build log file */
  (void)snprintf(log_file, sizeof(log_file), "%s.log", sr_project);

  /* dimension of the search */
  ndim = sr_search->n_par;

  #ifdef CONTROL
  fprintf(STDCTR,"(SEARCH): project name  = %s\n", sr_project);
  fprintf(STDCTR,"(SEARCH): log file name = %s\n", log_file);
  fprintf(STDCTR,"(SEARCH): dimension = %d\n", ndim);
  #endif

/***********************************************************************
  Write header and geometrical details to log file
***********************************************************************/

  if( (log_stream = fopen(log_file, "w")) == NULL) { OPEN_ERROR(log_file); }

  fprintf(log_stream,"CSEARCH - version %s\n\n", SR_VERSION);
  fprintf(log_stream,"=> Atoms in search:\n\n");
  for(i_atoms = 0; (sr_atoms + i_atoms)->type != I_END_OF_LIST; i_atoms ++)
  {
    fprintf(log_stream,"%d \"%s\": (%6.3f, %6.3f, %6.3f) ref: %d nref: %d",
            i_atoms, 
            (sr_atoms + i_atoms)->name,
            (sr_atoms + i_atoms)->x, 
            (sr_atoms + i_atoms)->y, 
            (sr_atoms + i_atoms)->z,
            (sr_atoms + i_atoms)->ref, 
            (sr_atoms + i_atoms)->nref);
    fprintf(log_stream," r_min: %.3f\n", (sr_atoms + i_atoms)->r_min);

    if(!sr_search->z_only)
    {
      fprintf(log_stream,"x_par:\t");
      for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
      { 
        fprintf(log_stream,"%.3f ", 
                (sr_atoms+i_atoms)->x_par[i_par]); 
      }
      fprintf(log_stream,"\ny_par:\t");
      for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
      { 
        fprintf(log_stream,"%.3f ", 
               (sr_atoms+i_atoms)->y_par[i_par]); 
      }
      fprintf(log_stream,"\n");
    }
    fprintf(log_stream,"z_par:\t");
    for(i_par = 1; i_par <= sr_search->n_par; i_par ++)
    { 
      fprintf(log_stream,"%.3f ", 
              (sr_atoms+i_atoms)->z_par[i_par]); 
    }
    fprintf(log_stream,"\n\n");
  }

  fclose(log_stream);

  log_stream = fopen(log_file, "a");
  if (log_stream == NULL) { OPEN_ERROR(log_file); }
  fprintf(log_stream, "=> Optimizer: %s\n", optimizer ? optimizer->name : "simplex");
  sr_optimizer_log_config(log_stream, &opt_cfg);
  fclose(log_stream);

/***********************************************************************
  Perform the search according to the selected algorithm.
***********************************************************************/

  if (!optimizer) {
    optimizer = sr_optimizer_by_type(SR_SIMPLEX);
  }
  if (sr_optimizer_run(optimizer, &opt_cfg, ndim, delta, bak_file, log_file) != 0)
  {
    #ifdef ERROR
    fprintf(STDERR,
           "*** error (SEARCH): failed to run search type\n");
    #endif
    exit(1);
  }
  
  return 0;
  
}  /* end of main */

/**********************************************************************/
