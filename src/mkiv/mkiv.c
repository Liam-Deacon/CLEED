/*********************************************************************
 *                             MKIV.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   SU/00.03.92
 *   GH/22.07.92
 *   GH/25.08.92
 *   CS/29.07.93
 *   CS/23.08.93
 *   VJ/28.01.03
 *   GH/11.03.03
 *   LD/28.02.14
 *********************************************************************/

/*! \file
 *
 * Main entry into the \c mkiv program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>

#include "tiffio.h"
#include "mkiv.h"

#ifdef USE_OPENMP
#include <omp.h>           /* for parallel processing */
#endif 


/***************************************************************************
 * mkiv evaluates LEED-images

 Options:
  -i --input        : followed by the prefix to the input files [default='mkiv']
  -b --current      : followed by the beam current value
  -B --beam         : followed by the prefix for .raw and .smo files [default='beam']
  -c --change-input : allow changes of mkiv.inp and mkiv.var
  -h --help         : print the IV_READ_ME help file
  -m --mask         : path to mask file [default='mkiv.byte']
  -M --make-mask    : produce mask and save to file [default='mask.byte']
  -o --output       : output file path [default='mkiv.ivdat']
  -p --param        : path to param file [default='mkiv.param']
  -P --pos          : path to pos file [default='mkiv.pos']
  -s --save-images  : save intermediate images
  -q --quiet        : faster, no graphical output, only few printf's
  -v --verbose      : give more detailed information during computations
  -V --version      : give version and other information

 Input files:
  mkiv.inp
  mkiv.var
  mkiv.pos
  LEED-images
  LEED-screen mask

 Output files:
  <fname>.ivdat, mkiv.ivdat 	   notice that fname may include path   
  <fname>.param, mkiv.param
  beam.raw 
  beam.smo
***************************************************************************/

#if (BUILD_LIBRARY == 1)
int mkiv_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  mkiv_image *mat_image = NULL;  /* resulting LEED-image as a matrix     */
  mkiv_image *mat_mask = NULL;   /* mask that defines visible LEED-screen*/

  mkiv_tif_values *tif_image = NULL;  /* resulting LEED-image in TIFF-format  */
  mkiv_tif_values *tif_mask = NULL;   /* mask that defines visible LEED-screen*/

  double radius;  /* bounds of visible screen             */

  size_t numb;                      /* run through number for subsequent    */
                                    /* leed image data files                */
  int numb_2;                       /* run through display of images        */
  int n_show;                       /* process images                       */
  int n_show_2;                     /* show image after each n_show_2 images*/

  char *linebuffer;

  char *fname2;
  char fname[STRSZ] = "\0";         /* filename of first leed image         */

  double kappa;                   /* comp. factor sqrt(en_old/energy)     */
  double energy, en_old, en_ref;  /* current, former and ref. beam energy */

  mkiv_vector a[3], newa[3];     /* a[0] = origin = (0,0)-mkiv_reflex         */
  mkiv_vector da[3];             /* a[1],a[2] = rec. lattice vectors     */
                                 /* vector.(len,xx,yy)                   */

  size_t n_spot, spot_max;        /* number of used, max array-members    */
  mkiv_reflex *spot;                  /* list of measurable reflexes          */
                                 /* spot.(kp_len,lind1/2,x0,y0,xx,yy,    */
                                 /*    cos_th,intensity,s2u,s2n,control) */

  double inty[I_MAX];             /* intensity values interim storage     */

  double range, rel_range;        /* range for maximum search             */
  mkiv_vector scale, rel_scale;  /* scaling factors for integration area */
  double angle;                   /*angle between the broadening direction*/
                                 /* of the reflexes and horizontal axis  */
  double int_norm[E_MAX];         /* beam current                         */
  double use_cur;                 /* beam current currently used          */

  /* auxiliaries */
  size_t i, nnref;
  int k;
  int flag;

  /* inputs from command line: */

  /* open statements */
  FILE *cur_stream, *smcur_stream, *iv_stream;

  /* initialise strings */
  fname[0] = '\0';
  linebuffer = (char *)malloc(sizeof(char) * STRSZ);
  linebuffer[0] = '\0';

  /* MAIN BEFORE ENERGY - LOOP */

  /* parse command line arguments */
  mkiv_args *args = mkiv_get_args(argc, argv);
  mkiv_input *inputs = mkiv_input_read(args);
  mkiv_params *params = mkiv_params_read(args);

  /* backup existing old output files */
  file_backup(args->ivdat_path);
  file_backup(args->beam_raw_path);
  file_backup(args->beam_smoothed_path);

  /* preset variables */
  n_show = n_show_2 = 1;

  /* Allocate memory for mat/tif_image and mat/tif_mask */
  CLEED_ALLOC_CHECK(mat_image = (mkiv_image *)calloc(1, sizeof(mkiv_image)));
  CLEED_ALLOC_CHECK(mat_mask  = (mkiv_image *)calloc(1, sizeof(mkiv_image)));
  
  mat_image->imagedata = mat_mask->imagedata = NULL;

  CLEED_ALLOC_CHECK(tif_image = (mkiv_tif_values *)
                                  calloc(1, sizeof( mkiv_tif_values )));
  CLEED_ALLOC_CHECK(tif_mask  = (mkiv_tif_values *)
                                  calloc(1, sizeof( mkiv_tif_values )));
  tif_image->buf = tif_mask->buf = NULL;

  /* Read mask, if existing, else use r_outer,r_inner for masking */
  if( !file_exists(args->mask_path) )
  {
    fprintf(stdout, "\nusing r_outer, r_inner for masking. \n");
  }
  else
  {
    mkiv_read_tif(tif_mask, args->mask_path);
    fprintf(stdout, "\nusing '%s' for masking. \n", args->mask_path);
  }

  /* Read image */
  mkiv_read_tif(tif_image, fname);

  /* Convert TIFF image (tif_values) into a matrix (mkiv_image) so that
   * MKIV is able to process them */
  mkiv_tif2mat(tif_image, mat_image);
    
  if (args->save_intermediates)
  {
    strcpy(args->mask_path, remove_ext(fname, '.', '/'));
    strcat(args->mask_path, ".byte");
  }
  mkiv_output_tif(mat_image, args->mask_path);

  if(tif_mask != NULL) mkiv_tif2mat(tif_mask, mat_mask);
  else
  {
    free(mat_mask);
    mat_mask = NULL;
  }

  /* Read energy and beam current from file header */

  /*
     en_old = en_ref = (double)(mat_image->ispare1 &0xFFFF)/(mat_image->ispare1 >> 16);
     *int_norm = (double)(mat_image->ispare2 &0xFFFF)/(mat_image->ispare2 >> 16);
   */

  /* !!!!!!!!!!!!!!!!!!!!!!! */
  en_old = en_ref = 100.;
  *int_norm = 100.;
  /* !!!!!!!!!!!!!!!!!!!!!!! */

  /* draw the mask or r_inner, r_outer and the center of the image and display */
  if (args->show_bounds)
    mkiv_draw_bounds(mat_image, mat_mask,
                     &inputs->center, inputs->r_outer,
                     inputs->r_inner, -1, args->mask_path);

  /* only needed to produce a mask: */
  if (args->make_mask)
    mkiv_mk_mask(mat_image, &inputs->center,
                 inputs->r_outer, inputs->r_inner, -1, args->mask_path);

  /* Allocate memory for the input reference spots for the first frame */
  spot = (mkiv_reflex*) malloc(100*sizeof(mkiv_reflex));
  if (spot == NULL)
  {
    ERROR_MSG("memory allocation for mkiv_ref_inp failed\n");
    ERROR_RETURN(-1);
  }

  /* Input of reference spots */
  mkiv_ref_inp(&n_spot, spot, args->verbose, args->pos_path);
  while ( n_spot < 3 ) mkiv_ref_inp(&n_spot, spot, 0, args->pos_path);

  /* Input of reference energy */
  fprintf(stdout, "energy of reference image (in eV):\n");
  linebuffer = fgets(linebuffer, STRSZ, stdin);
  en_old = en_ref = atof(linebuffer);
  fprintf(stdout, "energy = %f eV\n", en_ref);

  /* Compute basic vectors and origin */
  fprintf(stderr, "calculate new base and origin\n");
  mkiv_calc_basis(n_spot, spot,  /* input: no.of and list of spots */
           a,                   /* output: rec.lattice vectors, origin */
           params->ratio,       /* ratio radius/camera distance */
           params->distance/4,  /* min distance for recalibr. spots */
           params->trip_max,    /* max number of triples for recalib */
           -1.);                /* cor_fac<0 : no recorrection, no input-orig.*/

  /* Free allocated memory */
  free(spot);

  /* Allocate memory for struct spot array using highest energy 1000eV */
  mkiv_calc_area(&kappa, &en_old, 1000., a,
        &range, 0., rel_range, &params->scale, 0., rel_scale);
  spot = mkiv_reflex_alloc( &spot_max, a, inputs->n_dom, inputs->superlat, mat_image);
  fprintf(stdout, "allocated memory for %d spots\n", spot_max);

  mkiv_calc_area(&kappa, &en_old, en_ref, a,
        &range, 0., rel_range, &scale, 0., rel_scale);

  fprintf(stdout, "orig: %.0f,%.0f ,", a[0].yy, a[0].xx);
  fprintf(stdout, "a1:%.0f,%.0f ,a2:%.0f,%.0f\n",
                  a[1].yy, a[1].xx, a[2].yy, a[2].xx);

  /* Compute reciprocal lattice and create a table of spots containing:
   * indices, positions, k-value, cos_th */
  fprintf(stdout, "compute reciprocal lattice\n");
  mkiv_calc_spot_positions(a, inputs->n_dom, inputs->superlat, spot_max, &n_spot, spot, inputs->n_excl, inputs->excl,
              mat_mask, inputs->r_outer, 0., &radius, en_old, params->ratio, inputs->kpl_10);
  fprintf(stdout, "New lattice - sin: %6.4f, radius:%7.1f, \n",
          inputs->kpl_10*H2M/sqrt(en_old), radius);
  fprintf(stderr, "spots: %d\n", n_spot);

  /* Set the spot.control bits according to the lists desi,ref */
  fprintf(stderr, "set control according to desi,ref \n");
  if (QQ(args->verbose))
    mkiv_set_control(n_spot, spot,
                     inputs->n_desi, inputs->desi,
                     inputs->n_ref, inputs->ref);

  /* Mark measurable reflexes */
  fprintf(stderr, "draw circles around spots\n");
  mkiv_mark_reflex(n_spot, spot, mat_image,    /* list of spots, image */
              THICK, range, WHITE,       /* width,radius,color of circles*/
              -1, args->mask_path);       /* draw indices & output filename */

  /* ENERGY - LOOP */

  /* Open files "mkiv.ivdat", "beam_raw" and "beam_smo" */
  iv_stream = fopen(args->ivdat_path, "w");
  if (iv_stream == NULL)
  {
    ERROR_MSG("Unable to write to iv data file '%s'\n", args->ivdat_path);
    ERROR_RETURN(-1);
  }
    
  cur_stream = fopen(args->beam_raw_path, "w");
  if (cur_stream == NULL)
  {
    ERROR_MSG("Unable to write to raw beam file '%s'\n", args->beam_raw_path);
    ERROR_RETURN(-1);
  }
    
  smcur_stream = fopen(args->beam_smoothed_path, "w");
  if (smcur_stream == NULL)
  {
    ERROR_MSG("Unable to write to smoothed beam file '%s'\n", args->beam_smoothed_path);
    ERROR_RETURN(-1);
  }
 
  /* Make list header */
  fprintf(iv_stream, " h     ");
  for (i=0; i < inputs->n_desi; i++)
  {
    fprintf(iv_stream, "%10.2f", inputs->desi[i].lind1);
  }
  fprintf(iv_stream, "\n k     ");
  for (i=0; i < inputs->n_desi; i++)
  {
    fprintf(iv_stream, "%10.2f", inputs->desi[i].lind2);
  }
  fprintf(iv_stream, "\n nenergy\n");

  /* Loop for data files */
  fprintf(stderr, "n_start = %d, n_stop = %d \n", inputs->n_start, inputs->n_stop);
    
  /* int i_e_step; */
  /* i_e_step = (int)e_step; */
    
  numb_2 = inputs->n_start;
  #ifdef USE_OPENMP

  /* Default to maximum parallization if OMP_NUM_THREADS environment variable
   * is not set (this bahavior can be removed if OMP_DEFAULT_IS_SERIAL is
   * defined when compiling) */
  #ifndef OMP_DEFAULT_IS_SERIAL
  if ( getenv(OMP_NUM_THREADS) == NULL )
    omp_set_num_threads( omp_get_max_threads() );
  #endif /* OMP_DEFAULT_IS_SERIAL */

  #pragma omp parallel for
  #endif /* _USE_OPNEMP */
  for ( numb = inputs->n_start;
        ( (numb <= inputs->n_stop) && (inputs->e_step > 0) ) ||
        ( (numb >= inputs->n_stop) && (inputs->e_step < 0) );
        numb += (size_t)inputs->e_step)  /* note the dodgy cast from double to int */
  {
    /* Read image and convert tif_values to mkiv_image */
    fname2 = filename(fname, numb);
    fprintf(stderr, "read data from file \"%s\" and convert \n",
            fname2);

    mkiv_read_tif(tif_image, fname2);
    mkiv_tif2mat(tif_image, mat_image);

    /* Read energy from file header */
    energy = (double)numb;

    /* Compute contraction of new basic vectors, range and scale */
    mkiv_calc_area(&kappa, &en_old, energy, a, &params->range, params->range_min,
                   rel_range, &params->scale, params->scale_min, rel_scale);

    /* use current from file and smooth it when desired */
    int_norm[numb] = 100.;
    use_cur = int_norm[numb];
    if (params->modify_current < 0)
      use_cur = mkiv_bkgnd_smooth(int_norm, numb, inputs->n_start, inputs->e_step, 10);
    if (params->modify_current > 0) use_cur = (double)params->modify_current;

    printf("\n\n");
    if (QQ(args->verbose)) printf("#####################################################\n");
    if (QQ(args->verbose)) printf("\t\t FILE NAME: %s\n", fname);
    printf("\t\t ENERGY: %6.1f eV \n", energy);
    if (QQ(args->verbose)) printf("\t\t CURRENT: %7.1f (%7.1f)nA\n", int_norm[numb], use_cur);
    printf("#####################################################\n");
    printf("New range for Max. search: %.1f pixels.\n", range);
    printf("Elliptical integration area : (%.1f*%.1f) pixels.\n",
           scale.xx, scale.yy);
    if (args->verbose & VERBOSE )
      printf("orig: %.0f,%.0f , a1: %.0f,", a[0].yy, a[0].xx, a[1].yy);
    if (args->verbose & VERBOSE )
      printf("%.0f , a2: %.0f,%.0f \n", a[1].xx, a[2].yy, a[2].xx);

    /* Compute new reciprocal lattice */
    mkiv_calc_spot_positions(a, inputs->n_dom, inputs->superlat,
                             spot_max, &n_spot, spot,
                             inputs->n_excl, inputs->excl,
                             mat_mask, inputs->r_outer+params->range,
                             inputs->r_inner-params->range,
                             &radius, energy, params->ratio, inputs->kpl_10);
    if (QQ(args->verbose)) printf("New lattice : sin: %6.4f, radius:%7.1f, \n",
                                  inputs->kpl_10 * H2M/sqrt(energy), radius);

    /* Sort the table in increasing order of k_par */
    if (QQ(args->verbose)) mkiv_quicksort(spot, spot+n_spot-1);

    /* Set the spot.control bits according to the lists desi,ref */
    mkiv_set_control(n_spot, spot,
                     inputs->n_desi, inputs->desi,
                     inputs->n_ref, inputs->ref);

    /* Find spot maxima */
    printf("%d spots measurable. \n", n_spot);
    if (QQ(args->verbose))
    {
      printf("    Max. search for: \n");
      for (i=0; i < n_spot; i++)
      {
        printf("(%5.2f,%5.2f)%1" PRId64 "; ",
                spot[i].lind1, spot[i].lind2, spot[i].control);
      }
      printf("\n");
    }
        
    /* For spots not found in list excl, the maximum is determined */
    if (args->verbose & VERBOSE ) printf("find maximum \n");
    mkiv_find_imax(n_spot, spot,       /* number and list of measurable spots */
           params->step,              /* step size for max search */
           params->range, mat_image); /* range for max search, LEED image */

    /* Find the exact positions
       (i.e. center of gravity method) of all wanted spots*/
    if (args->verbose & VERBOSE ) printf("calculate center of intensity \n");
    mkiv_calc_spot_disc(n_spot, spot,       /* number and list of measurable spots */
           range, mat_image); /* range for cog search, LEED image */

    /* Compute total intensity of the reflexes */
    if(inputs->bkgnd <= 1)
    {
      ERROR_MSG("no more implemented background!\n");
      ERROR_RETURN(-1);
    }

    if (args->verbose & VERBOSE ) printf("get intensities of spots \n");
    mkiv_calc_intensities(
            n_spot, spot,         /* number and list of measurable spots */
            mat_image, mat_mask, /* LEED image, mask */
            &params->scale,              /* half axes of the ellip. integr. area*/
            params->angle,               /* angle of int.area versus horizontal */
            use_cur,             /* primary beam current (normalization)*/
            inputs->bkgnd-1,                /* (1,!=1)=(y/n)background subtraction */
            params->s2n_good, args->verbose,      /* if s2n < s2n_good -> bad spot */
            params->verh, params->acci, params->accb);   /* integr. area ratios */

    /* For spots with bad s/n-ratio, use calculated position */
    fprintf(stderr,"spots with bad s/n again : \n");
    for( i=0; i < n_spot; i++)
    {
      if (PYTH(spot[i].xx - spot[i].x0, spot[i].yy - spot[i].y0)
            < params->range/params->sec_range)
      {
        spot[i].control |= SPOT_GOOD_S2N;
      }
      if ( spot[i].control & SPOT_GOOD_S2N ) continue;
      spot[i].xx = spot[i].x0;
      spot[i].yy = spot[i].y0;
    }

    /* Second run for spots with bad s/n : use range/sec_range, step=1 */
    mkiv_find_imax(n_spot, spot, 1, params->range/params->sec_range, mat_image);
    mkiv_calc_spot_disc(n_spot, spot, params->range, mat_image);
    mkiv_calc_intensities(n_spot, spot, mat_image, mat_mask, &params->scale, params->angle,
            use_cur, inputs->bkgnd-1, params->s2n_bad, args->verbose,
            params->verh, params->acci, params->accb);

    /* Draw integration area boundaries into image */
    if(numb % n_show == 0)
    {
      mkiv_draw_bounds(mat_image, mat_mask, &inputs->center,
                      inputs->r_outer, inputs->r_inner, -1, args->mask_path);
      mkiv_draw_ellipse(n_spot, spot, mat_image, &params->scale, params->angle, params->verh);

      mkiv_mark_reflex(n_spot, spot, mat_image, /* list of spots, image */
                  THICK, range, WHITE,    /* width,radius,color of circles */
                  0, "mask.byte");              /* don't draw indices */
      numb_2++;
      /* if(numb_2 % n_show_2 == 0) XV;  // antiquated xv command */
    }

    /* Save intensity values */
    fprintf(stderr, "Save intensity values:\n");
    for (i=0; i < inputs->n_desi; i++)
    {
      k = inputs->desi[i].control;
      if (k == -1) inty[i] = INT_OUT;
      else inty[i] = spot[k].intensity;
    }

    /* Calculate basis with true spot positions */
    fprintf(stderr, "Calculate basis:\n");
    nnref = 0;
    for (i=0; i < n_spot; i++)
    {
      if (spot[i].s2n < params->s2n_ref ||
          spot[i].cos_th < params->cos_min || spot[i].cos_th > params->cos_max)
      {
        spot[i--] = spot[--n_spot];
      }
      else if (spot[i].control & SPOT_REF) nnref++;
    }

    /* not enough satisfying spots found -> no recalibration possible */
    if(n_spot < 3) printf("No basis recalibration possible.\n");
    else /* use reference spots */
    {
      if( nnref >= params->ref_min )
      {
        printf("Basis recalibration, S/N > %.2f ", params->s2n_ref);
        printf("(%d predefined spots)\n", nnref);
        for(i=0; i<n_spot; i++)
        {
          if( !( spot[i].control & SPOT_REF ) ) spot[i--] = spot[--n_spot];
        }
      }

      /* not enough predef. spots with satisfying s2n ratio -> use most intense
       * spots for recalibration */
      else printf("Basis recalibration, S/N > %.2f (%d spots)\n",
                  params->s2n_ref, n_spot);

      /* enough satisfying spots found -> recalibration */
      newa[0] = a[0];
      i = mkiv_calc_basis(n_spot, spot, /* input: reference spots */
                   newa,                /* output: basis, input+output: origin */
                   params->ratio,       /* ratio radius/distance  */
                   params->distance,    /* min distance for recalibr. spots */
                   params->trip_max,    /* max number of triples for recalib */
                   0.);                 /* recorrect with cos_th  */
           
      /* Allow  delta = sqrt( fabs(en_old - energy) ) * range / 4.
       * as variation of the origin and the lattice vectors */

      if (!i) printf("Sorry, no triples for recalibration found.\n");
      else
      {
        for (k=0; k <= 2; k++)
        {
          da[k].xx = newa[k].xx - a[k].xx;
          da[k].yy = newa[k].yy - a[k].yy;
          da[k].len = PYTH( da[k].xx, da[k].yy );
        }
        if (da[0].len > params->ref_dev ||
            da[1].len > params->ref_dev ||
            da[2].len > params->ref_dev)
        {
          flag = DEV_BIG;
        }
        else flag = DEV_SMALL;
        if (QQ(args->verbose)) printf("Used %d triples of spots for recalibration.\n",i);
        printf("orig:(%.0f,%.0f); ", da[0].yy, da[0].xx);
        printf("a1:(%.0f,%.0f); ", da[1].yy, da[1].xx);
        printf("a2:(%.0f,%.0f);\n", da[2].yy, da[2].xx);

        /* take the new values for the origin and the basic lattice vectors */
        for (i=0; i<3; i++)
        {
          a[i].xx = ( 1. - params->bas_rat ) * a[i].xx + params->bas_rat * newa[i].xx;
          a[i].yy = ( 1. - params->bas_rat ) * a[i].yy + params->bas_rat * newa[i].yy;
          a[i].len = PYTH( a[i].xx , a[i].yy );
        }
      }  /* if i ... */
    }  /* if nnref ... */

    /* if the deviation between old and new a[] is too big, repeat frame */
    if (flag == DEV_BIG && i && ++params->repetitions < 11)
    {
      numb -= inputs->e_step;
      printf("same frame again because of too big base/orig-deviation\n");
    }
    /* update files args->beam_raw_path, args->beam_smoothed_path and args->ivdat_path" */
    else
    {
      params->repetitions = 0;
      if (args->verbose & VERBOSE)
        printf("updating output ivdat and beam.cur files... \n");
      fprintf(cur_stream, "%f\t%f\n", energy, int_norm[numb]);
      fprintf(smcur_stream, "%f\t%f\n", energy, use_cur);
      fprintf(iv_stream, "%6.1f", energy);
      for (i=0; i < inputs->n_desi; i++ ) fprintf(iv_stream, " %10.5f", inty[i]);
      fprintf(iv_stream, "\n");
      if (QQ(args->verbose)) fflush(NULL);       /* flush all buffered output files */
      else if ((inputs->n_stop-numb) % params->update_freq == 0) fflush(NULL);
    }

  }
  /* END OF ENERGY - LOOP */

  /* close output files */
  fclose(iv_stream);
  fclose(cur_stream);

  /* copy args->ivdat_path to fname.ivdat */
  char fname_ivdat[FILENAME_MAX];
  strcpy(fname_ivdat, fname);
  strcat(fname_ivdat, ".ivdat");
  if (!strcmp(args->ivdat_path, fname_ivdat))
  {
    file_copy(args->ivdat_path, fname_ivdat);
  }

  /* copy args->param_path to fname.param */
  char fname_param[FILENAME_MAX];
  strcpy(fname_param, fname);
  strcat(fname_param, ".param");
  if (strcmp(args->param_path, fname_param) == 0)
  {
    file_copy(args->param_path, fname_param);
  }

  /* release memory that was allocated */
  free(spot);
  mkiv_args_free(args);
  mkiv_params_free(params);
  mkiv_input_free(inputs);

  return(0);
}
