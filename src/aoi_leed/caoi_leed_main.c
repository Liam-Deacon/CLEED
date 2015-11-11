/************************************************************************
 *                        CAOI_LEED_MAIN.C
 *
 *  Copyright 2008-2014 Zhasmina Zheleeva
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  file contains functions
 *   main()
 *     Main program for CAOI_LEED program
 *     (calls cleed program for each angle of incidence)
 *
 * Changes:
 *   ZZ/2008.03.03 - creation
 *   LD/2014.10.09 - changed filename length to FILENAME_MAX
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "caoi_leed.h"

extern void caoi_leed_info();
extern void caoi_leed_usage(FILE *);

int main (int argc, char *argv[])
{
  int i_arg = 0;
  size_t length = 0;
  size_t i_ang = 0;

  char bsr_file[FILENAME_MAX] = "";
  char par_file[FILENAME_MAX] = "";
  char res_file[FILENAME_MAX] = "leed.res";

  char linebuffer[STRSZ] = "";
  char helpstring[STRSZ] = "";
  char line_buffer[STRSZ] = "";

  FILE *inp_stream = NULL;
  FILE *write_stream = NULL;
  FILE *read_stream = NULL;

  bool sa_parsed = false;

  /* Preset program parameters */
  sa = 999;

  /* Decode arguments:
   * -b project name for modified bulk file
   * -i parameter file for overlayer
   * -o project name for output file
   */
  if (argc < 2)
  {
    caoi_leed_usage(STDERR);
    exit(EINVAL);
  }

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
      ERROR_MSG("\tsyntax error:\n%s",
          "\tusage: \n\tcaoi_leed -b <bsr_file>"
    			"-i <par_file> -v <vertex_file> -o <res_file>\n");
      exit(EINVAL);
    }
    else
    {

      /* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
         strcmp(argv[i_arg], "--help") == 0) 
      {
        caoi_leed_usage(stdout);
        exit(0);
      }  

      /* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 || 
         strcmp(argv[i_arg], "--version") == 0)
      {
        caoi_leed_info();
        exit(0);
      }
      
      /* Read modified bulk file */
      if(strncmp(argv[i_arg], "-b", 2) == 0)
      {
        if (i_arg+1 < argc)
        {
          i_arg++;
          strncpy(bsr_file, argv[i_arg], FILENAME_MAX);
          bsr_file[FILENAME_MAX] = '\0';
        }
        else
        {
          ERROR_MSG("no filename given for option '-b'\n");
          exit(EINVAL);
        }
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        if (i_arg+1 < argc)
        {
          i_arg++;
          strncpy(par_file, argv[i_arg], FILENAME_MAX);
          par_file[FILENAME_MAX] = '\0';
        }
        else {
          ERROR_MSG("no filename given for option '-i'\n");
          exit(EINVAL);
        }
      }

      /* Read vertex file */
      if(strncmp(argv[i_arg], "-o", 2) == 0)
      {
        if (i_arg+1 < argc)
        {
          i_arg++;
          strncpy(res_file, argv[i_arg], FILENAME_MAX);
          res_file[FILENAME_MAX] = '\0';
        }
        else
        {
          ERROR_MSG("no filename given for option '-b'\n");
          exit(EINVAL);
        }
      }

    } /* else */
  }  /* for i_arg */

  /* Check arguments:
   * - check existence of par_file.
   * - if bsr_file is not specified use par_file instead.
   */
  if(strlen(par_file) == 0)
  {
    ERROR_MSG("no parameter file (option -i) specified\n");
    exit(EINVAL);
  }

  if(strlen(bsr_file) == 0)
  {
    strncpy(bsr_file, par_file, FILENAME_MAX);
  }

  /* Initialize "sa" with the number of angles of incidence */
  length = strlen(par_file) - 4;
  strncpy(linebuffer, par_file, length);
  sprintf(linebuffer+length, ".bsr");

  if ((inp_stream = fopen(linebuffer, "r")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", linebuffer);
    exit(EIO);
  }

  while (fgets(linebuffer, STRSZ, inp_stream) != NULL)
  {
    if (!strncasecmp(linebuffer, "sa:", 3))
    {
      sa_parsed = (sscanf(linebuffer+3, "%u", &sa) < 1) || sa_parsed;
    }
  }

  if (!sa_parsed)
  {
    ERROR_MSG("could not read sa: from the .bsr file\n");
    exit(EIO);
  }

  fclose(inp_stream);

/*****************************************************************
 Makes new .bsr file for each angle of incidence
  - calls the function bulinp()
******************************************************************/

  bsrinp(bsr_file, sa);

  CONTROL_MSG(CONTROL, "sa = %d ***\n", sa);

  /* Call cleed_nsym program for each angle of incidence */
  for (i_ang = 0; i_ang < sa; i_ang ++)
  {

    /* append "ia_<i_ang>" to par_file */
    strncpy(linebuffer, par_file, length);
    sprintf(linebuffer + length, "ia_%u", i_ang + 1);

    /* execute cleed program held in CAOI_LEED environment variable */
    sprintf(helpstring,            /* note the quotations in next line */
         "\"%s\" -b \"%s.bsr\" -i \"%s\" -o \"%s.res\" > \"%s.out\"",
         getenv("CAOI_LEED"),      /* environment variable for cleed program */
         linebuffer,               /* project name for modified bulk file */
         par_file,                 /* parameter file for overlayer */
         linebuffer,               /* project name for results file */
         linebuffer                /* project name for output file */    
       );

    /* get result from system call */
    if (system(helpstring) != 0)
    { 
      ERROR_MSG("system call: %s failed\n", helpstring);
    }

  } /*for i_ang*/

  /* Make proj_name.res = proj_nameia_1.res + proj_nameia_2.res + ... */
  strncpy(linebuffer, par_file, length);
  sprintf(linebuffer + length, ".res");

  if ((write_stream = fopen(linebuffer,"w")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", linebuffer);
    exit(EIO);
  }

  for (i_ang = 0; i_ang < sa; i_ang ++)
  {
    strncpy(helpstring, par_file, length);
    sprintf(helpstring + length, "ia_%u.res", i_ang + 1);
    if ((read_stream = fopen(helpstring,"r")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", helpstring);
      exit(EIO);
    }

    fprintf(write_stream, "Copy of %s\n", helpstring);

    while (fgets(line_buffer, STRSZ, read_stream))
    {
      fprintf(write_stream, "%s", line_buffer);
    }

    fclose(read_stream);
    
  } /* for i_ang */

  fclose(write_stream);
  fflush(STDERR);

  return(0);

} /* main */
