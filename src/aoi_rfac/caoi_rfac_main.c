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
#include "caoi_rfac.h"
#include "gh_stddef.h"

extern void caoi_rfac_info();
extern void caoi_rfac_usage(FILE *);

int main(int argc, char *argv[])
{
  size_t i;
  int i_arg;
  size_t length = 0;
  int iaux;
  char linebuffer[STRSZ];
  char string[STRSZ];
  FILE *fp;
  FILE *fpp;
  char res_file[FILENAME_MAX];                 /* input/output files */
  char ctr_file[FILENAME_MAX];
  char rfac_type[STRSZ];
  float m_rfac_shift_range;
  float p_rfac_shift_range;
  float rfac_shift_step;
  float rfac;
  float faux, shift;
  float enrange;
  float averfac;
  float averenr;
  float avershift;
  float sum;
  float sterror;
  float sum1;


  /* set default values */
  m_rfac_shift_range = -10.;
  p_rfac_shift_range =  10.;
  rfac_shift_step = 0.5;
  strncpy(rfac_type, "rp", STRSZ);
  strncpy(res_file, "---", FILENAME_MAX);
  strncpy(ctr_file, "---", FILENAME_MAX);
  iaux = 0;

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
      exit(1);
  }
  
  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
#if ERROR_LOG
      ERROR_MSG("syntax error:\n");
    	caoi_rfac_usage(STDERR);
#endif
      exit(1);
    }
    else
    {

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-t", 2) == 0 ||
         strcmp(argv[i_arg], "--theory") == 0 )
      {
        i_arg++;
        strncpy(res_file, argv[i_arg], FILENAME_MAX);
      } /* -t */

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-c", 2) == 0 ||
         strcmp(argv[i_arg], "--control") == 0 )
      {
        i_arg++;
        strncpy(ctr_file, argv[i_arg], FILENAME_MAX);
      } /* -i */

      /* Read R factor type*/
      if(strncmp(argv[i_arg], "-r", 2) == 0 ||
         strcmp(argv[i_arg], "--rfactor") == 0 )
      {
        i_arg++;
        strncpy(rfac_type, argv[i_arg], STRSZ);
      } /* -r */


      /* Read initial shift, final shift, step of a shift*/
      if(strncmp(argv[i_arg], "-s", 2) == 0 ||
         strcmp(argv[i_arg], "--shift") == 0 )
      {
        i_arg++;
        sscanf(argv[i_arg], "%f,%f,%f", &m_rfac_shift_range,
        		   &p_rfac_shift_range, &rfac_shift_step);
	
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
  if(strncmp(res_file, "---", 3) == 0)
  {
    ERROR_MSG("no .res file (option -t) specified\n");
    exit(1);
  }

  if(strncmp(ctr_file, "---", 3) == 0)
  {
    ERROR_MSG("no .ctr file (option -t) specified\n");
    exit(1);
  }


  /* Read sa value from the .bul file */
  if (strlen(ctr_file) > 3)
  {
    length = strlen(ctr_file) - 4;     /* assume 3 char file extension */
  }
  strncpy(proj_name, ctr_file, length);
  strncpy(string, ctr_file, length);
  sprintf(string+length, ".bul");


  if ((fp = fopen(string, "r")) == NULL)
  {
    ERROR_MSG("could not open output file \"%s\"\n", string);
    exit(1);
  }

  while (fgets(linebuffer, STRSZ, fp))
  {
    if (!strncasecmp(linebuffer, "sa:", 3))
    {
      iaux = sscanf(linebuffer+3, " %u", &sa);
    }
  }  /*while*/

  fclose(fp);

  /* Makes .ctr file for each angle */
  strncpy(string, ctr_file, length);
  sprintf(string+length, ".ctr");
  ctrinp(string);

  /* Call rfac progam sa times */
  for (i=0; i<sa; i++)
  {

    strncpy(string, proj_name, STRSZ);
    sprintf(string+length, "ia_%d",i+1);

    sprintf(linebuffer,
         "\"%s\" -t \"%s.res\" -c \"%s.ctr\" -r \"%s\" -s %.2f,%.2f,%.2f > \"%s.dum\"",
         getenv("CAOI_RFAC"),
         string,                  /* project name for the. file */
         string,                  /* project name for control file */
         rfac_type,                 /*type of R factor*/
         m_rfac_shift_range,          /*initial shift*/
         p_rfac_shift_range,            /*final shift*/
         rfac_shift_step,             /*step of a shift*/
         string);                 /* project name for output file */


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
    strncpy(string, proj_name, STRSZ);
    sprintf(string+length, "ia_%u.dum", i+1);

    if ((fpp = fopen(string, "r")) == NULL)
    {
      ERROR_MSG("could not open output file \"%s\"\n", string);
      exit(1);
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

