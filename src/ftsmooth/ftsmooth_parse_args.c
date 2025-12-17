/*********************************************************************
LD/24.04.14
                        FTSMOOTH_PARSE_ARGS

  file contains functions:

  parse_args (24.04.14)
     evaluates arguments and sets appropriate values for ftsmooth
     
  decode_ranges (24.04.14)
     decode string arguments into x ranges
  
Changes:

*********************************************************************/

#include "ftsmooth.h"
#include "cleed_cstring.h"
#include "cleed_string.h"


/****************************************************************
*						PARSE ARGUMENTS							*
*****************************************************************/
/* subroutine to deal with commandline arguments */
int parse_args(int argc, char *argv[], 
	  FILE *in_stream, FILE *out_stream, 
	  int *stdin_flag, int *stdout_flag,
	  double *cutoff, double *tailoff, char *mode,
	  int *offset_flag, double *offset, int *range_flag, int *i_r,
	  double *lbound, double *ubound, int *del_flag)
{
 int i_arg;
 
 if (!argc) {ftsmooth_usage(stderr);exit(1);}
 
 for (i_arg = 1; i_arg < argc; i_arg++)
 {
  if(*argv[i_arg] != '-')
  {
   fprintf(stderr,"\n\tsyntax error\n");
   ftsmooth_usage(stdout);
   exit(1);
  }
  else
  {
   if((!strncmp(argv[i_arg], "-i", 2)) || 
      (!strcmp(argv[i_arg], "--input")))
   {
/* Open input file */
    i_arg++;
    if ((in_stream = fopen(argv[i_arg],"r")) == NULL)
    {
     fprintf(stderr,"error: failed to open '%s'",argv[i_arg]);
     exit(1);
    }
	*stdin_flag = 0;
   }
/* Open output file */
   if((!strncmp(argv[i_arg], "-o", 2)) ||
      (!strcmp(argv[i_arg], "--output"))) 
   {
    i_arg++;
    fclose(out_stream); /* !will always close stdout! */
    if ((out_stream = fopen(argv[i_arg],"w")) == NULL)
    {
     fprintf(stderr,"error: failed to open '%s'",argv[i_arg]);
     exit(1);
    }
	*stdout_flag = 0;
   }

/* Define cutoff */
   if((!strncmp(argv[i_arg], "-c", 2)) ||
	  (!strcmp(argv[i_arg], "--cutoff")))
   {
     i_arg++;
     *cutoff = (double)atof(argv[i_arg]);
     if (*cutoff > 1.)
     *cutoff = 1.;
   }
/* Define tailoff */
   if((!strncmp(argv[i_arg], "-t", 2)) ||
      (!strcmp(argv[i_arg], "--tailoff")))
   {
     i_arg++;
     ARG_PARSE_DBL(*tailoff);
   }

/* Define mode */
   if((!strncmp(argv[i_arg], "-m", 2)) || 
      (!strcmp(argv[i_arg], "--mode")))
   {
     i_arg++;
     if (i_arg >= argc) 
     {
       fprintf(stderr, "***error (ftsmooth): no mode defined\n");
       exit(1);
     }
     *mode = *argv[i_arg];
     switch (*mode)
     {
      case 'n': case 's':
        break;
      default:
        fprintf(stderr, 
            "***error (ftsmooth): Wrong mode option: \"%c\"\n", *mode);
        exit(1);
     }
   }

/* Define offset */
   if((!strncmp(argv[i_arg], "-y", 2)) ||
	  (!strcmp(argv[i_arg], "--offset")))
   {
     *offset_flag = 1;
	 if (i_arg+1 < argc)
	 {
	   i_arg++;
	   switch (*argv[i_arg])
	   {
	     case '*': case 'z':
		   *offset_flag = OFFSET_Y_TO_ZERO;
	       break;
		 case '-': /* check argument */
		   if (cleed_strnlen(argv[i_arg], 4096) > 1) /* check for command options */
		     if ((isalpha(argv[i_arg][1])) || (argv[i_arg][1] == '-'))
			 {
			    *offset_flag = OFFSET_Y_TO_ZERO;
				break;
			 }			 
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

	 if (i_arg+1 < argc)
	 {
	   i_arg++;	   
	   decode_ranges(lbound, ubound, &*i_r, argv[i_arg]);  
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

 return 0;
}

/********************************************************************
*					DECODE RANGE ARGUMENT							*
*********************************************************************/
/* subroutine to decode argv range argument into seperate ranges
returns the dimension of the lower and upper bound matricies*/
int decode_ranges(double *lbound, double *ubound, int *i_r, char *argv)
/* 
Encoding as follows: 
  
  '*' = no bound 
  ',' = iterator
  ':' = separator 
  
  i.e. '230:600,*:624.9' will allow any value between 230 to 600, or
        any value <= 624.9
*/
{
  int pos;
  int islowerbound;
  
  char *str;
  char ichar[1];

  str = (char *) malloc(sizeof(char)*(cleed_strnlen(argv, 4096)+1));
  ichar[0] = '\0';
  
  pos = 0; 
  islowerbound = 1; /* first entry is lower bound */
  if(*i_r<0) *i_r=0; 
  
  ubound[0] = DBL_MAX;
  lbound[0] = DBL_MIN;
  
  while (argv[pos] != '\0') /* might be dangerous as will iterate until NULL */
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
			if(!strcmp(str, "\0"))
		      ubound[*i_r] = atof(str);
			else
			  break; /* both bounds could be anything */
		  }
		  else
		  {
			if(!strcmp(str, "\0"))
		      ubound[*i_r] = atof(str);
			printf("%s\n", str);
		  }
		  
		  ubound = (double *) realloc(ubound,(*i_r+1)*sizeof(double));
		  lbound = (double *) realloc(lbound,(*i_r+1)*sizeof(double));
		  *i_r =(*i_r)+1;
		  islowerbound = (islowerbound+1) % 2;
		  strcpy(str, "\0");
		  break;
		
	    case '*': case 'i': /* ignore */
		  if(islowerbound)
		    lbound[*i_r] = DBL_MIN;
		  else
		    ubound[*i_r] = DBL_MAX;
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
	   }
    pos++;
  }
  
  if(!islowerbound)
	ubound[*i_r-1] = DBL_MAX;
  
  return 0;
}
