/**************************************************************************<
    
             File Name: mkiv.c
    
**************************************************************************/

/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>

#include "tiffio.h"
#include "mkiv.h"

#ifdef _USE_OPENMP
#include <omp.h>           /* for parallel processing */
#endif 

#ifndef  VPRINT
#define  VPRINT            if ( verb & VERBOSE ) printf
#endif
#ifndef  QQ
#define  QQ                if ( !( verb & QUICK ) )
#endif


/***************************************************************************
SU/   03.92
GH/22.07.92
GH/25.08.92
CS/29.7.93
CS/23.8.93
VJ/28.01.03
GH/11.03.03
LD/28.02.14

 Purpose:
  subtest evaluates LEED-images

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


/**************************************************************************/
int main(argc, argv)
/**************************************************************************/
/* parameters */

int argc;                          /* number of arguments                  */
char *argv[];
{

/* type definitions */

    ImageMatrix *mat_image;        /* resulting LEED-image as a matrix     */
    ImageMatrix *mat_mask;         /* mask that defines visible LEED-screen*/

    tifvalues *tif_image,          /* resulting LEED-image in TIFF-format  */
              *tif_mask;           /* mask that defines visible LEED-screen*/
     
    struct coord center;           /* LEED-screen center                   */
    float router,rinner,radius;    /* bounds of visible screen             */
    float ratio;                   /* (screen radius)/(camera distance)    */

    float e_step = 1.;             /* energy step between images           */
    
    int numb;                      /* run through number for subsequent    */
                                   /* leed image data files                */
    int numb_2;                    /* run through display of images        */
    int nstart,nstop;              /* number of first and last LEED image  */
    int n_show;                    /* process images                       */
    int n_show_2;                  /* show image after each n_show_2 images*/
    int repetitions;		       /* number of repetitions of frame       */

    char *linebuffer;

    char *fname2;
    char fname[STRSZ],             /* filename of first leed image         */
         maskname[STRSZ];

    float kappa,                   /* comp. factor sqrt(en_old/energy)     */
          energy, en_old, en_ref,  /* current, former and ref. beam energy */ 
          k_10,                    /* parallel length of (1,0) beam        */
          s2n_ref;                 /* min S/N ratio for basis recalibration*/

    struct vector a[3], newa[3],   /* a[0] = origin = (0,0)-reflex         */
           da[3];                  /* a[1],a[2] = rec. lattice vectors     */
                                   /* vector.(len,xx,yy)                   */

    int nspot, spot_max;           /* number of used, max array-members    */
    struct spot *spot;             /* list of measurable reflexes          */
                                   /* spot.(kp_len,lind1/2,x0,y0,xx,yy,    */
                                   /*    cos_th,intensity,s2u,s2n,control) */

    struct spot *mem4spots();      /* subroutine allocating memory 4 spots */
    float inty[I_MAX];             /* intensity values interim storage     */

    int ndesi, nexcl, nref;        /* number of used array-members         */
    struct lindex desi[I_MAX],     /* indices of the desired reflexes      */
                  excl[I_MAX],     /* indices of the excluded reflexes     */
                  ref[I_MAX];      /* indices of the reference reflexes    */
                                   /* lindex.(lind1,lind2,control)         */

    int ndom;                      /* number of domains                    */
    struct domain superl[D_MAX];   /* basic superlattice vectors           */
                                   /* in units of a[1],a[2]                */
                                   /* domain.(lind11,lind12,lind21,lind22) */

    int bg;                        /* 0: no background subtraction         */
    float range, rel_range;        /* range for maximum search             */
    struct vector scale, rel_scale;/* scaling factors for integration area */
    float angle;                   /*angle between the broadening direction*/
                                   /* of the reflexes and horizontal axis  */
    float int_norm[E_MAX],         /* beam current                         */
          use_cur;                 /* beam current currently used          */
    int mod_cur;                   /* 0: use stored beam current 
                                      1: smooth stored beam current 
                                     -1: use mod_cur as beam current       */

/* variables from mkiv.var */
    float cos_min, cos_max;        /* min/max cosine for reference spots   */
    float verh, acci, accb;        /* integration area ratios              */
    int distance;                  /* min distance for reference spots     */
    int update;                    /* QQ -> step for updating output files */
    int step;                      /* step for fimax                       */
    int trip_max;                  /* max number of triples for recalib.   */
    float scale_min, range_min;    /* min for scale/range                  */
    float s2n_good, s2n_bad;       /* thresholds for good/bad spots        */
    int ref_min;                   /* min no. of predef spots for recalib. */
    float sec_range;               /* range(2nd loop)=range(1st)/sec_range */
    float ref_dev;      		   /* max. deviation of new base/origin    */
    float bas_rat;

/* auxiliaries */
    int i, k, nnref; 
    int flag, save_intermediates, make_mask;

/* inputs from command line: */
    int verb;                      /* verb&VERBOSE -> more info about run  */
                                   /* verb&QUICK -> quick and quiet mode   */

 /* filenames */
    char *fname_mkiv_ima = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_mkiv_inp = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_mkiv_dat = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_mkiv_var = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_mkiv_par = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_mkiv_pos = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_beam_raw = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *fname_beam_smo = (char*)malloc(sizeof(char)*FILENAME_MAX);
    char *dummy = (char*)malloc(sizeof(char)*FILENAME_MAX); 
    
/* open statements */
    FILE *cur_stream, *smcur_stream, *iv_stream;

/***************************************************************************/

    fname[0] = '\0';
    maskname[0] = '\0';

/***************************************************************************
   MAIN BEFORE ENERGY - LOOP
***************************************************************************/
/* initialise filenames */
    strcpy(fname_mkiv_ima, "ima.byte");
    strcpy(fname_mkiv_inp, "mkiv.inp");
    strcpy(fname_mkiv_dat, "mkiv.ivdat");
    strcpy(fname_mkiv_var, "mkiv.var");
    strcpy(fname_mkiv_pos, "mkiv.pos");
    strcpy(fname_mkiv_par, "mkiv.param");
    strcpy(fname_beam_raw, "beam.raw");
    strcpy(fname_beam_smo, "beam.smo");
    
/* backup existing old output files */
    file_backup(fname_mkiv_dat);
    file_backup(fname_beam_raw);
    file_backup(fname_beam_smo);
    
/* preset variables */
    verb = flag = save_intermediates = repetitions = make_mask = 0;
    n_show = n_show_2 = 1;

/* Allocate memory for mat/tif_image and mat/tif_mask */

    mat_image = (ImageMatrix *)malloc(sizeof(ImageMatrix));
    mat_mask  = (ImageMatrix *)malloc(sizeof(ImageMatrix));
    mat_image->imagedata = mat_mask->imagedata = NULL;

    tif_image = (tifvalues *)malloc(sizeof(tifvalues));
    tif_mask  = (tifvalues *)malloc(sizeof(tifvalues));
    tif_image->buf = tif_mask->buf = NULL;

    linebuffer = (char *)malloc(STRSZ);

/* decode arguments */
    if (argc <= 1) {
        ERR_EXIT(mkiv : wrong arguments - see 'mkiv -h' for usage.);
    }
    for (i=1; i<=argc; i++)
    {
        while (*++*argv != '\0' || i < argc)
        {
            if (ARG_IS("-h") || ARG_IS("--help")) {
                mkiv_usage(stdout);
                exit(0);
            }
            else if (ARG_IS("-i") || ARG_IS("--input")) {
                if (i+1 < argc) {
                    strcpy(dummy, argv[i+1]);
                    if (file_exists(dummy)) {
                        fprintf(stderr, 
                                "***warning: '%s' is a filename not a prefix\n",
                                dummy);
                        fprintf(stderr, "\tattempting to find files...");
                        dummy = remove_ext(argv[i+1], '.', '/');
                        
                    }

                    /* populate file names */
                    strcpy(fname_mkiv_inp, dummy); 
                    strcpy(fname_mkiv_pos, dummy);
                    strcpy(fname_mkiv_var, dummy);
                    strcat(fname_mkiv_inp, ".inp");
                    strcat(fname_mkiv_pos, ".pos");
                    strcat(fname_mkiv_var, ".var");

                    /* check files exist */
                    sprintf(dummy, "'%s' does not exist!\n", fname_mkiv_inp);
                    if (!file_exists(fname_mkiv_inp))
                        ERR_EXIT(dummy);
                    sprintf(dummy, "'%s' does not exist!\n", fname_mkiv_pos);
                    if (!file_exists(fname_mkiv_pos))
                        ERR_EXIT(dummy);
                    sprintf(dummy, "'%s' does not exist!\n", fname_mkiv_var);
                    if (!file_exists(fname_mkiv_var))
                        ERR_EXIT(dummy);
                            
                }
            }
            else if (ARG_IS("-s") || ARG_IS("--save-images")) {
                /* save processed images ('ima.byte' */
                save_intermediates = 1;
            }
            else if (ARG_IS("-m") || ARG_IS("--mask")) {
                STRCPY_ARG(maskname);
            }
            else if (ARG_IS("-M") || ARG_IS("--make-mask")) {
                make_mask = 1;
            }
            else if (ARG_IS("-o") || ARG_IS("--output")) {
                STRCPY_ARG(fname_mkiv_dat);
            }
            else if (ARG_IS("-p") || ARG_IS("--param")) {
                STRCPY_ARG(fname_mkiv_par);
            }
            else if (ARG_IS("-P") || ARG_IS("--pos")) {
                STRCPY_ARG(fname_mkiv_pos);
            }
            else if (ARG_IS("-V") || ARG_IS("--version")) {
                mkiv_info();
                exit(0);
            }
            else if (ARG_IS("-v") || ARG_IS("--verbose")) verb |= VERBOSE;
            else if (ARG_IS("-V") || ARG_IS("--version")) verb |= QUICK;
            else if (ARG_IS("-b") || ARG_IS("--current")) {
                if ( --argc && isdigit(**++argv) ) 
                    mod_cur = atoi(*argv);
                else 
                    ERR_EXIT(mkiv : wrong arguments for beam current.)
            }
            else if (ARG_IS("-q") || ARG_IS("--quiet") || ARG_IS("--quick"))
                verb |= QUICK;
            else ERR_EXIT(mkiv : wrong arguments - see 'mkiv -h' for usage.);
        }
    }

/* Some necessary inputs and inversion of superlattice matrix */

    readinp(verb,
        fname_mkiv_inp,
        fname_mkiv_par,
	    fname,                /* name of the reference image */
	    maskname,	    	  /* name of the file containing mask */
	    &nstart, &nstop,      /* number of first and last input file */
	    &e_step,  		      /* stepsize for energy-loop */
	    &center,              /* center of the image */
	    &router, &rinner,     /* outer and inner radius of the screen */
	    &bg,                  /* 0: no background subtraction */
	    &mod_cur,             /* mode of current treatment */
	    &k_10, 	              /* parallel length of (1,0) */
	    &ndom, superl,        /* superstructure matrix */
	    &ndesi, desi,         /* number and list of spots */
	    &nref, ref,           /* spots used for basis recalibration */
	    &nexcl, excl          /* spots without max. search */
	    );

    readvar(verb, 
        fname_mkiv_var, 
        fname_mkiv_par,
        &cos_min, &cos_max,
        &verh, &acci, &accb,
        &distance,
        &update, 
        &step,
        &trip_max,
        &scale_min, &range_min,
        &s2n_good, &s2n_bad, 
        &ref_min,
        &sec_range,
        &s2n_ref,
        &ref_dev,
        &bas_rat,
        &rel_range,
        &rel_scale, &angle,
        &ratio
        );

/* Read mask, if existing, else use router,rinner for masking */
    if(maskname[0] == '\0' && file_exists(maskname))
    {
        fprintf(stdout,"\nusing router,rinner for masking. \n");
    }
    else
    {
        readtif(tif_mask, maskname);
        fprintf(stdout, "\nusing %s for masking. \n", maskname);
    }


/* Read image */
    readtif(tif_image, fname);

/* Convert TIFF image (tifvalues) into a matrix (ImageMatrix) so that 
   MKIV is able to process them 
*/
    conv_tif2mat(tif_image, mat_image);
    
    if (save_intermediates) 
    {
        strcpy(fname_mkiv_ima, remove_ext(fname, '.', '/'));
        strcat(fname_mkiv_ima, ".byte");
    }
    out_tif(mat_image, fname_mkiv_ima);

    if(tif_mask != NULL) conv_tif2mat(tif_mask, mat_mask);
    else 
    {
        free(mat_mask);
        mat_mask = NULL;
    }

/* Read energy and beam current from file header */

/*
     en_old = en_ref = (float)(mat_image->ispare1 &0xFFFF)/(mat_image->ispare1 >> 16);
     *int_norm = (float)(mat_image->ispare2 &0xFFFF)/(mat_image->ispare2 >> 16); 
*/

/* !!!!!!!!!!!!!!!!!!!!!!! */
    en_old = en_ref = 100.;
    *int_norm = 100.;
/* !!!!!!!!!!!!!!!!!!!!!!! */

/* draw the mask or rinner, router and the center of the image and display */
    fprintf(stdout, "draw LEED-screen boundaries \n");
    drawbound(mat_image, mat_mask, &center, router, rinner, -1, fname_mkiv_ima);

/* only needed to produce a mask: */
    if (make_mask) mkmask(mat_image, &center, router, rinner, -1, maskname);

/* Allocate memory for the input reference spots for the first frame */
    spot = (struct spot *)malloc( 100* sizeof(struct spot));
    if (spot==NULL) ERR_EXIT(mkiv : memory allocation for ref_inp failed)

/* Input of reference spots */
    ref_inp(&nspot,spot,verb);     
    while ( nspot < 3 ) ref_inp(&nspot, spot, 0);

/* Input of reference energy */
    fprintf(stdout,"energy of reference image (in eV):\n");
    linebuffer = fgets(linebuffer, STRSZ, stdin);
    en_old = en_ref = atof(linebuffer);
    fprintf(stdout, "energy = %f eV\n", en_ref);

/* Compute basic vectors and origin */
    fprintf(stderr,"calculate new base and origin\n");
    calcbase(nspot, spot,  /* input: no.of and list of spots */
             a,            /* output: rec.lattice vectors, origin */
             ratio,        /* ratio radius/camera distance */
             distance/4,   /* min distance for recalibr. spots */
             trip_max,     /* max number of triples for recalib */
             -1.);         /* cor_fac<0 : no recorrection, no input-orig.*/

/* Free allocated memory */
    free(spot);

/* Allocate memory for struct spot array using highest energy 1000eV */
    calca(&kappa, &en_old, 1000., a, 
          &range, 0., rel_range, &scale, 0., rel_scale);
    spot = mem4spots( &spot_max, a, ndom, superl, mat_image);
    fprintf(stdout, "number of spots memory was allocated to : %d \n", 
            spot_max);

    calca(&kappa, &en_old, en_ref, a, 
          &range, 0., rel_range, &scale, 0., rel_scale);

    fprintf(stdout,"orig: %.0f,%.0f ,", a[0].yy, a[0].xx);
    fprintf(stdout,"a1:%.0f,%.0f ,a2:%.0f,%.0f\n", 
                    a[1].yy, a[1].xx, a[2].yy, a[2].xx);

/* Compute reciprocal lattice and create a table of spots containing: 
   indices, positions, k-value, cos_th */
    fprintf(stdout, "compute reciprocal lattice\n");
    calcspotpos(a, ndom, superl, spot_max, &nspot, spot, nexcl, excl,
                mat_mask, router, 0., &radius, en_old, ratio, k_10);
    fprintf(stdout, "New lattice - sin: %6.4f, radius:%7.1f, \n",
            k_10*H2M/sqrt(en_old), radius);
    fprintf(stderr, "spots: %d\n", nspot);

/* Set the spot.control bits according to the lists desi,ref */
     fprintf(stderr, "set control according to desi,ref \n");
     QQ setcontrol(nspot, spot, ndesi,desi, nref, ref); 

/* Mark measurable reflexes */
    fprintf(stderr,"draw circles around spots\n");
    mark_reflex(nspot, spot, mat_image,    /* list of spots, image */
                THICK, range, WHITE,       /* width,radius,color of circles*/
                -1, fname_mkiv_ima);       /* draw indices & output filename */
    //XV;  //antiquated xv unix command

/*************************************************************************
     ENERGY - LOOP 
*************************************************************************/

/* Open files "mkiv.ivdat", "beam_raw" and "beam_smo" */

    char *msg = (char *)malloc(512);
    

    fprintf(stderr,"Open output files\n");

    iv_stream = fopen(fname_mkiv_dat, "w");
    sprintf(msg, "(mkiv_main): '%s' open failed!", fname_mkiv_dat);
    if (iv_stream == NULL) ERR_EXIT(msg);
    
    cur_stream = fopen(fname_beam_raw, "w");
    sprintf(msg, "(mkiv_main): '%s' open failed!", fname_beam_raw);
    if (cur_stream == NULL) ERR_EXIT(msg);
    
    smcur_stream = fopen(fname_beam_smo, "w");
    sprintf(msg, "(mkiv_main): '%s' open failed!", fname_beam_smo);
    if (smcur_stream == NULL) ERR_EXIT(msg);
 
/* Make list header */
    fprintf(iv_stream," h     ");
    for ( i=0; i<ndesi; i++) 
        fprintf(iv_stream, "%10.2f", desi[i].lind1);
    fprintf(iv_stream, "\n k     ");
    for ( i=0; i<ndesi; i++) 
        fprintf(iv_stream, "%10.2f", desi[i].lind2);
    fprintf(iv_stream, "\n nenergy\n");

/* Loop for data files */
    fprintf(stderr,"nstart = %d, nstop = %d \n", nstart, nstop);
    
    /* int i_e_step; */
    /* i_e_step = (int)e_step; */
    
    numb_2 = nstart;
#ifdef _USE_OPENMP
    /* #pragma omp parallel for */
#endif
    for ( numb = nstart; 
        ((numb<=nstop)&&(e_step>0)) || ((numb>=nstop)&&(e_step<0)); 
        numb+=e_step)  /* note the dodgy cast from float to int */ 
    {

        /* Read image and convert tifvalues to ImageMatrix */

        fname2 = filename(fname, numb);
        fprintf(stderr, "read data from file \"%s\" and convert \n", 
                fname2);

        readtif(tif_image, fname2);
        conv_tif2mat(tif_image, mat_image);

        /* Read energy from file header */
        energy = (float)numb;

        /* Compute contraction of new basic vectors, range and scale */
        calca(&kappa, &en_old, energy, a, 
              &range, range_min, rel_range, &scale, scale_min, rel_scale);

        /* use current from file and smooth it when desired */
        int_norm[numb]= 100.;
        use_cur = int_norm[numb];
        if (mod_cur<0) use_cur = b_smooth(int_norm, numb, nstart, e_step, 10);
        if (mod_cur>0) use_cur = (float)mod_cur;

        printf("\n\n");
        QQ printf("#####################################################\n");
        QQ printf("\t\t FILE NAME: %s\n", fname);
        printf("\t\t ENERGY: %6.1f eV \n", energy);
        QQ printf("\t\t CURRENT: %7.1f (%7.1f)nA\n", int_norm[numb], use_cur);
        printf("#####################################################\n");
        printf("New range for Max. search: %.1f pixels.\n", range);
        printf("Elliptical integration area : (%.1f*%.1f) pixels.\n",
               scale.xx, scale.yy);
        VPRINT("orig: %.0f,%.0f , a1: %.0f,", a[0].yy, a[0].xx, a[1].yy);
        VPRINT("%.0f , a2: %.0f,%.0f \n", a[1].xx, a[2].yy, a[2].xx);

        /* Compute new reciprocal lattice */
        calcspotpos(a, ndom, superl, spot_max, &nspot, spot, nexcl, excl,
                    mat_mask, router+range, rinner-range,
                    &radius, energy, ratio, k_10);
        QQ printf("New lattice : sin: %6.4f, radius:%7.1f, \n",
                  k_10*H2M/sqrt(energy), radius);

        /* Sort the table in increasing order of k_par */
        QQ quicksort(spot, spot+nspot-1);

        /* Set the spot.control bits according to the lists desi,ref */
        setcontrol(nspot, spot, ndesi, desi, nref, ref);

        /* Find spot maxima */
        printf("%d spots measurable. \n", nspot);
        QQ 
        {
            printf("    Max. search for: \n");
            for (i=0; i<nspot; i++)
                printf("(%5.2f,%5.2f)%1li; ",
                spot[i].lind1, spot[i].lind2, spot[i].control);  
            printf("\n");
        }
        
        /* For spots not found in list excl, the maximum is determined */
        VPRINT("find maximum \n");
        fimax4(nspot, spot,       /* number and list of measurable spots */
               step,              /* step size for max search */
               range, mat_image); /* range for max search, LEED image */

        /* Find the exact positions 
           (i.e. center of gravity method) of all wanted spots*/
        VPRINT("calculate center of intensity \n");
        calcoi(nspot, spot,       /* number and list of measurable spots */
               range, mat_image); /* range for cog search, LEED image */

        /* Compute total intensity of the reflexes */
        if(bg <=1) ERR_EXIT(no more implemented background!);

        VPRINT("get intensities of spots \n");
        get_int(nspot,spot,          /* number and list of measurable spots */
                mat_image, mat_mask, /* LEED image, mask */
                &scale,              /* half axes of the ellip. integr. area*/
                angle,               /* angle of int.area versus horizontal */
                use_cur,             /* primary beam current (normalization)*/
                bg-1,                /* (1,!=1)=(y/n)background subtraction */
                s2n_good, verb,      /* if s2n < s2n_good -> bad spot */
                verh, acci, accb);   /* integr. area ratios */

        /* For spots with bad s/n-ratio, use calculated position */

        fprintf(stderr,"spots with bad s/n again : \n");  
        for( i=0; i<nspot; i++)
        {
            if (PYTH(spot[i].xx - spot[i].x0, spot[i].yy - spot[i].y0)
                < range/sec_range) 
                spot[i].control |= SPOT_GOOD_S2N;
            if ( spot[i].control & SPOT_GOOD_S2N ) continue;
            spot[i].xx = spot[i].x0;
            spot[i].yy = spot[i].y0;
        }

        /* Second run for spots with bad s/n : use range/sec_range, step=1 */
        fimax4(nspot, spot, 1,range/sec_range, mat_image);
        calcoi(nspot, spot, range, mat_image);
        get_int(nspot, spot, mat_image, mat_mask, &scale, angle,
                use_cur, bg-1, s2n_bad, verb, verh,acci,accb);

        /* Draw integration area boundaries into image */
        if(numb % n_show == 0)
        {  
            drawbound(mat_image, mat_mask, &center, router, rinner, -1, fname_mkiv_ima);

            fprintf(stderr, "draw ellipses around found positions\n");
            fprintf(stderr, "drawell:\n");
            drawell(nspot, spot, mat_image, &scale, angle, verh);
            fprintf(stderr, "mark_reflex:\n");
            mark_reflex(nspot, spot, mat_image, /* list of spots, image */
                        THICK, range, WHITE,    /* width,radius,color of circles */
                        0, dummy);              /* don't draw indices */
            numb_2++;
            /* if(numb_2 % n_show_2 == 0) XV;  // antiquated xv command */
        } 	  

        /* Save intensity values */
        fprintf(stderr,"Save intensity values:\n");
        for (i=0; i<ndesi; i++) 
        {
            k = desi[i].control;
            if (k==-1)  inty[i] = INT_OUT;
            else inty[i] = spot[k].intensity;
        }

        /* Calculate basis with true spot positions */
        fprintf(stderr,"Calculate basis:\n");
        nnref = 0;
        for (i=0; i<nspot; i++)
        {
            if (spot[i].s2n < s2n_ref ||
                spot[i].cos_th < cos_min || spot[i].cos_th > cos_max)
                spot[i--] = spot[--nspot];
            else
                if (spot[i].control & SPOT_REF) 
                    nnref++;
        }

        /* not enough satisfying spots found -> no recalibration possible */
        if(nspot < 3)  printf("No basis recalibration possible. \n");

        /* use reference spots */
        else {
            if( nnref >= ref_min ) 
            {
                printf("Basis recalibration, S/N > %.2f ",s2n_ref);
                printf("(%d predefined spots)\n",nnref);
                for( i=0; i<nspot; i++)
                    if( !( spot[i].control & SPOT_REF ) ) 
                        spot[i--] = spot[--nspot];
            }

            /* not enough predef. spots with satisfying s2n ratio -> use most intense 
            spots for recalibration */
            else printf("Basis recalibration, S/N > %.2f (%d spots)\n",
                        s2n_ref, nspot);

            /* enough satisfying spots found -> recalibration */
            newa[0] = a[0];
            i = calcbase(nspot,spot,  /* input: reference spots */
                         newa,        /* output: basis, input+output: origin */
                         ratio,       /* ratio radius/distance  */
                         distance,    /* min distance for recalibr. spots */
                         trip_max,    /* max number of triples for recalib */
                         0.);         /* recorrect with cos_th  */
           
            /* Allow  delta = sqrt( fabs(en_old - energy) ) * range / 4.
            as variation of the origin and the lattice vectors */

            if (!i) printf("Sorry, no triples for recalibration found.\n");
            else 
            {
                for (k=0; k<=2; k++) 
                {
                    da[k].xx = newa[k].xx - a[k].xx;
                    da[k].yy = newa[k].yy - a[k].yy;
                    da[k].len = PYTH( da[k].xx, da[k].yy );
                }
                if (da[0].len>ref_dev || da[1].len>ref_dev || da[2].len>ref_dev)
                    flag = DEV_BIG;
                else flag = DEV_SMALL;
                QQ printf("Used %d triples of spots for recalibration.\n",i);
                printf("dorig:(%.0f,%.0f); ", da[0].yy, da[0].xx);
                printf("da1:(%.0f,%.0f); ", da[1].yy, da[1].xx);
                printf("da2:(%.0f,%.0f);\n", da[2].yy, da[2].xx);

                /* take the new values for the origin and the basic lattice vectors */
                for (i=0; i<3; i++) 
                {
                   a[i].xx = ( 1. - bas_rat ) * a[i].xx + bas_rat * newa[i].xx;
                   a[i].yy = ( 1. - bas_rat ) * a[i].yy + bas_rat * newa[i].yy;
                   a[i].len = PYTH( a[i].xx , a[i].yy );
                }
            }  /* if i ... */
        }  /* if nnref ... */

        /* if the deviation between old and new a[] is too big, repeat frame */
        if (flag==DEV_BIG && i && ++repetitions<11) 
        {
            numb-=e_step;
            printf("same frame again because of too big base/orig-deviation\n");
        }
     
        /* update files fname_beam_raw, fname_beam_smo and fname_mkiv_dat" */
        else 
        {
            repetitions=0;
            VPRINT("updating output ivdat and beam.cur files... \n");
            fprintf(cur_stream, "%f\t%f\n", energy, int_norm[numb]);
            fprintf(smcur_stream, "%f\t%f\n", energy, use_cur);
            fprintf(iv_stream, "%6.1f", energy);
            for (i=0; i<ndesi; i++ ) 
                fprintf(iv_stream, " %10.5f", inty[i]);
            fprintf(iv_stream, "\n");
            QQ fflush(NULL);       /* flush all buffered output files */
            else if ((nstop-numb)%update == 0) 
                fflush(NULL);
        }



    }
/*************************************************************************
   END OF ENERGY - LOOP 
*************************************************************************/

    /* release memory that was allocated for spot structure array */
    free(spot);

    /* close output files */
    fclose(iv_stream);
    fclose(cur_stream);

    /* copy fname_mkiv_dat to fname.ivdat */
    char *fname_ivdat = (char *)malloc(strlen(fname) + 7 * sizeof(char));
    strcpy(fname_ivdat, fname);
    strcat(fname_ivdat, ".ivdat");
    if (!strcmp(fname_mkiv_dat, fname_ivdat)) {
        file_copy(fname_mkiv_dat, fname_ivdat);
    }

    /* copy fname_mkiv_par to fname.param */
    char *fname_param = (char *)malloc(strlen(fname) + 7 * sizeof(char));
    strcpy(fname_param, fname);
    strcat(fname_param, ".param");
    if (!strcmp(fname_mkiv_par, fname_param)) {
        file_copy(fname_mkiv_par, fname_param);
    }
    
    return 0;
}
/***************************************************************************/
