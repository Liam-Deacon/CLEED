/*********************************************************************
                        FTSMOOTH_PARSE_ARGS.C

  file contains functions:

  parse_args (24.04.14)
     evaluates arguments and sets appropriate values for ftsmooth
     
  decode_ranges (24.04.14)
     decode string arguments into x ranges
  
Changes:
  LD/2014.04.24 - creation (split from ftsmooth.c)
*********************************************************************/

#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <float.h>

#include "ftsmooth.h"

/****************************************************************
*						PARSE ARGUMENTS							*
*****************************************************************/
/* subroutine to deal with commandline arguments */
int parse_args(int argc, char *argv[], 
	  FILE *in_stream, FILE *out_stream, 
	  int *stdin_flag, int *stdout_flag,
	  double *cutoff, double *tailoff, char *mode,
	  int *offset_flag, double *offset, int *range_flag, size_t *i_r,
	  double *lbound, double *ubound, int *del_flag)
{
 int i_arg;
 
 if (argc == 0)
 {
   ftsmooth_usage(stderr);
   exit(EINVAL);
 }
 
 for (i_arg = 1; i_arg < argc; i_arg++)
 {
  if(*argv[i_arg] != '-')
  {
   fprintf(stderr,"\n\tsyntax error\n");
   ftsmooth_usage(stdout);
   exit(EINVAL);
  }
  else
  {
   if((!strncmp(argv[i_arg], "-i", 2)) || 
      (!strcmp(argv[i_arg], "--input")))
   {
/* Open input file */
    if (++i_arg >= argc)
    {
      ERROR_MSG("no input file specified\n");
      exit(EINVAL);
    }
    if ((in_stream = fopen(argv[i_arg],"r")) == NULL)
    {
     ERROR_MSG("failed to open '%s'\n", argv[i_arg]);
     exit(EIO);
    }
	*stdin_flag = 0;
   }
/* Open output file */
   if((!strncmp(argv[i_arg], "-o", 2)) ||
      (!strcmp(argv[i_arg], "--output"))) 
   {
    if (++i_arg >= argc)
    {
      ERROR_MSG("no input file specified\n");
      exit(EINVAL);
    }
    fclose(out_stream); /* !will always close stdout! */
    if ((out_stream = fopen(argv[i_arg], "w")) == NULL)
    {
     ERROR_MSG("failed to open '%s'\n", argv[i_arg]);
     exit(EIO);
    }
	*stdout_flag = 0;
   }

/* Define cutoff */
   if((!strncmp(argv[i_arg], "-c", 2)) ||
	  (!strcmp(argv[i_arg], "--cutoff")))
   {
     if (++i_arg >= argc)
     {
       ERROR_MSG("no cutoff value specified\n");
       exit(EINVAL);
     }
     *cutoff = (double)atof(argv[i_arg]);
     if (*cutoff > 1.)
       *cutoff = 1.;
   }
/* Define tailoff */
   if((!strncmp(argv[i_arg], "-t", 2)) ||
      (!strcmp(argv[i_arg], "--tailoff")))
   {
     if (++i_arg >= argc)
     {
       ERROR_MSG("no tailoff value specified\n");
       exit(EINVAL);
     }
     arg_parse_dbl(tailoff, argc, argv, i_arg);
   }

/* Define mode */
   if((!strncmp(argv[i_arg], "-m", 2)) || 
      (!strcmp(argv[i_arg], "--mode")))
   {
     if (++i_arg >= argc)
     {
       ERROR_MSG("no mode defined\n");
       exit(EINVAL);
     }
     *mode = *argv[i_arg];
     switch (*mode)
     {
      case 'n': case 's':
        break;
      default:
        ERROR_MSG("Wrong mode option: \"%c\"\n", *mode);
        exit(EINVAL);
     }
   }

/* Define offset */
   if((!strncmp(argv[i_arg], "-y", 2)) ||
	  (!strcmp(argv[i_arg], "--offset")))
   {
     *offset_flag = 1;
	 if (++i_arg < argc)
	 {
	   switch (*argv[i_arg])
	   {
	     case '*': case 'z':
		   *offset_flag = OFFSET_Y_TO_ZERO;
	       break;

	     case '-': /* check argument */
	       if (strlen(argv[i_arg])-1) /* check for command options */
		     if ((isalpha(argv[i_arg][1])) || (argv[i_arg][1] == '-'))
		     {
		       *offset_flag = OFFSET_Y_TO_ZERO;
		     }
	       break;

	     default:
	       *offset = (double)atof(argv[i_arg]);
	   }
	 }
   }
/* Define range */
   if((!strncmp(argv[i_arg], "-r", 2)) ||
	  (!strcmp(argv[i_arg], "--range")))
   {
     *range_flag = 1; 

     /* initialise ranges */
     *i_r = 1;

     if (++i_arg < argc)
     {
       decode_ranges(lbound, ubound, &*i_r, argv[i_arg]);
     }
     else
     {
       ERROR_MSG("no range values given\n");
       exit(EINVAL);
     }
   }
/* Define delete */
   if((!strncmp(argv[i_arg], "-d", 2)) ||
	  (!strcmp(argv[i_arg], "--delete")))
   {
     *del_flag = 1;
   }

/* Help */
   if((!strncmp(argv[i_arg], "-h", 2)) ||
	  (!strcmp(argv[i_arg], "--help")))
   {
     ftsmooth_usage(stdout);
     exit(0);
   }
/* Print version information to stdout */
   if(!strcmp(argv[i_arg], "--version") || !strcmp(argv[i_arg], "-V"))
   {
     ftsmooth_info();
     exit(0);
   }
  } /* else */
 }  /* for i_arg */

 return(0);
}

/********************************************************************
*					DECODE RANGE ARGUMENT							*
*********************************************************************/
/* subroutine to decode argv range argument into seperate ranges
returns the dimension of the lower and upper bound matricies*/
int decode_ranges(double *lbound, double *ubound, size_t *i_r, const char *argv)
/* 
Encoding as follows: 
  
  '*' = no bound 
  ',' = iterator
  ':' = separator 
  
  i.e. '230:600,*:624.9' will allow any value between 230 to 600, or
        any value <= 624.9
*/
{
  int pos = 0;
  int islowerbound = 1; /* first entry is lower bound */

  char *str = NULL;
  char ichar[1] = "\0";
  
  /* check for NULLs and allocate memory if needed */
  CLEED_ALLOC_CHECK(str = (char*)calloc(sizeof(char), strlen(argv) + 1));

  if (ubound == NULL)
  {
    CLEED_ALLOC_CHECK(ubound = (double *)calloc(sizeof(double), 1));
  }

  if (lbound == NULL)
  {
    CLEED_ALLOC_CHECK(lbound = (double *)calloc(sizeof(double), 1));
  }

  if (i_r == NULL)
  {
    CLEED_ALLOC_CHECK(i_r = (size_t *)calloc(sizeof(size_t), 1));
    *i_r = 0;
  }

  ubound[0] = DBL_MAX;
  lbound[0] = DBL_MIN;
  
  while (argv[pos] != '\0') /* might be dangerous as will iterate until nul */
  {   
    switch (argv[pos])
	  {
	    case ':':
        if(islowerbound)
          if(!strcmp(str, "\0"))
            lbound[*i_r] = atof(str);
        printf("%s\n", str);
        islowerbound = (islowerbound+1) % 2;
        strcpy(str, "\0");
        break;
		  
	    case ',': /* change to next dataset */
        if(islowerbound) /* redimension bound arrays */
        {
          lbound[*i_r] = DBL_MIN;
          if(!strcmp(str, "\0")) ubound[*i_r] = atof(str);
          else break; /* both bounds could be anything */
        }
        else
        {
        if(!strcmp(str, "\0"))
            ubound[*i_r] = atof(str);
        printf("%s\n", str);
        }

        *i_r += 1;
        CLEED_REALLOC(ubound, (*i_r)*sizeof(double));
        CLEED_REALLOC(lbound, (*i_r)*sizeof(double));

        islowerbound = (islowerbound+1) % 2;
        strcpy(str, "\0");
        break;
		
	    case '*': case 'i': /* ignore */
	      if(islowerbound) lbound[*i_r] = DBL_MIN;
	      else ubound[*i_r] = DBL_MAX;
	      islowerbound = (islowerbound+1) % 2;
	      strcpy(str, "\0");
	      break;
	    
	    default:
	      if(isdigit(argv[pos]) || (argv[pos] == '-'))
	      {
	        ichar[0] = argv[pos];
	        strcat(str, ichar);
	      }
	      break;
	  } /* switch */
    pos++;
  } /* while */
  
  if(!islowerbound) ubound[*i_r-1] = DBL_MAX;
  
  if (str) free(str);

  return(0);
}
