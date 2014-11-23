/************************************************************************
 *                            CSEARCH.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * version 0.1
 * GH/23.08.95
 * GH/29.12.95 - include option d (initial displacement).
 *              print version number to log file
 * LD/03.04.14 - added double quotes around pathnames to enable spaces
 ***********************************************************************/

/*!
 * \file csearch.c
 * \brief Driver for routine AMOEBA
 * \author Georg Held <g.held@reading.ac.uk>
 *
 * CSEARCH main() program for search.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include "csearch.h"

void search_parse_args(size_t argc, char *argv[], real *delta,
                       search_method *search_type, char *inp_file,
                       char *bak_file)
{

  size_t i_arg;

  /*********************************************************************
   * Preset program parameters and
   * Decode arguments:
   *
   *  -d <delta> - (optional) initial displacement for simplex search.
   *               Will be ignored with -v option.
   *  -i <inp_file> - (mandatory input file) overlayer parameters of all
   *                  parameters.
   *  -v <bak_file> - (optional input file) vertex.
   *
   *  -s <search_type> - (optional) default is "simplex"
   *********************************************************************/

    sr_project = (char *) malloc(FILENAME_MAX * sizeof(char) );
    strncpy(sr_project, "search", FILENAME_MAX);

    /* exit if no arguments */
    if (!argc)
    {
      search_usage(STDERR);
      exit(SR_FAILURE);
    }

    for (i_arg = 1; i_arg < argc; i_arg++)
    {
      if(*argv[i_arg] != '-')
      {
        #ifdef ERROR
        fprintf(STDERR, "*** error (SEARCH):\tsyntax error:\n");
        search_usage(STDERR);
        #endif
        exit(SR_FAILURE);
      }
      else
      {

        /* Read initial displacement */
        if(strncmp(argv[i_arg], "-d", 2) == 0)
        {
          i_arg++;
          if (i_arg < argc)
          {
            *delta = (real)atof(argv[i_arg]);
          }
          else
          {
            #ifdef ERROR
            fprintf(STDERR, "*** error (SEARCH): "
                   "initial displacement value not given\n");
            #endif
            exit(SR_MISSING_ARGUMENT);
          }
        }

        /* Read parameter input file */
        if(strncmp(argv[i_arg], "-i", 2) == 0)
        {
          i_arg++;
          if (i_arg < argc)
          {
            strncpy(inp_file, argv[i_arg], FILENAME_MAX);
          }
          else
          {
            #ifdef ERROR
            fprintf(STDERR, "*** error (SEARCH): no input file specified\n");
            #endif
            exit(SR_INVALID_INPUT_FILE);
          }
        }

        /* Read vertex file */
        if(strncmp(argv[i_arg], "-v", 2) == 0)
        {
          i_arg++;
          if (i_arg < argc)
          {
            strncpy(bak_file, argv[i_arg], FILENAME_MAX);
          }
          else
          {
            #ifdef ERROR
            fprintf(STDERR, "*** error (SEARCH): no vertex file specified\n");
            #endif
            exit(SR_INVALID_VERTEX_FILE);
          }
        }

        /* Read search type */
        if(strncmp(argv[i_arg], "-s", 2) == 0)
        {
          i_arg++;
          if (i_arg >= argc)
          {
            #ifdef ERROR
            fprintf(STDERR, "*** error (SEARCH): no search algorithm specified\n");
            #endif
            exit(SR_INVALID_SEARCH_TYPE);
          }
          if((strncmp(argv[i_arg], "si", 2) == 0) ||
             (strncmp(argv[i_arg], "sx", 2) == 0))
          {
            *search_type = SR_SIMPLEX;
          }
          else if(strncmp(argv[i_arg], "po", 2) == 0)
          {
            *search_type = SR_POWELL;
          }
          else if(strncmp(argv[i_arg], "sa", 2) == 0)
          {
            *search_type = SR_SIM_ANNEALING;
          }
          else if(strncmp(argv[i_arg], "ga", 2) == 0)
          {
            *search_type = SR_GENETIC;
          }
          else
          {
            #ifdef ERROR
            fprintf(STDERR, "*** error (SEARCH): "
                "unknown search type \"%s\" (option -s)\n", argv[i_arg]);
            #endif
            exit(SR_INVALID_SEARCH_TYPE);
          }

        } /* search type */

        /* help */
        if ((strcmp(argv[i_arg], "-h") == 0) ||
            (strcmp(argv[i_arg], "--help") == 0))
        {
          search_usage(STDOUT);
          exit(SR_SUCCESS);
        }

        /* version information */
        if ((strcmp(argv[i_arg], "-V") == 0) ||
             (strcmp(argv[i_arg], "--version") == 0))
        {
          search_info();
          exit(SR_SUCCESS);
        }

      } /* else */

    }  /* for i_arg */

  /*********************************************************************
   * Check arguments:
   * - check existence of inp_file.
   *********************************************************************/

    if(strncmp(inp_file, "---", 3) == 0)
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (SEARCH): "
             "no parameter input file (option -i) specified\n");
      #endif
      exit(SR_INVALID_INPUT_FILE);
    }

}

int main(int argc, char *argv[])
{
  size_t n_dim;

  search_method search_type = SR_SIMPLEX;
  real delta = DPOS;
  char inp_file[FILENAME_MAX] = "---";
  char bak_file[FILENAME_MAX] = "---";
  char log_file[FILENAME_MAX] = "---";

  search_parse_args(argc, argv, &delta, &search_type, inp_file, bak_file);

/***********************************************************************
 * Read input and assign external variables.
 * Build name of log file.
 ***********************************************************************/

  fprintf(STDCTR, "(SEARCH): sr_project  = %s\n", sr_project);

  sr_rdinp(inp_file);

  fprintf(STDCTR, "(SEARCH): sr_project  = %s\n", sr_project);

  /* build log file */
  sprintf(log_file, "%s.log", sr_project);

  /* dimension of the search */
  n_dim = sr_search->n_par;

  #ifdef CONTROL
  fprintf(STDCTR, "(SEARCH): project name  = %s\n", sr_project);
  fprintf(STDCTR, "(SEARCH): log file name = %s\n", log_file);
  fprintf(STDCTR, "(SEARCH): dimension = %d\n", n_dim);
  #endif

/***********************************************************************
 * Write header and geometrical details to log file
 ***********************************************************************/

  sr_mklog(log_file);

/***********************************************************************
 * Perform the search according to the selected algorithm.
 ***********************************************************************/

  switch(search_type)
  {
    /* SIMPLEX METHOD */
    case(SR_SIMPLEX):
    {
      SR_SX(n_dim, delta, bak_file, log_file);
      break;
    } /* case SR_SIMPLEX */

    /* POWELL'S METHOD */
    case(SR_POWELL):
    {
      #if defined(USE_GSL) || defined(USE_GSL)
         SR_NOT_IMPLEMENTED_ERROR("Powell's method");
      #else
         SR_PO(n_dim, bak_file, log_file);
      #endif
      break;
    } /* case SR_POWELL */

    /* SIMULATED ANNEALING */
    case(SR_SIM_ANNEALING):
    {
      #if defined(USE_GSL) || defined(USE_GSL)
         SR_NOT_IMPLEMENTED_ERROR("simulated annealing");
      #else
         SR_SA(n_dim, delta, bak_file, log_file);
      #endif
      break;
    } /* case SR_SIM_ANNEALING */

    /* GENETIC ALGORITHM */
    case(SR_GENETIC):
    {
      SR_NOT_IMPLEMENTED_ERROR("genetic algorithm");
      break;
    } /* case SR_GENETIC */
    
    default:
    {
      #ifdef ERROR
      fprintf(STDERR, "*** error (SEARCH): "
              "unknown search type \"%d\"\n", search_type);
      #endif
      exit(SR_INVALID_SEARCH_TYPE);
    }
    
  }  /* switch */
  
  return(SR_SUCCESS);
}  /* end of main */
