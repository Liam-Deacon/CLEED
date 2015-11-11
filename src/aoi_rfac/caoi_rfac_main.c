/*********************************************************************
ZZ/03.03.08
  file contains functions:

  main (03.03.08)
     Main program for CAOI_RFAC_MAIN program 
     (calls crfac program for each angle of incidence)

Changes:

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "caoi_rfac.h"
#include "gh_stddef.h"

extern void caoi_rfac_info();
extern void caoi_rfac_usage(FILE *);

int main(int argc, char *argv[])
{
  size_t i;
  int i_arg;

  size_t length = 0;
  int iaux = 0;

  FILE *fp = NULL;
  FILE *fpp = NULL;

  char linebuffer[STRSZ] = "";
  char fname_string[FILENAME_MAX] = "";
  char res_file[FILENAME_MAX] = "";                 /* input/output files */
  char ctr_file[FILENAME_MAX] = "";
  char rfac_type[STRSZ] = "rp";

  float m_rfac_shift_range = -10.;
  float p_rfac_shift_range = 10.;
  float rfac_shift_step = 0.5;

  float rfac;
  float faux, shift;
  float enrange;
  float averfac;
  float averenr;
  float avershift;
  float sum;
  float sterror;
  float sum1;

  /* Decode arguments:
   *
   * -c <ctr_file> - control file
   * -t <res_file> - (output file) IV output.
   */
  if (argc < 2) 
  {
#if ERROR_LOG
	  	ERROR_MSG("syntax error\n");
	  	caoi_rfac_usage(STDERR);
#endif
      exit(EINVAL);
  }
  
  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
#if ERROR_LOG
      ERROR_MSG("syntax error:\n");
    	caoi_rfac_usage(STDERR);
#endif
      exit(EINVAL);
    }
    else
    {
      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-t", 2) == 0 ||
         strcmp(argv[i_arg], "--theory") == 0 )
      {
        if (i_arg+1 >= argc)
        {
          ERROR_MSG("no theory filename given\n");
          exit(EINVAL);
        }
        i_arg++;
        strncpy(res_file, argv[i_arg], FILENAME_MAX);
      } /* -t */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-c", 2) == 0 ||
         strcmp(argv[i_arg], "--control") == 0 )
      {
        if (i_arg+1 >= argc)
        {
          ERROR_MSG("no control filename given\n");
          exit(EINVAL);
        }
        i_arg++;
        strncpy(ctr_file, argv[i_arg], FILENAME_MAX);
      } /* -i */

      /* Read R factor type*/
      if(strncmp(argv[i_arg], "-r", 2) == 0 ||
         strcmp(argv[i_arg], "--rfactor") == 0 )
      {
        if (i_arg+1 >= argc)
        {
          ERROR_MSG("no rfactor argument given\n");
          exit(EINVAL);
        }
        i_arg++;
        strncpy(rfac_type, argv[i_arg], STRSZ);
      } /* -r */

      /* Read initial shift, final shift, step of a shift*/
      if(strncmp(argv[i_arg], "-s", 2) == 0 ||
         strcmp(argv[i_arg], "--shift") == 0 )
      {
        if (i_arg+1 >= argc)
        {
          ERROR_MSG("no argument given\n");
          exit(EINVAL);
        }
        i_arg++;
        if (sscanf(argv[i_arg], "%f,%f,%f", &m_rfac_shift_range,
        		   &p_rfac_shift_range, &rfac_shift_step) < 3)
        {
          WARNING_MSG("not enough arguments given for -s option"
                      "(using defaults)\n");
        }
	
      } /* -s */
      
      /* help */
      if ((strcmp(argv[i_arg], "-h") == 0) || 
          (strcmp(argv[i_arg], "--help") == 0))
      {
        caoi_rfac_usage(stdout);
        exit(0);
      }
      
      /* version info */
      if ((strcmp(argv[i_arg], "-V") == 0) ||
           (strcmp(argv[i_arg], "--version") == 0))
      {
        caoi_rfac_info();
        exit(0);
      }

    }  /* else */
    
  }  /* for i_arg */

  /*
   * Check arguments:
   * - check existence of ctr_file.
   * - check existence of res_file.
   */
  if(strlen(res_file) == 0)
  {
    ERROR_MSG("no LEED theory result file (option -t) specified\n");
    exit(EINVAL);
  }

  if(strlen(ctr_file) == 0)
  {
    ERROR_MSG("no control file (option -c) specified\n");
    exit(EINVAL);
  }

  /* Read sa value from the .bul file */
  if (strlen(ctr_file) > 3)
  {
    length = strlen(ctr_file) - 4;     /* assume 3 char file extension */
  }

  strncpy(proj_name, ctr_file,
            (length < sizeof(proj_name)-1) ?
                length : sizeof(proj_name)-1);
  proj_name[sizeof(proj_name)-1] = '\0';

  strncpy(fname_string, ctr_file,
            (length < sizeof(fname_string)-1) ?
                length : sizeof(fname_string)-1);

  if (strlen(fname_string) + 4 >= sizeof(fname_string)-1 ||
      strlen(fname_string) + 4 > FILENAME_MAX ||
      length > sizeof(fname_string)-1)
  {
    ERROR_MSG("string length too long\n");
    exit(ENAMETOOLONG);
  }
  sprintf(fname_string+length, ".bul");

  if ((fp = fopen(fname_string, "r")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", fname_string);
    exit(EIO);
  }

  while (fgets(linebuffer, STRSZ, fp) != NULL)
  {
    if (!strncasecmp(linebuffer, "sa:", 3))
    {
      iaux = sscanf(linebuffer+3, " %u", &sa);
    }
  }  /*while*/

  fclose(fp);

  /* Makes .ctr file for each angle */
  strncpy(fname_string, ctr_file, length);
  sprintf(fname_string+length, ".ctr");
  ctrinp(fname_string);

  /* Call rfac progam sa times */
  for (i=0; i<sa; i++)
  {
    strncpy(fname_string, proj_name, STRSZ);
    sprintf(fname_string+length, "ia_%d",i+1);

    sprintf(linebuffer,
         "\"%s\" -t \"%s.res\" -c \"%s.ctr"
         "\" -r \"%s\" -s %.2f,%.2f,%.2f > \"%s.dum\"",
         getenv("CAOI_RFAC"),
         fname_string,                /* project name for the. file */
         fname_string,                /* project name for control file */
         rfac_type,                   /*type of R factor*/
         m_rfac_shift_range,          /*initial shift*/
         p_rfac_shift_range,          /*final shift*/
         rfac_shift_step,             /*step of a shift*/
         fname_string);               /* project name for output file */

    if (system (linebuffer) != 0)
    {
      ERROR_MSG("system call: '%s' failed\n", linebuffer);
    }

  } /*for i*/

  /* Read R factor value from .dum file */
  sum1 = 0;
  avershift = 0;
  averfac = 0;
  sum = 0;

  for (i=0; i<sa; i++)
  {
    strncpy(fname_string, proj_name, STRSZ);
    sprintf(fname_string+length, "ia_%u.dum", i+1);

    if ((fpp = fopen(fname_string, "r")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", fname_string);
      exit(EIO);
    }

    while (fgets(linebuffer, STRSZ, fpp)!= NULL)
    {
      iaux = sscanf(linebuffer, 
                    "%f %f %f %f", &rfac, &faux, &shift, &enrange);
    }  /*while*/


    fprintf(STDERR, "\nR%u = %f\n", i+1, rfac);
    avershift = avershift + shift;
    averfac=averfac+rfac*enrange;
    sum = sum + enrange;
    sum1 = sum1 + (1/faux)*(1/faux);
    fclose(fpp);

  } /*for*/


  avershift = (avershift/(float)sa);
  averfac = (averfac/sum);
  averenr = (sum);
  sterror = sqrtf(1/sum1);
  fprintf(STDERR, "<R> = %f\n", averfac);
  fprintf(STDERR, 
          "\n%f %f %f %f     #     Rp  RR  shift range\n", 
          averfac, sterror, avershift, averenr);

  /* Open proj_name.dum file and write the average R factor */
  printf("\n%f %f %f %f     #     Rp  RR  shift range\n", 
            averfac, sterror, avershift, averenr);
  exit(0);
  
  iaux = iaux * 1; /* remove warning */

  return(0);
  
} /*main*/

