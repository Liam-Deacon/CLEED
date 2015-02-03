/************************************************************************
 *                            LATT_PARSE_ARGS.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 ***********************************************************************/

/*! \file
 *
 * Parses command line arguments to \c latt program.
 */

#include <stdio.h>
#include <strings.h>
#include <limits.h>

#include "latt.h"
#include "atom.h"
#include "lattice.h"
#include "gh_stddef.h"

/*!
 * Processes the command line arguments to \c latt into #lattice \p latt .
 *
 * \param argc The number of arguments passed to the function.
 * \param argv Array of strings containing command line arguments to process.
 * \param latt Pointer to #lattice \c struct for processed arguments.
 */
int latt_parse_args(int argc, char *argv[], lattice *latt)
{
  int i_arg; 
  atom *atom_list = (atom*) malloc(sizeof(atom));
  char *at_name = (char*) malloc(sizeof(char) * NAMSZ);
  
  if (latt == NULL) latt = lattice_init(1);
  
  /* Carbon is default element */
  strcpy(at_name, "C");
  latt->atoms = atom_list;
  latt->atoms[0].element = at_name;
  latt->n_atoms = 1;
  latt->allocated_atoms = 1;
  
  #if (DEBUG == 1)
  fprintf(stderr, "** debug (latt_parse_args):\n");
  fprintf(stderr, "%s should be %s\n", (latt->atoms[0]).element, at_name);
  #endif
  
  if (argc == 1) /* no arguments given */
  {
    /* read from stdin */
    latt->latt_type = LAT_INP;
    strcpy(latt->input_filename, "stdin");
  }

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    #if (DEBUG == 1)
    fprintf(stderr, "#%2i: '%s' -> '%s'\n", i_arg, argv[i_arg], argv[i_arg+1]);
    #endif
    
    if(*argv[i_arg] != '-')
    {
      fprintf(stderr, "SYNTAX ERROR_LOG:\n\n");
      latt_usage(stderr);
      exit(INVALID_ARGUMENT_ERROR);
    }
    else
    {
      /* a lattice constant a */
      if ( !strncmp(argv[i_arg], "-a", 2) )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->a_latt = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* b lattice constant b */
      if ( !strncmp(argv[i_arg], "-b", 2) )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->b_latt = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }
   
      /* c: lattice constant c */
      if ( !strncmp(argv[i_arg], "-c", 2) )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->c_latt = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* d: maximum displacement in x & y */
      if ( ( !strncmp(argv[i_arg], "-d", 2) ) ||
           ( !strcmp(argv[i_arg], "--max-displacement") ) )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->max_disp_z = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* nlayers */
      if ( !strcmp(argv[i_arg], "--max-layers") )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->max_layers = (size_t)atoi(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* ncells */
      if ( !strcmp(argv[i_arg], "--max-cells") )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->max_cells = (size_t)atoi(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* natoms */
      if ( !strcmp(argv[i_arg], "--max-atoms") )
	    {
        if (i_arg < argc)
        {
          i_arg++;
          latt->max_atoms = (size_t)atoi(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
	    }
	
      /* h */
      if ( !strcmp(argv[i_arg], "-h") )
      {
        i_arg++;
      
        if (i_arg < argc)
        {
          latt->vec_h = (float)atof(argv[i_arg]);
        }
        else
        {
          latt_usage(stdout);
          exit(0);
        }
      }

      /* help */
      if ( !strncmp(argv[i_arg], "--help", 6) )
      {
        latt_usage(stdout);
        exit(0);
      }
   
      /* i: open input file (read complicated basis) */
      if (( !strncmp(argv[i_arg], "-i", 2)) || (!strcmp(argv[i_arg], "--input")) )
      {
        i_arg++;
     
        if (i_arg >= argc) 
        {
          ERROR_MSG("missing input file argument for '-i'\n");
          exit(INVALID_ARGUMENT_ERROR);
        }
     
        if (latt->input_filename == NULL)
        {
          latt->input_filename = (char*) malloc(sizeof(char) * FILENAME_MAX);
        }
     
        strncpy(latt->input_filename, argv[i_arg], FILENAME_MAX);
        strcat(latt->input_filename, "\0");
        latt->latt_type = LAT_INP;
      }

      /* k */
      if ( !strcmp(argv[i_arg], "-k") )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->vec_k = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* l */
      if ( !strcmp(argv[i_arg], "-l") )
      {
        if (i_arg < argc)
        {
          i_arg++;
          latt->vec_l = (float)atof(argv[i_arg]);
        }
        else
        {
          ERROR_MSG("missing value for option '%s'\n", argv[i_arg]);
          ERROR_RETURN(INVALID_ARGUMENT_ERROR);
        }
      }

      /* n: name of atoms */
      if(!strcmp(argv[i_arg], "-n") || !strcmp(argv[i_arg], "--atom"))
      {
        i_arg++;
        if (i_arg < argc) 
        {
          if (latt->atoms[0].element == NULL)
            latt->atoms[0].element = (char*) calloc(NAMSZ, sizeof(char));
          strncpy(latt->atoms[0].element, argv[i_arg], NAMSZ);
        }
        else 
        {
          ERROR_MSG("no atom name specified\n");
          exit(INVALID_ARGUMENT_ERROR);
        }
      }

      /* s  script index for line 2 of output file */
      if (!strcmp(argv[i_arg], "-s") || !strcmp(argv[i_arg], "--script"))
      {
        i_arg++;
        if (i_arg < argc) strcpy(latt->script, argv[i_arg]);
        i_arg++;
      }

      /* t  lattice type */
      if ( (!strncmp(argv[i_arg], "-t", 2) ) || (!strcmp(argv[i_arg], "--lattice")) )
      {
        i_arg++;
        if      ( !strncmp(argv[i_arg], "fc", 2) ) {latt->latt_type = LAT_FCC;}
        else if ( !strncmp(argv[i_arg], "hc", 2) ) {latt->latt_type = LAT_HCP;}
        else if ( !strncmp(argv[i_arg], "bc", 2) ) {latt->latt_type = LAT_BCC;}
        else if ( !strncmp(argv[i_arg], "di", 2) ) {latt->latt_type = LAT_DIA;}
        else 
        {
          ERROR_MSG("%s is an invalid argument\n", argv[i_arg]);
        }
      }

      /* Print version */
      if (!strcmp(argv[i_arg], "--version") || !strcmp(argv[i_arg], "-V"))
      {
        latt_info();
        exit(0);
	  }
	
      /* Open output files (output and info) */
      if ( (!strncmp(argv[i_arg], "-o", 2)) || !strcmp(argv[i_arg], "--output"))
      {
        i_arg++;
        if (i_arg >= argc) break;
        if (latt->output_filename == NULL)
        {
          latt->output_filename = (char*) malloc(sizeof(char) * FILENAME_MAX);
        }
        strcpy(latt->output_filename, argv[i_arg]);
        strcat(latt->output_filename, "\0");
      }
   
    } /* else */
    
  }  /* for i_arg */
  
  #if (DEBUG == 1)
  lattice_debug(latt);
  fprintf(stderr, "** debug (latt_parse_args): finished parsing args.\n");
  #endif

  return(0);
}
