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
#include <math.h>
#include "search.h"
#include "search_optimizer.h"

/**********************************************************************/

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
    if(*argv[i_arg] != '-')
    {
      #ifdef ERROR
      fprintf(STDERR,"*** error (SEARCH):\tsyntax error:\n");
      search_usage(STDERR);exit(1);
      #endif
      exit(1);
    }
    else
    {

      /* Read initial displacement */
      if(strncmp(argv[i_arg], "-d", 2) == 0)
      {
        i_arg++;
        if (i_arg < argc)
          delta = (real)atof(argv[i_arg]);
        else 
        {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): initial displacement value not given\n");
          #endif
          exit(1);
        }
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        i_arg++;
        if (i_arg < argc)
            (void)snprintf(inp_file, sizeof(inp_file), "%s", argv[i_arg]);
        else 
        {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): no input file specified\n");
          #endif
          exit(1);
        }
      }

      /* Read vertex file */
      if(strncmp(argv[i_arg], "-v", 2) == 0)
      {
        i_arg++;
        if (i_arg < argc)
            (void)snprintf(bak_file, sizeof(bak_file), "%s", argv[i_arg]);
        else
        {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): no vertex file specified\n");
          #endif
          exit(1);
        }
      }

      /* Read search type */
      if(strncmp(argv[i_arg], "-s", 2) == 0)
      {
        i_arg++;
        if (i_arg >= argc) 
        {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): no search algorithm specified\n");
          #endif
          exit(1);
        }
        optimizer = sr_optimizer_by_name(argv[i_arg]);
        if (!optimizer)
        {
          #ifdef ERROR
          fprintf(STDERR,
             "*** error (SEARCH): unknown search type \"%s\" (option -s)\n",
             argv[i_arg]);
          #endif
          exit(1);
        }
        
      } /* search type */

      if (strcmp(argv[i_arg], "--max-evals") == 0)
      {
        i_arg++;
        if (i_arg < argc) {
          opt_cfg.max_evals = atoi(argv[i_arg]);
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): max evals value not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--max-iters") == 0)
      {
        i_arg++;
        if (i_arg < argc) {
          opt_cfg.max_iters = atoi(argv[i_arg]);
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): max iters value not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--seed") == 0)
      {
        char *end = NULL;
        i_arg++;
        if (i_arg < argc) {
          opt_cfg.seed = strtoull(argv[i_arg], &end, 10);
          if (end == argv[i_arg]) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid seed value\n");
            #endif
            exit(1);
          }
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): seed value not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--pso-swarm") == 0)
      {
        char *end = NULL;
        long parsed = 0;
        i_arg++;
        if (i_arg < argc) {
          parsed = strtol(argv[i_arg], &end, 10);
          if (end == argv[i_arg] || parsed <= 0) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid PSO swarm size\n");
            #endif
            exit(1);
          }
          opt_cfg.pso_swarm_size = (int)parsed;
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): PSO swarm size not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--pso-inertia") == 0)
      {
        char *end = NULL;
        double parsed = 0.0;
        i_arg++;
        if (i_arg < argc) {
          parsed = strtod(argv[i_arg], &end);
          if (end == argv[i_arg] || parsed <= 0.0) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid PSO inertia\n");
            #endif
            exit(1);
          }
          opt_cfg.pso_inertia = (real)parsed;
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): PSO inertia not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--pso-c1") == 0)
      {
        char *end = NULL;
        double parsed = 0.0;
        i_arg++;
        if (i_arg < argc) {
          parsed = strtod(argv[i_arg], &end);
          if (end == argv[i_arg] || parsed <= 0.0) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid PSO c1\n");
            #endif
            exit(1);
          }
          opt_cfg.pso_c1 = (real)parsed;
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): PSO c1 not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--pso-c2") == 0)
      {
        char *end = NULL;
        double parsed = 0.0;
        i_arg++;
        if (i_arg < argc) {
          parsed = strtod(argv[i_arg], &end);
          if (end == argv[i_arg] || parsed <= 0.0) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid PSO c2\n");
            #endif
            exit(1);
          }
          opt_cfg.pso_c2 = (real)parsed;
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): PSO c2 not given\n");
          #endif
          exit(1);
        }
      }

      if (strcmp(argv[i_arg], "--pso-vmax") == 0)
      {
        char *end = NULL;
        double parsed = 0.0;
        i_arg++;
        if (i_arg < argc) {
          parsed = strtod(argv[i_arg], &end);
          if (end == argv[i_arg] || parsed <= 0.0) {
            #ifdef ERROR
            fprintf(STDERR,"*** error (SEARCH): invalid PSO vmax\n");
            #endif
            exit(1);
          }
          opt_cfg.pso_vmax = (real)parsed;
        } else {
          #ifdef ERROR
          fprintf(STDERR,"*** error (SEARCH): PSO vmax not given\n");
          #endif
          exit(1);
        }
      }
      
      /* help */
      if ((strcmp(argv[i_arg], "-h") == 0) || 
          (strcmp(argv[i_arg], "--help") == 0))
      {
        search_usage(STDOUT);
        exit(0);
      }
      
      /* version information */
      if ((strcmp(argv[i_arg], "-V") == 0) ||
           (strcmp(argv[i_arg], "--version") == 0))
      {
        search_info();
        exit(0);
      }

    } /* else */
  
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
  if (!optimizer || sr_optimizer_run(optimizer, &opt_cfg, ndim, delta, bak_file, log_file) != 0)
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
