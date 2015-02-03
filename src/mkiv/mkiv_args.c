/*
 * mkiv_args.c
 *
 *  Created on: 27 Jan 2015
 *      Author: Liam
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mkiv.h"

mkiv_args *mkiv_args_init() {
  mkiv_args *args = (mkiv_args*) calloc(1, sizeof(args));

  if (args == NULL)
  {
    ERROR_MSG("Could not allocate memory\n");
    ERROR_RETURN(NULL);
  }

  /* set initial values */
  args->verbose = false;
  args->save_intermediates = false;
  args->make_mask = false;
  strcpy(args->beam_raw_path, "beam.raw");
  strcpy(args->beam_smoothed_path, "beam.smo");
  strcpy(args->inp_path, "mkiv.inp");
  strcpy(args->ivdat_path, "mkiv.ivdat");
  strcpy(args->mask_path, "mask.byte");
  strcpy(args->param_path, "mkiv.param");
  strcpy(args->pos_path, "mkiv.pos");
  strcpy(args->var_path, "mkiv.var");

  return (args);
}

mkiv_args *mkiv_get_args(int argc, char *argv[]) {

  int i;
  mkiv_args *args = NULL;

  char dummy[FILENAME_MAX];

  if ( (args = mkiv_args_init()) == NULL)
  {
    ERROR_RETURN(NULL);
  }

  /* decode arguments */
  if (argc <= 1)
  {
    ERROR_MSG("wrong arguments - see 'mkiv -h' for usage.");
    ERROR_RETURN(-1);
  }
  for (i=1; i<=argc; i++)
  {
    while (*++*argv != '\0' || i < argc)
    {
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
      {
        mkiv_usage(stdout);
        exit(0);
      }
      else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--input"))
      {
        if (i+1 < argc)
        {
          strcpy(dummy, argv[i+1]);
          if (file_exists(dummy))
          {
            WARNING_MSG("'%s' is a filename not a prefix\n"
                        "\tattempting to find files...", dummy);
            strcpy( dummy, remove_ext(argv[i+1], '.', '/') );
          }

          /* populate file names */
          strcpy(args->inp_path, dummy);
          strcpy(args->pos_path, dummy);
          strcpy(args->var_path, dummy);
          strcat(args->inp_path, ".inp");
          strcat(args->pos_path, ".pos");
          strcat(args->var_path, ".var");

          /* check files exist */
          if (!file_exists(args->inp_path))
          {
            ERROR_MSG("'%s' does not exist!\n", args->inp_path);
            ERROR_RETURN(NULL);
          }
          if (!file_exists(args->pos_path))
          {
            ERROR_MSG("'%s' does not exist!\n", args->pos_path);
            ERROR_RETURN(NULL);
          }
          if (!file_exists(args->var_path))
          {
            ERROR_MSG("'%s' does not exist!\n", args->var_path);
            ERROR_RETURN(NULL);
          }
        }
      }
      else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--save-images"))
      {
        /* save processed images ('ima.byte' */
        args->save_intermediates = 1;
      }
      else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--mask"))
      {
        if (i+1 < argc)
        {
          i++;
          strcpy(args->mask_path, argv[i]);
        }
        else
        {
          ERROR_MSG("no argument to %s\n", argv[i]);
          ERROR_RETURN(NULL);
        }
      }
      else if (!strcmp(argv[i], "-M") || !strcmp(argv[i], "--make-mask"))
      {
        args->make_mask = 1;
      }
      else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output"))
      {
        if (i+1 < argc)
        {
          i++;
          strcpy(args->ivdat_path, argv[i]);
        }
        else
        {
          ERROR_MSG("no argument to %s\n", argv[i]);
          ERROR_RETURN(NULL);
        }
      }
      else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--param"))
      {
        if (i+1 < argc)
        {
          i++;
          strcpy(args->param_path, argv[i]);
        }
        else
        {
          ERROR_MSG("no argument to %s\n", argv[i]);
          ERROR_RETURN(NULL);
        }
      }
      else if (!strcmp(argv[i], "-P") || !strcmp(argv[i], "--pos"))
      {
        if (i+1 < argc)
        {
          i++;
          strcpy(args->pos_path, argv[i]);
        }
        else
        {
          ERROR_MSG("no argument to %s\n", argv[i]);
          ERROR_RETURN(NULL);
        }
      }
      else if (!strcmp(argv[i], "-V") || !strcmp(argv[i], "--version"))
      {
        mkiv_info();
        exit(0);
      }
      else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose"))
      {
        args->verbose |= VERBOSE;
      }
      else if (!strcmp(argv[i], "-V") || !strcmp(argv[i], "--version"))
      {
        args->verbose |= QUICK;
      }
      else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--current"))
      {
        if ( --argc && isdigit(**++argv) ) args->beam_current = atoi(*argv);
        else
        {
          ERROR_MSG("wrong arguments for beam current.\n");
          ERROR_RETURN(NULL);
        }
      }
      else if (!strcmp(argv[i], "-q") ||
               !strcmp(argv[i], "--quiet") ||
               !strcmp(argv[i], "--quick"))
      {
        args->verbose |= QUICK;
      }
      else
      {
        ERROR_MSG("wrong arguments - see 'mkiv -h' for usage.\n");
        ERROR_RETURN(NULL);
      }
    }
  }

  return(args);
}

inline void mkiv_args_free(mkiv_args *args)
{
  if (args != NULL) free(args);
}
