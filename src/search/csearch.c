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
#include <strings.h>
#include <string.h>
#include <math.h>
#include "search.h"

/**********************************************************************/

int main(int argc, char *argv[])
{

  int i_arg;
  int i_par;
  int i_atoms;
  int ndim;
  int search_type;

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
  strncpy(sr_project, "search", STRSZ);

  delta = DPOS;
  strncpy(inp_file,"---", STRSZ);
  strncpy(bak_file,"---", STRSZ);

  search_type = SR_SIMPLEX;

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
            strncpy(inp_file, argv[i_arg], STRSZ);
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
            strncpy(bak_file, argv[i_arg], STRSZ);
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
        if((strncmp(argv[i_arg], "si", 2) == 0) || 
           (strncmp(argv[i_arg], "sx", 2) == 0))
          search_type = SR_SIMPLEX;
        else if(strncmp(argv[i_arg], "po", 2) == 0)
          search_type = SR_POWELL;
        else if(strncmp(argv[i_arg], "sa", 2) == 0)
          search_type = SR_SIM_ANNEALING;
        else if(strncmp(argv[i_arg], "ga", 2) == 0)
          search_type = SR_GENETIC;
        else
        {
          #ifdef ERROR
          fprintf(STDERR,
             "*** error (SEARCH): unknown search type \"%s\" (option -s)\n", 
             argv[i_arg]);
          #endif
          exit(1);
        }
        
      } /* search type */
      
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
  sprintf(log_file,"%s.log",sr_project);

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

/***********************************************************************
  Perform the search according to the selected algorithm.
***********************************************************************/

  switch(search_type)
  {
/*
  SIMPLEX METHOD
*/
    case(SR_SIMPLEX):
    {
      SR_SX(ndim, delta, bak_file, log_file);
      break;
    } /* case SR_SIMPLEX */

/*
  POWELL'S METHOD
*/
    case(SR_POWELL):
    {
      #if defined(_USE_GSL) || defined(USE_GSL)
         SR_NOT_IMPLEMENTED_ERROR("Powell's method");
      #else
         SR_PO(ndim, bak_file, log_file);
      #endif
      break;
    } /* case SR_POWELL */

/*
  SIMULATED ANNEALING
*/
    case(SR_SIM_ANNEALING):
    {
      #if defined(_USE_GSL) || defined(USE_GSL)
         SR_NOT_IMPLEMENTED_ERROR("simulated annealing");
      #else
         SR_SA(ndim, delta, bak_file, log_file);
      #endif
      break;
    } /* case SR_SIM_ANNEALING */

/*
  GENETIC ALGORITHM
*/
    case(SR_GENETIC):
    {
      SR_NOT_IMPLEMENTED_ERROR("genetic algorithm");
      break;
    } /* case SR_GENETIC */
    
    default:
    {
      #ifdef ERROR
      fprintf(STDERR,
             "*** error (SEARCH): unknown search type \"%d\"\n", search_type);
      #endif
      exit(1);
    }
    
  }  /* switch */
  
  return 0;
  
}  /* end of main */

/**********************************************************************/
