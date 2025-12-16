/*********************************************************************
ZZ/03.03.08
  file contains functions:

  main (03.03.08)
     Main program for CAOI_LEED program 
     (calls cleed program for each angle of incidence)

Changes:
 LD/25.04.14 - added help() and info() 

*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "caoi_leed.h"
#include "proghelp.h"

int sa;

int main (int argc, char *argv[])
{
  int i_arg;
  int length;
  int i_ang;

  char bsr_file[STRSIZE];
  char par_file[STRSIZE];
  char res_file[STRSIZE];

  char linebuffer[STRSIZE];
  char helpstring[STRSIZE];
  char line_buffer[STRSIZE];

  FILE *inp_stream;
  FILE *write_stream;
  FILE *read_stream;

/*****************************************************************
 Preset program parameters
******************************************************************/

  sa = 999;

  strncpy(bsr_file,"---", STRSIZE);
  strncpy(par_file,"---", STRSIZE);
  strncpy(res_file,"leed.res", STRSIZE);

/*****************************************************************
Decode arguments:
 -b project name for modified bulk file
 -i parameter file for overlayer
 -o project name for output file
******************************************************************/
  if (argc < 2) {usage(STDERR);exit(1);}

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if(*argv[i_arg] != '-')
    {
#ifdef ERROR
      fprintf(STDERR,"*** error (caoi_leed_main):\tsyntax error:\n");
      fprintf(STDERR,"\tusage: \n\t"
        "caoi_leed" 
          "-b <bsr_file>" 
          "-i <par_file>"
          "-v <vertex_file>"
          "-o <res_file>""\n");
#endif
      exit(1);
    }
    else
    {

      /* Display help */
      if(strcmp(argv[i_arg], "-h") == 0 ||
         strcmp(argv[i_arg], "--help") == 0) 
      {
        usage(stdout);
        exit(0);
      }  

      /* Display program information */
      if(strcmp(argv[i_arg], "-V") == 0 || 
         strcmp(argv[i_arg], "--version") == 0)
      {
        info(); 
        exit(0);
      }
      
      /* Read modified bulk file */
      if(strncmp(argv[i_arg], "-b", 2) == 0)
      {
        i_arg++;
        strncpy(bsr_file,argv[i_arg],STRSIZE);
      }

      /* Read parameter input file */
      if(strncmp(argv[i_arg], "-i", 2) == 0)
      {
        i_arg++;
        strncpy(par_file,argv[i_arg],STRSIZE);
      }

      /* Read vertex file */
      if(strncmp(argv[i_arg], "-o", 2) == 0)
      {
        i_arg++;
        strncpy(res_file, argv[i_arg], STRSIZE);
      }

    } /* else */
  }  /* for i_arg */

/*****************************************************************
  Check arguments:
  - check existence of par_file.
  - if bsr_file is not specified use par_file instead.
******************************************************************/

  if(strncmp(par_file, "---", 3) == 0)
  {
#ifdef ERROR
    fprintf(STDERR,
    "*** error (caoi_leed_main): no parameter file (option -i) specified\n");
#endif
    exit(1);
  }

  if(strncmp(bsr_file, "---", 3) == 0)
  {
    strncpy(bsr_file, par_file, STRSIZE);
  }

/*****************************************************************
 Initialize "sa" with the number of angles of incidence

******************************************************************/

 length = strlen(par_file) - 4;
 strncpy(linebuffer, par_file, length);
 sprintf(linebuffer+length,".bsr");

 if ((inp_stream = fopen(linebuffer,"r")) == NULL)
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (caoi_leed_main): could not open output file \"%s\"\n",
      linebuffer);
#endif
     exit(1);
    } 

 while (fgets(linebuffer,STRSIZE,inp_stream))
    {
      if (!strncasecmp(linebuffer, "sa:", 3)) {sscanf(linebuffer+3,"%d",&sa);}
    }

 if (sa == 999)
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (caoi_leed_main): could not read sa: from the .bsr file");
#endif
     exit(1);
    }

fclose(inp_stream);

/*****************************************************************
 Makes new .bsr file for each angle of incidence
  - calls the function bulinp()
******************************************************************/

bsrinp(bsr_file, sa);

#ifdef CONTROL_X
fprintf(STDERR,
   "*** caoi_leed_main: sa = %d ***\n", sa);
#endif

/*****************************************************************
 Calls cleed_nsym program for each angle of incidence
*****************************************************************/

  for (i_ang = 0; i_ang < sa; i_ang ++)
  {

    /* append "ia_<i_ang>" to par_file */
    strncpy(linebuffer, par_file, length);
    sprintf(linebuffer + length, "ia_%d", i_ang + 1);

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
    if (system(helpstring)) 
    { 
      fprintf(STDERR,
        "***(caoi_leed_main): system call: %s failed\n", helpstring);
    }

   } /*for i_ang*/

/*********************************************************************
 Makes proj_name.res = proj_nameia_1.res + proj_nameia_2.res + ...
*********************************************************************/

  strncpy(linebuffer, par_file, length);
  sprintf(linebuffer + length, ".res");

  if ((write_stream = fopen(linebuffer,"w")) == NULL)
  {
#ifdef ERROR
    fprintf(STDERR,
      "*** error (caoi_leed_main): could not open output file \"%s\"\n",
      linebuffer);
#endif
    exit(1);
  }

  for (i_ang = 0; i_ang < sa; i_ang ++)
  {
    strncpy(helpstring, par_file, length);
    sprintf(helpstring + length, "ia_%d.res", i_ang + 1);
    if ((read_stream = fopen(helpstring,"r")) == NULL)
    {
#ifdef ERROR
      fprintf(STDERR,
        "*** error (caoi_leed_main): could not open output file \"%s\"\n",
        helpstring);
#endif
      exit(1);
    }

    fprintf(write_stream, "Copy of %s\n", helpstring);

    while (fgets(line_buffer , STRSIZE, read_stream))
    {
      fprintf(write_stream, "%s", line_buffer);
    }

    fclose(read_stream);
    
  } /* for i_ang */

  fclose(write_stream);
  fclose(STDERR);

  return(0);

} /* main */
