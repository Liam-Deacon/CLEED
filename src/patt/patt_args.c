#include <stdio.h>
#include <strings.h>
#include "patt.h"

int parse_patt_args(int argc, char *argv[], 
                    drawing_t *drawing, int *format,
                    char *output_filename)
{

  int i_arg, ii;
  
  if (argc < 2)
  {
    fprintf(stderr, "\nsyntax error:\n");
    usage(stderr);
    exit(1);
  }
 
  for (i_arg = 1; i_arg < argc; i_arg++)
  {
    if (*argv[i_arg] != '-')
    {
      fprintf(stderr, "\nsyntax error:\n");
      usage(stderr);
      exit(1);
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
 		  strcpy(drawing->title.text, argv[i_arg]);
 		  strcat(drawing->title.text, "\0");
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
      }
 /* Set Electron Energy */
      if((strcmp(argv[i_arg], "--energy") == 0) ||
 	     (strcmp(argv[i_arg], "-e") == 0))
      {
        i_arg++;
        strcpy(drawing->eV.text, argv[i_arg]); 
        strcat(drawing->eV.text, '\0');
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
      }
 /* Clip spots to edge of MAX_RADIUS */
      if((strcmp(argv[i_arg], "--clip") == 0) ||
 	     (strcmp(argv[i_arg], "-c") == 0))
      {
        drawing->screen.clip = 1;
      }
 /* Draw k-space vectors */
      if(strcmp(argv[i_arg], "--vectors") == 0)
      {
        drawing->show_vectors = 1; 
        ii=-1;
        while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
        {
 		  i_arg++;
          if (ii < 0) 
 		    decode_vectors(SPOT_GS, drawing->show_gs_vectors, argv[i_arg], 
                           strlen(argv[i_arg]));
          else
            decode_vectors(SPOT_SS, drawing->show_ss_vectors[ii],
                           argv[i_arg], strlen(argv[i_arg]));
          ii++;
        }
      }
 /* Draw a 'screen' */
      if((strcmp(argv[i_arg], "--screen") == 0) ||
         (strcmp(argv[i_arg], "-s") == 0))
      {
        drawing->screen.visible = 1;
      }
 /* Draw a simulated e-gun */
      if((strcmp(argv[i_arg], "--gun") == 0) ||
 	     (strcmp(argv[i_arg], "-g") == 0))
      {
        drawing->gun_flag.visible = 1;
        if (i_arg + 1 < argc)
          if(isdigit(argv[i_arg+1][1]))
          {
            i_arg++;
            drawing->gun.angle = atof(argv[i_arg]);
            if (drawing->gun.angle > 2*M_PI)
            {
            /* assume degrees */
              if (drawing->gun.angle > 360.)
                drawing->gun.angle = (int)(drawing->gun.angle) % 360;
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
      }
 /* Do not fill spots or screen */
      if(strcmp(argv[i_arg], "--no-fill") == 0)
      {
        drawing->screen.fill = 0;
        drawing->fill_gs = 0;
        drawing->fill_ss = 0;
      }
 /* Fill GS spots */
      if(strcmp(argv[i_arg], "--gs-fill") == 0)
      {
        drawing->fill_gs = 1;
      }
 /* Fill SS spots */
      if(strcmp(argv[i_arg], "--ss-fill") == 0)
      {
        drawing->fill_ss = 1;
      }
 /* Fill 'screen' */
      if(strcmp(argv[i_arg], "--screen-fill") == 0)
      {
        drawing->screen.fill = 1;
      }
 /* Allow drawing of SS spots over GS spots */
      if(strcmp(argv[i_arg], "--overlap") == 0)
      {
        drawing->show_overlap = 1;
      }
 /* Use color */
      if(strcmp(argv[i_arg], "--rgb") == 0)
      {
        drawing->color = PATT_COLOR;
      }
 /* Use greyscale */
      if((strcmp(argv[i_arg], "--greyscale") == 0) ||
         (strcmp(argv[i_arg], "--grayscale") == 0))
      {
        drawing->color = PATT_GRAYSCALE;
      }
 /* Use black only */
      if(strcmp(argv[i_arg], "--black") == 0)
      {
        drawing->color = PATT_BLACK_ONLY;
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
            fprintf(stderr,
                "***warning (patt): too many input files (max = %i) -",
                "extra files will be ignored...\n", MAX_INPUT_FILES);
          } 
          else
          {
            if ((f = fopen(argv[i_arg], "r")) == NULL)
            {
              fprintf(stderr, "***error (patt): failed to open '%s'\n", 
                      argv[i_arg]);
              exit(1);
            }
            fclose(f);
            strcpy(drawing->input_files[drawing->n_files], argv[i_arg]);
            strcat(drawing->input_files[drawing->n_files], '\0');
            drawing->n_files += 1;
            
          }
          i_arg++;
        }
      }
 /* Open output file */
      if( (strcmp(argv[i_arg], "-o") == 0) ||
          (strcmp(argv[i_arg], "--output") == 0))
      {
        i_arg++;
        if (out_stream != stdout)
 		  fclose(out_stream); 
        strcpy(output_filename, argv[i_arg]);
        if ((out_stream = fopen(argv[i_arg], "w")) == NULL)
        {
          fprintf(stderr, "***error (patt): failed to open '%s'\n", 
                  argv[i_arg]);
          exit(1);
        }
      }
 /* Do not print indices */
      if(strcmp(argv[i_arg], "-ni") == 0)
      {
        drawing->show_indices = 0;
      }
 /* Only print GS spot indices */
      if((strcmp(argv[i_arg], "-gi") == 0) ||
         (strcmp(argv[i_arg], "--index-gs") == 0))
      {
        drawing->show_indices = 1;
        for (ii=0;ii<MAX_INPUT_FILES;ii++)
        {
          drawing->show_gs_indices[ii] = 1;
          drawing->show_ss_indices[ii] = 0;
        }
      }
 /* Only print SS spot indices */
      if((strcmp(argv[i_arg], "-si") == 0) ||
         (strcmp(argv[i_arg], "--index-ss") == 0))
      {
        index = 1;
        for (ii=0;ii<MAX_INPUT_FILES;ii++)
        {
 		  drawing->show_gs_indices[ii] = 0;
 		  drawing->show_ss_indices[ii] = 1;
        }
      }
 /* print specified indicies */
      if(strcmp(argv[i_arg], "--index") == 0)
      {
        drawing->show_indices = 1; 
        for (ii=0;ii<MAX_INPUT_FILES;ii++)
        {
 		  drawing->show_gs_indices[ii] = 0;
          drawing->show_ss_indices[ii] = 0;
        }	
      ii = -1;
      while ((i_arg+1 < argc) && (*argv[i_arg+1] != '-'))
 	  {
        i_arg++;
        if (ii < 0) 
 		  decode_vectors(SPOT_GS, drawing->show_gs_indices,
                         argv[i_arg], strlen(argv[i_arg]));
 	    else
          decode_vectors(SPOT_GS, drawing->show_ss_indices,
                         argv[i_arg], strlen(argv[i_arg]));
 	  ii++;
 	}
    }
 /* Do not use different symbols */
    if(strcmp(argv[i_arg], "-ns") == 0)
    {
      drawing->symbols = 0;
    }
 /* Radius of GS spots */
    if(strcmp(argv[i_arg], "-rg") == 0)
    {
      i_arg++;
      drawing->gs_spots.radius = (float)atof(argv[i_arg]);
 	  drawing->gs_spots.flag = 1;
    }
 /* Radius of SS spots */
    if(strcmp(argv[i_arg], "-rs") == 0)
    {
     i_arg++;
     drawing->ss_spots.radius = (float)atof(argv[i_arg]);
 	 drawing->gs_spots.flag = 1;
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
         drawing->footnote.text = "";
         strcat(drawing->footnote.text, argv[i_arg]);
         strcat(drawing->footnote.text, "\0");
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

             
         #ifdef _USE_CAIRO
         
         #ifdef CAIRO_HAS_PS_SURFACE
         if (strcasecmp(argv[i_arg], "ps") == 0) 
             *format = PATT_PS;
         #else
             *format = PATT_PS_OLD;
         #endif
         #ifdef CAIRO_HAS_IMAGE_SURFACE
         else if (strcasecmp(argv[i_arg], "png") == 0) 
             *format = PATT_PNG;
         #ifdef CAIRO_HAS_PDF_SURFACE
         else if (strcasecmp(argv[i_arg], "pdf") == 0) 
             *format = PATT_PDF;
         #endif
         #ifdef CAIRO_HAS_SVG_SURFACE
         else if (strcasecmp(argv[i_arg], "svg") == 0) 
             *format = PATT_SVG;    
         #endif
         
         #endif
         
         else
         {
           fprintf(stderr, "***error (patt): "
                   "'%s' format is either not supported or is invalid - "
                   "defaulting to 'ps'\n");
           *format = PATT_PS_OLD;
         }
       }
     }
 /* Help */
    if((strcmp(argv[i_arg], "-h") == 0) ||
 	  (strcmp(argv[i_arg], "--help") == 0))
    {
      patt_usage(stdout);
      exit(0);
    }
 /* Print PATTERN information to stdout */
    if((strcmp(argv[i_arg], "--version") == 0) ||
       (strcmp(argv[i_arg], "-V") == 0))
    {
 	 patt_info();
 	 exit(0);
    }
   } /* else */
  }  /* for i_arg */
  
  /* check footnote string */
  if (drawing->footnote.text == NULL || 
      strlen(drawing->footnote->text) == 0)
  {
    strcpy(drawing->footnote->text, VERSION);
  }
  
  return 0;
}