/*********************************************************************
 *                       RFRFARGS.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 * GH/1992.10.27 - Creation
 * GH/1995.08.30 - Adaptation to CRFAC
 * LD/2014.03.07 - Added POSIX style arguments
 *********************************************************************/

/*! \file
 *  \brief File contains C implementation for \ref rfac_rdargs function.
 */

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <strings.h>
#include <malloc.h>

#include "rfac.h"

#define ARG_IS(text) (strcmp(argv[i], text) == 0)

/*!
 * Read argument list from command line and set the adequate parameters
 * for the program (structure crargs)
 *
 * \param argc argument counter
 * \param argv argument string vector
 *
 * valid arguments:
 *
 * --print
 * -a <flag>: specify which group ID's appear in output.
 *      arguments: "al(l)", "av(erage)".
 *      default: "average".
 *      program parameter: all_groups
 *
 * --control
 * -c <filename>: specify control file for averaging and assigning
 *      data input.
 *      program parameter: ctrfile
 *
 * --help
 * -h:        Print help file.
 *
 * --output
 * -o <output file>: specify a file name for the r factor output to be
 *            written to.
 *      valid arguments: "stdout" / "-", "single", <file name>.
 *      default: "stdout".
 *
 * --rfactor
 * -r <r_factor>: specify a particular R-factor to be used for comparison.
 *      valid arguments: "r1", "r2", "rb", "rp".
 *      default: "rp".
 *      program parameter: r_type
 *
 * --shift
 * -s <shift>: specify an energy range for shifting experimental and
 *      theoretical data with respect to each other (Ee = Et + shift).
 *      1-3 arguments: <first shift>{,<last shift>,<step>}
 *            default: -10, 10, 0.5;
 *      program parameters: s_ini, s_fin, s_step.
 *
 * --theory
 * -t <filename>: specify theoretical input file (outside control file)
 *            program parameter: thefile.
 *
 * --potential
 * -v <potential>: imaginary part of optical potential (used for
 *            smoothing and for computation of Pendry's R-factor).
 *      arguments: floating point number [eV].
 *      default: 4.0
 *      program parameter: vi
 *
 * --write
 * -w <filename>: specify file name for iv curves output.
 *            program parameter: iv_file.
 *
 *
 * \return #rfac_args struct containing argument list.
 *
 * \warning Function exits if any error occurred.
 */
rfac_args *rfac_rdargs(int argc, char **argv)
{
  int i, j;

  /* structure containing all program parameters:
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

  rfac_args *args = rfac_args_init();

  /*********************************************************************
   * decode arguments
   *********************************************************************/
  if( argc == 1)
  {
    rfac_help(stderr);
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
            args->all_groups = false;
          else if ( !strncasecmp(argv[i], "al", 2) )
            args->all_groups = true;
          else
          {
            ERROR_MSG("in argument list "
                    " \"%s\" requires \"ave\" or \"all\" as argument.\n", 
                    argv[i-1]);
            exit(RFAC_INVALID_ARGUMENT);
          }
        }
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
        }
    
      } /* case a */

      else if (ARG_IS("-c") || ARG_IS("--control"))
      {
        /* 
         -c <filename>: specify control file for averaging and assigning
                data input.
        */
    
        if (++i < argc) strncpy(args->ctr_file, argv[i], FILENAME_MAX);
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
        }
      } /* case c */

      else if (ARG_IS("-h") || ARG_IS("--help")) 
      {
        /*
            call help function
        */
        rfac_help(stdout);
        exit(RFAC_SUCCESS);
      } /* case h */

      else if (ARG_IS("-o") || ARG_IS("--output"))
      {
        /* 
           -o <output file>: specify a file name for the r factor output to be
                     written to.
                 valid arguments: "stdout", "single", <file name>.
                 default: "stdout".
        */
        if (++i < argc) strncpy(args->out_file, argv[i], FILENAME_MAX);
        else
        {
          fprintf(stderr, " *** error (rfac_rdargs): "
                  "missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
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
            args->r_type = RP_FACTOR;
          else if( !strncasecmp(argv[i],"r1", 2) )
            args->r_type = R1_FACTOR;
          else if( !strncasecmp(argv[i],"r2", 2) )
            args->r_type = R2_FACTOR;
          else if( !strncasecmp(argv[i],"rb", 2) )
            args->r_type = RB_FACTOR;
          else
          {
            ERROR_MSG("R-factor \"%s\" not known\n", argv[i]);
            exit(RFAC_INVALID_ARGUMENT);
          }
        }
        else
        {
          fprintf(stderr, " *** error (rfac_rdargs): "
                  "missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
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
          j = sscanf(argv[i], "%" REAL_FMT "f,%" REAL_FMT "f,%" REAL_FMT "f",
                     &(args->s_ini), &(args->s_fin), &(args->s_step));

          switch (j)     /* j is the number of arguments */
          {
            case (1):    /* one number: one single shift */
            {
              args->s_fin = args->s_ini;
              args->s_step = 0.5;
              break;
            }
            case (2):    /* two numbers: range of shifts with step 0.5 */
            {
              args->s_step = 0.5;
              break;
            }
            case (3):    /* three numbers: range of shifts with given step */
            {
              break;
            }
            default:
            {
              ERROR_MSG("\n\"%s\"needs at 1 >= i <=3 arguments\n", argv[i-1]);
              exit(RFAC_INVALID_ARGUMENT);
            }
          } /* switch j */
        }  /* if i++ < argc */
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
        }
      } /* case s */

      else if (ARG_IS("-t") || ARG_IS("--theory"))
      {
        /*
          -t <filename>: specify theoretical input file
        */
     
        if (++i < argc) strncpy(args->the_file, argv[i], FILENAME_MAX);
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
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
          args->vi = (real)atof(argv[i]);
        }
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
        }
      } /* case v */

      else if (ARG_IS("-w") || ARG_IS("--write"))
      {
        /*
         -w <filename>: specify file name for IV curves
        */
        if (++i < argc)
        {
          strncpy(args->iv_file, argv[i], FILENAME_MAX);
        }
        else
        {
          ERROR_MSG("missing argument for \"%s\"\n", argv[i-1]);
          exit(RFAC_INVALID_ARGUMENT);
        }
      } /* case w */

      else if (ARG_IS("-V") || ARG_IS("--version"))
      {
        rfac_info();
        exit(0);
      }
      else
      {
        ERROR_MSG("\"%s\" not known.\n", argv[i]);
        rfac_help(stderr);
        exit(RFAC_INVALID_ARGUMENT);
      }
    
    }   /* if argv[...] == '-') */
    else
    {
      ERROR_MSG("\"%s\"\n", argv[i]);
      rfac_help(stderr);
      exit(RFAC_INVALID_ARGUMENT);
    }
  }    /* for i */

  if (args->ctr_file == NULL)
  {
    ERROR_MSG("No control file specified\n     use option -c to specify\n");
    exit(RFAC_INVALID_ARGUMENT);
  }

  return (args);
}
