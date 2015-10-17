/*********************************************************************
 *                           PATT_ARGS.C
 *
 *  COpyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/2014.08.23 - split from patt.c
 *********************************************************************/

/*! \file
 *
 * Parses command line arguments for the \c patt program.
 */

#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include "patt.h"
#include "pattern.h"

/*!
 * Initializes \p drawing for the LEED pattern to the specified command line
 * arguments.
 *
 * \param argc Number of command line arguments in \p argv
 * \param[in] argv Array of command line argument strings to parse.
 * \param[in,out] drawing
 * \return #patt_error
 * \retval #PATT_SUCCESS on successful completion.
 * \note Exits with a #patt_error code other than #PATT_SUCCESS if errors occur.
 */
int patt_args(int argc, char *argv[], patt_drawing *drawing)
{

  int i_arg, ii, jj;
  FILE *f;

  if (drawing == NULL)
  {
    ;//if ((drawing = patt_drawing_init()) == NULL) exit(PATT_ALLOC_ERROR);
  }
  drawing->std_in = true;
  drawing->std_out = true;

  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if (*argv[i_arg] != '-')
    {
      fprintf(stderr, "\nsyntax error:\n");
      patt_usage(stderr);
      exit(PATT_ARGUMENT_ERROR);
    }
    else
    {

      /* Title */
      if((strcmp(argv[i_arg], "-t") == 0) ||
 	     (strcmp(argv[i_arg], "--title") == 0))
      {
        if (i_arg+1 < argc)
        {
          i_arg++;
          strcpy(drawing->title.label, argv[i_arg]);
          strcat(drawing->title.label, "\0");
        }

        if (i_arg+1 < argc)
          if (isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->title.x = atof(argv[i_arg]);
          }

 	      if (i_arg+1 < argc)
          if (isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->title.y = atof(argv[i_arg]);
          }
      } /* title */

      /* Set Electron Energy */
      if((strcmp(argv[i_arg], "--energy") == 0) ||
 	     (strcmp(argv[i_arg], "-e") == 0))
      {
        i_arg++;
        strcpy(drawing->eV.label, argv[i_arg]);
        strcat(drawing->eV.label, "\0");
        drawing->energy = 1;
        if (i_arg+1 < argc)
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->eV.x = atof(argv[i_arg]);
          }
        if (i_arg+1 < argc)
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->eV.y = atof(argv[i_arg]);
          }
      } /* energy */

      /* Clip spots to edge of MAX_RADIUS */
      if((strcmp(argv[i_arg], "--clip") == 0) ||
 	     (strcmp(argv[i_arg], "-c") == 0))
      {
        drawing->screen.clip = true;
      }

      /* Draw k-space vectors */
      if(strcmp(argv[i_arg], "--vectors") == 0)
      {
        drawing->show_vectors = true;
        ii=-1;
        while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
        {
          i_arg++;
          if (ii < 0)
          {
            decode_vectors(SPOT_GS, drawing->show_gs_vectors, argv[i_arg],
                       strlen(argv[i_arg]));
          }
          else
          {
            decode_vectors(SPOT_SS, drawing->show_ss_vectors[ii],
                           argv[i_arg], strlen(argv[i_arg]));
          }
          ii++;
        }
      } /* vectors */

      /* Draw a 'screen' */
      if((strcmp(argv[i_arg], "--screen") == 0) ||
         (strcmp(argv[i_arg], "-s") == 0))
      {
        drawing->screen.visible = true;
      }

      /* Draw a simulated e-gun */
      if((strcmp(argv[i_arg], "--gun") == 0) ||
 	     (strcmp(argv[i_arg], "-g") == 0))
      {
        drawing->gun.visible = 1;
        if (i_arg + 1 < argc)
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->gun.angle = atof(argv[i_arg]);
            if (drawing->gun.angle > 2*M_PI)
            {
              /* assume degrees */
              if (drawing->gun.angle > 360.)
              {
                drawing->gun.angle = (int)(drawing->gun.angle) % 360;
              }
              drawing->gun.angle *= M_PI/180;
            }
          }
        if (i_arg + 1 < argc)
        {
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->gun.radius = atof(argv[i_arg]);
          }
        }
      } /* e-gun */

      /* Do not fill spots or screen */
      if(strcmp(argv[i_arg], "--no-fill") == 0)
      {
        drawing->screen.fill = false;
        drawing->fill_gs = false;
        drawing->fill_ss = false;
      }

      /* Fill GS spots */
      if(strcmp(argv[i_arg], "--gs-fill") == 0)
      {
        drawing->fill_gs = true;
      }

      /* Fill SS spots */
      if(strcmp(argv[i_arg], "--ss-fill") == 0)
      {
        drawing->fill_ss = true;
      }

      /* Fill 'screen' */
      if(strcmp(argv[i_arg], "--screen-fill") == 0)
      {
        drawing->screen.fill = true;
      }

      /* Allow drawing of SS spots over GS spots */
      if(strcmp(argv[i_arg], "--overlap") == 0)
      {
        drawing->show_overlap = true;
      }

      /* Use color */
      if(strcmp(argv[i_arg], "--rgb") == 0)
      {
        drawing->color_scheme = PATT_COLOR_SCHEME;
      }

      /* Use greyscale */
      if((strcmp(argv[i_arg], "--greyscale") == 0) ||
         (strcmp(argv[i_arg], "--grayscale") == 0))
      {
        drawing->color_scheme = PATT_GRAYSCALE_SCHEME;
      }

      /* Use black only */
      if(strcmp(argv[i_arg], "--black") == 0)
      {
        drawing->color_scheme = PATT_MONOCHROME_SCHEME;
      }

      /* Open input file(s) */
      if( (strcmp(argv[i_arg], "-i") == 0) ||
          (strcmp(argv[i_arg], "--input") == 0))
      {
        i_arg++;
        drawing->n_files = 0;
        while ((strncmp(argv[i_arg], "-", 1)) && (i_arg < argc))
        {
          if (drawing->n_files >= MAX_INPUT_FILES) 
          {
            WARNING_MSG("too many input files (max = %i) -"
                    "extra files will be ignored...\n", MAX_INPUT_FILES);
          } 
          else
          {
            if ((f = fopen(argv[i_arg], "r")) == NULL)
            {
              fprintf(stderr, "***error (patt): failed to open '%s'\n", 
                      argv[i_arg]);
              exit(PATT_READ_ERROR);
            }
            fclose(f);
            strcpy(drawing->input_files[drawing->n_files], argv[i_arg]);
            strcat(drawing->input_files[drawing->n_files], "\0");
            drawing->n_files += 1;
            
          }
          i_arg++;
          drawing->std_in = false;
        }
      }

      /* Open output file */
      if( (strcmp(argv[i_arg], "-o") == 0) ||
          (strcmp(argv[i_arg], "--output") == 0))
      {
        i_arg++;
        strcpy(drawing->output_filename, argv[i_arg]);
        if ((f = fopen(argv[i_arg], "w")) == NULL)
        {
          ERROR_MSG("failed to open '%s'\n", argv[i_arg]);
          exit(PATT_WRITE_ERROR);
        }
        fclose(f);
        drawing->std_out = false;
      }

      /* Do not print indices */
      if(strcmp(argv[i_arg], "-ni") == 0)
      {
        drawing->show_indices = false;
      }

      /* Only print GS spot indices */
      if((strcmp(argv[i_arg], "-gi") == 0) ||
         (strcmp(argv[i_arg], "--index-gs") == 0))
      {
        drawing->show_indices = true;
        for (ii=0; ii<MAX_INPUT_FILES; ii++)
        {
          drawing->show_gs_indices[ii] = true;
          for (jj=0; jj<MAX_DOMAINS; jj++)
          {
            drawing->show_ss_indices[ii][jj] = false;
          }
        }
      }

      /* Only print SS spot indices */
      if((strcmp(argv[i_arg], "-si") == 0) ||
         (strcmp(argv[i_arg], "--index-ss") == 0))
      {
        for (ii=0; ii<MAX_INPUT_FILES; ii++)
        {
          drawing->show_gs_indices[ii] = false;
          for (jj=0; jj<MAX_DOMAINS; jj++)
          {
            drawing->show_ss_indices[ii][jj] = true;
          }
        }
      }

      /* print specified indicies */
      if(strcmp(argv[i_arg], "--index") == 0)
      {
        drawing->show_indices = true;
        for (ii=0; ii<MAX_INPUT_FILES; ii++)
        {
          drawing->show_gs_indices[ii] = false;
          for (jj=0; jj<MAX_DOMAINS; jj++)
          {
            drawing->show_ss_indices[ii][jj] = false;
          }
        }

        ii = -1;
        while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
        {
          i_arg++;
          if (ii < 0)
          {
            decode_vectors(SPOT_GS, drawing->show_gs_indices,
                           argv[i_arg], strlen(argv[i_arg]));
          }
          else
          {
            for (jj=0; jj<MAX_DOMAINS; jj++)
            {
              decode_vectors(SPOT_GS, drawing->show_ss_indices[jj],
                             argv[i_arg], strlen(argv[i_arg]));
            }
          }
          ii++;
        } /* while */
      } /* index */

    /* Do not use different symbols */
    if(strcmp(argv[i_arg], "-ns") == 0)
    {
      drawing->symbols = false;
    }

    /* Radius of GS spots */
    if(strcmp(argv[i_arg], "-rg") == 0)
    {
      i_arg++;
      drawing->spots[SPOT_GS].radius = (float)atof(argv[i_arg]);
    }

    /* Radius of SS spots */
    if(strcmp(argv[i_arg], "-rs") == 0)
    {
      i_arg++;
      drawing->spots[SPOT_SS].radius = (float)atof(argv[i_arg]);
    }

    /* Custom Footer */
     if ((strcmp(argv[i_arg], "-fn") == 0) ||
         (strcmp(argv[i_arg], "--footer") == 0) ||
         (strcmp(argv[i_arg], "--header") == 0))
     {
       if (strcmp(argv[i_arg], "--header") == 0)
       {
         drawing->footnote.x = -MAX_RADIUS;
         drawing->footnote.y = 400;
       }
 	  
       if (i_arg+1 < argc)
       {
         i_arg++;
         strcpy(drawing->footnote.label, "");
         strcat(drawing->footnote.label, argv[i_arg]);
         strcat(drawing->footnote.label, "\0");
       }
 	  
       if(i_arg+1 < argc)
       {
         if(isdigit(argv[i_arg+1][1]))
         {
           i_arg++;
           drawing->footnote.x = atof(argv[i_arg]);
         }
       }

       if(i_arg+1 < argc)
       {
         if(isdigit(argv[i_arg+1][1]))
         {
           i_arg++;
           drawing->footnote.y = atof(argv[i_arg]);
         }
       }

     }

     /* Output format */
     if ((strcmp(argv[i_arg], "-f") == 0) ||
         (strcmp(argv[i_arg], "--format") == 0))
     {
       if (i_arg+1 < argc)
       {

         #if USE_CAIRO
         
         drawing->format = PATT_UNKNOWN_FORMAT;

         #ifdef CAIRO_HAS_PS_SURFACE
           if (strcasecmp(argv[i_arg], "ps") == 0) drawing->format = PATT_PS;
         #else
           drawing->format = PATT_PS_OLD;
         #endif /* CAIRO_HAS_PS_SURFACE */

         #ifdef CAIRO_HAS_IMAGE_SURFACE
           if (strcasecmp(argv[i_arg], "png") == 0) drawing->format = PATT_PNG;
         #endif /* CAIRO_HAS_IMAGE_SURFACE */

         #ifdef CAIRO_HAS_PDF_SURFACE
           if (strcasecmp(argv[i_arg], "pdf") == 0) drawing->format = PATT_PDF;
         #endif /* CAIRO_HAS_PDF_SURFACE */

         #ifdef CAIRO_HAS_SVG_SURFACE
           if (strcasecmp(argv[i_arg], "svg") == 0) drawing->format = PATT_SVG;
         #endif /* CAIRO_HAS_SVG_SURFACE */
         
         #endif /* USE_CAIRO */
         
         if (drawing->format == PATT_UNKNOWN_FORMAT)
         {
           ERROR_MSG("'%s' format is either not supported or is invalid - "
                     "defaulting to 'ps'\n", argv[i_arg+1]);
           drawing->format = PATT_PS_OLD;
         }
       } /* < argc */
     }

     /* Help */
    if((strcmp(argv[i_arg], "-h") == 0) ||
       (strcmp(argv[i_arg], "--help") == 0))
    {
      patt_usage(stdout);
      exit(PATT_SUCCESS);
    }

    /* Print PATTERN information to stdout */
    if((strcmp(argv[i_arg], "--version") == 0) ||
       (strcmp(argv[i_arg], "-V") == 0))
    {
      patt_info();
      exit(PATT_SUCCESS);
    }

    if ((strcmp(argv[i_arg], "-I") == 0) ||
        (strcmp(argv[i_arg], "--interactive") == 0))
    {
      drawing->interactive = true;
    }
  } /* else */

  }  /* for i_arg */
  
  /* check footnote string */
  if ((drawing->footnote.label == NULL) ||
      (strlen(drawing->footnote.label) == 0))
  {
    strcpy(drawing->footnote.label, PATT_VERSION);
  }
  
  return(PATT_SUCCESS);
}
