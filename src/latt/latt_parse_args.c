
/*! \file
 *
 * Parses command line arguments to \c latt program.
 */

#include "latt.h"
#include "atom.h"
#include "lattice.h"
#include <stdio.h>
#include <strings.h>
#include <limits.h>

/*!
 * Processes the command line arguments to \c latt into #lattice \p latt .
 *
 * \param argc The number of arguments passed to the function.
 * \param argv Array of strings containing command line arguments to process.
 * \param latt Pointer to #lattice \c struct for processed arguments.
 */
void latt_parse_args(int argc, char *argv[], lattice *latt)
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
      fprintf(stderr, "SYNTAX ERROR:\n\n");
      latt_usage(stderr);
	  exit(INVALID_ARGUMENT_ERROR);
    }
    else
    {
      /* a lattice constant a */
      if ARG_ISH("-a", 2)
      {
        ARG_PARSE_FLOAT(latt->a_latt);
      }

      /* b lattice constant b */
      if ARG_ISH("-b", 2)
      {
        ARG_PARSE_FLOAT(latt->b_latt);
      }
   
      /* c: lattice constant c */
      if ARG_ISH("-c", 2)
      {
        ARG_PARSE_FLOAT(latt->c_latt);
      }

      /* d: maximum displacement in x & y */
      if (ARG_ISH("-d", 2) || ARG_IS("--max-displacement"))
      {
        ARG_PARSE_FLOAT(latt->max_disp_z);
      }

      /* nlayers */
      if ARG_IS("--max-layers")
	  {
        ARG_PARSE_UINT(latt->max_layers);
	  }

      /* ncells */
	  if ARG_IS("--max-cells")
	  {
        ARG_PARSE_UINT(latt->max_cells);
      }

      /* natoms */
	  if ARG_IS("--max-atoms")
	  {
        ARG_PARSE_UINT(latt->max_atoms);
	  }
	
      /* h */
      if ARG_IS("-h")
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
      if ARG_ISH("--help", 6)
      {
        latt_usage(stdout);
        exit(0);
      }
   
      /* i: open input file (read complicated basis) */
      if (ARG_ISH("-i", 2) || ARG_IS("--input"))
      {
        i_arg++;
     
        if (i_arg >= argc) 
        {
          fprintf(stderr, "*** error (latt_main): "
                  "missing input file argument for '-i'\n");
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
      if ARG_IS("-k")
      {
        ARG_PARSE_FLOAT(latt->vec_k);
      }

      /* l */
      if ARG_IS("-l")
      {
        ARG_PARSE_FLOAT(latt->vec_l);
      }

      /* n: name of atoms */
      if(ARG_IS("-n") || ARG_IS("--atom"))
      {
        i_arg++;
        if (i_arg < argc) 
        {
          strncpy(at_name, argv[i_arg], NAMSZ);
          latt->atoms->element = at_name;
        }
        else 
        {
          fprintf(stderr, "*** error (latt): "
                  "no atom name specified\n");
          exit(INVALID_ARGUMENT_ERROR);
        }
      }

      /* s  script index for line 2 of output file */
      if (ARG_IS("-s") || ARG_IS("--script"))
      {
        i_arg++;
        if (i_arg < argc) strcpy(latt->script, argv[i_arg]);
        i_arg++;
      }

      /* t  lattice type */
      if (ARG_ISH("-t", 2) || ARG_IS("--lattice"))
      {
        i_arg++;
        if      ARG_ISH("fc", 2) {latt->latt_type = LAT_FCC;}
        else if ARG_ISH("hc", 2) {latt->latt_type = LAT_HCP;}
        else if ARG_ISH("bc", 2) {latt->latt_type = LAT_BCC;}
        else if ARG_ISH("di", 2) {latt->latt_type = LAT_DIA;}
        else 
        {
          fprintf(stderr, "*** error: "
                  "%s is an invalid argument\n", argv[i_arg]);
        }
      }

      /* Print version */
      if (ARG_IS("--version") || ARG_IS("-V"))
      {
        latt_info();
        exit(0);
	  }
	
      /* Open output files (output and info) */
      if (ARG_ISH("-o", 2) || ARG_IS("--output"))
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
 
}
