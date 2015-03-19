/*********************************************************************
 *													leed_args.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/19.03.2015 - creation
 *********************************************************************/

/*!
 * \file leed_args.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 *
 * Implements functions for handling \c leed_args structs.
 */

#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leed_ver.h"   /* version information */
#include "leed_help.h"
#include "leed_def.h"
#include "leed.h"

/*! Checks for existence of par_file.
 *
 *  If bul_file is not specified, use par_file instead.
 */
int leed_check_input_files(const char *par_file, char *bul_file) {

  if(strncmp(par_file, "---", 3) == 0) {
    ERROR_MSG("no parameter input file (option -i) specified\n");
    exit(1);
  }

  if(strncmp(bul_file, "---", 3) == 0) {
    strncpy(bul_file, par_file, PATH_MAX);
  }

  return 0;
}

/*!
 * Checks for the existance of \p res_file .
 *
 * \param res_file Path to results file (e.g. 'leed.res')
 * \return File pointer to opened results file
 */
FILE *leed_check_result_file(const char* res_file) {
  FILE *res_stream = NULL;

  if(strncmp(res_file, "leed.res", 8) == 0) {
    WARNING_MSG("no output file (option -o) specified\n"
                "\toutput will be written to file \"%s\"\n", res_file);

    if ((res_stream = fopen(res_file, "w")) == NULL) {
      ERROR_MSG("could not open output file \"%s\"\n", res_file);
      exit(ENOFILE);
    }

  }

  return res_stream;
}

/*!
 * Inializes an instance of \c leed_args in memory
 *
 * \return Pointer to new \c leed_args
 */
leed_args *leed_args_init() {
  leed_args *args = (leed_args*) calloc(sizeof(leed_args), 1);

  if (args == NULL) {
    ERROR_MSG("could not allocate memory for leed_args\n");
    exit(ENOMEM);
  }

  /* explicit initialize args members */
  args->ctr_flag = CTR_NORMAL;
  strcpy(args->bul_file, "---");
  strcpy(args->par_file, "---");
  strcpy(args->res_file, "leed.res");
  strcpy(args->pro_name, "leed.pro");
  args->pro_stream = NULL;
  args->res_stream = NULL;

  FILE *pro_stream = NULL;
  FILE *res_stream = NULL;          /* results stream */

  return args;
}

void leed_args_free(leed_args *args)
{
  if (args != NULL) {
    free(args);
  }
  args = NULL;
}

leed_args *leed_args_parse(int argc, char *argv[])
{
  /* Decode arguments:
   *
   * -b <bul_file> - (optional input file) bulk and non-geometrical
   *                 parameters.
   * -i <par_file> - (mandatory input file) overlayer parameters of all
   *                 parameters (if bul_file does not exist).
   * -o <res_file> - (output file) IV output.
   */

  int i_arg;
  leed_args *args = leed_args_init();

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
      #ifdef ERROR_LOG
      leed_usage(stderr);
      #endif
      exit(1);
    }
    else
    {

      /* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
         strcmp(argv[i_arg], "--help") == 0)
      {
        leed_usage(stdout);
        exit(0);
      }

      /* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 ||
         strcmp(argv[i_arg], "--version") == 0)
      {
        leed_info();
        exit(0);
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-b", 2) == 0)
      {
        i_arg++;
        strncpy(args->bul_file, argv[i_arg], PATH_MAX);
      } /* -b */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        i_arg++;
        strncpy(args->par_file, argv[i_arg], PATH_MAX);
      } /* -i */

      /* Read and open results file */
      if(strncmp(argv[i_arg], "-o", 2) == 0)
      {
        i_arg++;
        strncpy(args->res_file, argv[i_arg], PATH_MAX);
        if ((args->res_stream = fopen(args->res_file, "w")) == NULL)
        {
          ERROR_MSG("could not open output file \"%s\"\n", args->res_file);
          exit(ENOFILE);
        }
      }  /* -o */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-e", 2) == 0)
      {
        args->ctr_flag = CTR_EARLY_RETURN;
      } /* -e */
      /* Read project name for reading matrices */
      if(strncmp(argv[i_arg], "-r", 2) == 0)
      {
        i_arg++;
        strncpy(args->pro_name, argv[i_arg], PATH_MAX);
        if ((args->pro_stream = fopen(args->pro_name, "r")) == NULL)
        {
          ERROR_MSG("can not open project file \"%s\" for reading\n",
                    args->pro_name);
          exit(ENOFILE);
        }
      } /* -r */

      /* Read project name for writing matrices */
      if(strncmp(argv[i_arg], "-w", 2) == 0)
      {
        i_arg++;
        strncpy(args->pro_name, argv[i_arg], PATH_MAX);
        if ((args->pro_stream = fopen(args->pro_name,"w")) == NULL)
        {
          ERROR_MSG("could not open project file \"%s\" for writing\n",
                    args->pro_name);
          exit(ENOFILE);
        }
      } /* -w */

    }  /* else */
  }  /* for i_arg */

  /* perform sanity checks */
  leed_check_input_files(args->par_file, args->bul_file);
  args->res_stream = leed_check_result_file(args->res_file);

  return args;
}
