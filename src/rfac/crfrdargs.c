/*********************************************************************
LD/07.03.14
  CRFRDARGS
 
File contains:
   struct crargs cr_rdargs (int argc, char **argv)

Changes:
  GH/27.10.92 - Creation
  GH/30.08.95 - Adaptation to CRFAC
  LD/07.03.14 - Added POSIX style arguments
*********************************************************************/
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <strings.h>
#include <malloc.h>

#include "cleed_cstring.h"
#include "crfac.h"

#define ARG_IS(text) (strcmp(argv[i], text) == 0)

struct crargs cr_rdargs (int argc, char **argv)

/*********************************************************************
  Read argument list from command line and set the adequate parameters
  for the program (structure crargs)

  input: argc - argument counter
         argv - argument vector
 
  valid arguments:

  --print
  -a <flag>: specify which group ID's appear in output.
	     arguments: "al(l)", "av(erage)".
	     default: "average".
	     program parameter: all_groups

  --control
  -c <filename>: specify control file for averaging and assigning
	     data input.
	     program parameter: ctrfile

  --help
  -h:        Print help file.

  --output
  -o <output file>: specify a file name for the r factor output to be
             written to.
	     valid arguments: "stdout" / "-", "single", <file name>.
	     default: "stdout".

  --rfactor
  -r <r_factor>: specify a particular R-factor to be used for comparison.
	     valid arguments: "r1", "r2", "rb", "rp".
	     default: "rp".
	     program parameter: r_type

  --shift
  -s <shift>: specify an energy range for shifting experimental and
	     theoretical data with respect to each other (Ee = Et + shift).
	     1-3 arguments: <first shift>{,<last shift>,<step>}
             default: -10, 10, 0.5;
	     program parameters: s_ini, s_fin, s_step.

  --theory
  -t <filename>: specify theoretical input file (outside control file)
             program parameter: thefile.

  --potential
  -v <potential>: imaginary part of optical potential (used for
             smoothing and for computation of Pendry's R-factor).
	     arguments: floating point number [eV].
	     default: 4.0
	     program parameter: vi

  --write
  -w <filename>: specify file name for iv curves output.
             program parameter: iv_file.


  output: argument list.
	  function exits if any error occured.
*********************************************************************/

{
  int i, j;

/* 
structure containing all program parameters:
  char *ctrfile              - input control file 
  char *thefile              - input theor file 
  char *outfile              - output file.
  int r_type                 - R factor type 
  real  s_ini                - initial shift
  real  s_fin                - final shift
  real  s_step               - step for shift
  int all_groups             - flag: print R-factors of all group ID's 
  real vi                    - im. part of optical potential.
  real ignore_max            - ignore features larger than ignore_max.
*/
  struct crargs args;

/*********************************************************************
 set default values 
*********************************************************************/

  args.ctrfile = NULL;

  args.s_ini = -10.;
  args.s_fin =  10.;
  args.s_step = 0.5;

  args.r_type = RP_FACTOR;         /* default R factor: rp */

  args.all_groups = 0;             /* display only averge over R-factors */
  args.vi = 4.0;                   /* im. part of opt. potential */

  args.iv_out = 0;                 /* default: no output of IV curves */
  args.iv_file = NULL;

  args.outfile = NULL;             /* will be treated as "stdout" */

/*********************************************************************
 decode arguments 
*********************************************************************/
  printf("got here\n");
  if( argc == 1)
  {
    rf_help(stderr);
    exit(1);
  }
  for( i=1; i < argc; i++)
  {
    if(argv[i][0] == '-')
    {
     
      if (ARG_IS("-a") || ARG_IS("--print"))
      {
        /* 
         -a <flag>: specify which group ID's appear in output.
                    arguments: "al(l)", "av(erage)".
                    default: "all".
        */
    
        if (++i < argc)
        {        
          if ( !strncasecmp(argv[i], "av", 2) )
            args.all_groups = 0;
          else if ( !strncasecmp(argv[i], "al", 2) )
            args.all_groups = 1;
          else
          {
            fprintf(stderr, " *** error in argument list:");
            fprintf(stderr, 
                    " \"%s\" requires \"ave\" or \"all\" as argument.\n", 
                    argv[i-1]);
            exit(1);
          }
        }
        else
        {
          fprintf(stderr, 
                " *** error in argument list: missing argument for \"%s\"\n", 
                argv[i-1]);
          exit(1);
        }
    
      } /* case a */

      else if (ARG_IS("-c") || ARG_IS("--control"))
      {
        /* 
         -c <filename>: specify control file for averaging and assigning
                data input.
        */
    
        if (++i < argc) args.ctrfile = argv[i];
        else
        {
          fprintf(stderr, 
                " *** error in argument list: missing argument for \"%s\"\n", 
                argv[i-1]);
          exit(1);
        }
      } /* case c */

      else if (ARG_IS("-h") || ARG_IS("--help")) 
      {
        /*
            call help function
        */
        rf_help(stdout);
        exit(1);
      } /* case h */

      else if (ARG_IS("-o") || ARG_IS("--output"))
      {
        /* 
           -o <output file>: specify a file name for the r factor output to be
                     written to.
                 valid arguments: "stdout", "single", <file name>.
                 default: "stdout".
        */
        if (++i < argc) args.outfile = argv[i];
        else
        {
          fprintf(stderr, 
                  " *** error in argument list: missing argument for \"%s\"\n", 
                  argv[i-1]);
          exit(1);
        }
      } /* case o */

      else if (ARG_IS("-r") || ARG_IS("--rfactor"))
      {
        /* 
         -r <r_factor>: specify which R-factor should be used for comparison.
                Valid arguments are: "r1", "r2", "rb", "rp".
                default: "rp".
        */
    
        if (++i < argc)
        {
          if( !strncasecmp(argv[i],"rp", 2) )
            args.r_type = RP_FACTOR;
          else if( !strncasecmp(argv[i],"r1", 2) )
            args.r_type = R1_FACTOR;
          else if( !strncasecmp(argv[i],"r2", 2) )
            args.r_type = R2_FACTOR;
          else if( !strncasecmp(argv[i],"rb", 2) )
            args.r_type = RB_FACTOR;
          else
          {
#ifdef ERROR
            fprintf(STDERR,
                "*** error in argument list: R-factor \"%s\" not known\n", 
                argv[i]);
#endif
            exit(1);
          }
        }
        else
        {
          fprintf(stderr, 
                " *** error in argument list: missing argument for \"%s\"\n", 
                argv[i-1]);
          exit(1);
        }
      } /* case r */

      else if (ARG_IS("-s") || ARG_IS("--shift"))
      {
        /* 
         -s <shift>: specify an energy range for shifting experimental and 
                 theoretical data with respect to each other.
                 1-3 arguments: <first shift>{,<last shift>,<step>}
        */
    
        if (++i < argc)
        {
#ifdef REAL_IS_DOUBLE
          j = sscanf(argv[i],"%lf,%lf,%lf",
#endif
#ifdef REAL_IS_FLOAT
          j = sscanf(argv[i],"%f,%f,%f",
#endif
                     &(args.s_ini), &(args.s_fin), &(args.s_step));

          switch (j)     /* j is the number of arguments */
          {
            case (1):    /* one number: one single shift */
            {
              args.s_fin = args.s_ini;
              args.s_step = 0.5;
              break;
            }
            case (2):    /* two numbers: range of shifts with step 0.5 */
            {
              args.s_step = 0.5;
              break;
            }
            case (3):    /* three numbers: range of shifts with given step */
            {
              break;
            }
            default:
            {
              fprintf(stderr, " *** error in argument list:\n");
              fprintf(stderr, 
                      "\toption \"%s\"needs at least 1, maximum 3 arguments\n", 
                      argv[i-1]);
              exit(1);
            }
          } /* switch j */
        }  /* if i++ < argc */
        else
        {
          fprintf(stderr, 
                " *** error in argument list: missing argument for \"%s\"\n", 
                argv[i-1]);
          exit(1);
        }
      } /* case s */

      else if (ARG_IS("-t") || ARG_IS("--theory"))
      {
        /*
          -t <filename>: specify theoretical input file
        */
     
        if (++i < argc) args.thefile = argv[i];
        else
        {
          fprintf(stderr, 
                " *** error in argument list: missing argument for \"%s\"\n",
                argv[i-1]);
          exit(1);
        }
      } /* case t */


      else if (ARG_IS("-v") || ARG_IS("--potential"))
      {
        /* 
           -v <potential>: imaginary part of optical potential (used for
                     smoothing and for computation of Pendry's R-factor).
                     arguments: floating point number [eV].
                     default: 4.0
        */
        if (++i < argc)
        {
          args.vi = (real)atof(argv[i]);
        }
        else
        {
          fprintf(stderr, 
            " *** error in argument list: missing argument for \"%s\"\n", 
            argv[i-1]);
          exit(1);
        }
      } /* case v */

      else if (ARG_IS("-w") || ARG_IS("--write"))
      {
        /*
         -w <filename>: specify file name for IV curves
        */
        if (++i < argc)
        {
          args.iv_out = 1;
          args.iv_file = argv[i];
        }
        else
        {
          fprintf(stderr, 
            " *** error in argument list: missing argument for \"%s\"\n",
            argv[i-1]);
          exit(1);
        }
      } /* case w */

      else if (ARG_IS("-V") || ARG_IS("--version"))
      {
        rf_info();
        exit(0);
      }
      else
      {
        fprintf(stderr,
            " *** error in argument list: \"%s\" not known.\n", argv[i]);
        rf_help(stderr);
        exit(1);
      }
    
    }   /* if argv[...] == '-') */
    else
    {
      fprintf(stderr, " *** error in argument list: \"%s\"\n", argv[i]);
      rf_help(stderr);
      exit(1);
    }
  }    /* for i */

  if (args.ctrfile == NULL)
  {
    fprintf(stderr, 
            " *** error in argument list: No control file specified\n");
    fprintf(stderr, "     use option -c to specify\n");
    exit(1);
  }

  return (args);
}
/********************************************************************/
