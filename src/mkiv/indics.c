
/**************************************************************************<
    >
    >         File Name: subtest.c
    >
>**************************************************************************/

/**************************************************************************/

#include "mkiv.h"
#include "file_functs.h"
#include <strings.h>
#include <ctype.h>

/**************************************************************************/
 
int main(int argc, char *argv[])

/***************************************************************************
SU/   03.92
GH/22.07.92
GH/25.08.92
CS/29.7.93
CS/23.8.93

 Purpose:
  subtest evaluates LEED-images

 Options:
  -b beam current : followed by the beam current value
  -c change input : allow changes of subtest.inp and subtest.var
  -h help         : print the IV_READ_ME helpfile
  -v verbose      : give more detailled information during computations
  -q quick & quiet: faster, no graphical output, only few printf's

 Input files:
  subtest.inp
  subtest.var
  mkiv.ecl 
  mkiv.pos
  LEED-images
  LEED-screen mask

 Output files:
  fname.ivdat
  fname.param
  beam.raw 
  beam.smo
***************************************************************************/

{

/* type definitions */

     struct ImageMatrix *image,    /* resulting LEED-image in matrix format*/
		    *imask,                /* mask that defines visible LEED-screen*/
            *readimage();          /* reading routine for image matrix     */
            
     struct coord center;          /* LEED-screen center                   */
     float router,rinner,radius;   /* bounds of visible screen             */
     float ratio;                  /* (screen radius)/(camera distance)    */

     int numb;                     /* run through number for subsequent    */
                                   /* leed image data files                */
     int nstart,nstop,e_step;      /* number of first and last LEED image  */
     char *filename(),             /* filename of actual leed image        */
          *strrchr();

     char fname[STRSZ];            /* filename of first leed image         */
     char maskname[STRSZ];

     float kappa,                  /* comp. factor sqrt(en_old/energy)     */
	   energy, en_old, en_ref,     /* current, former and ref. beam energy */ 
	   k_10,                       /* parallel length of (1,0) beam        */
	   s2n_ref;                    /* min S/N ratio for basis recalibration*/

     struct vector a[3], newa[3],  /* a[0] = origin = (0,0)-reflex         */
                   da[3];          /* a[1],a[2] = rec. lattice vectors     */
                                   /* vector.(len,xx,yy)                   */

     int nspot, spot_max;          /* number of used, max array-members    */
     struct spot *spot;            /* list of measurable reflexes          */
                                   /* spot.(kp_len,lind1/2,x0,y0,xx,yy,    */
                                   /*    cos_th,intensity,s2u,s2n,control) */

     struct spot *mem4spots();     /* subroutine allocating memory 4 spots */
     float inty[I_MAX];            /* intensity values interim storage     */

     int ndesi, nexcl, nref;       /* number of used array-members         */
     struct lindex desi[I_MAX],    /* indices of the desired reflexes      */
                   excl[I_MAX],    /* indices of the excluded reflexes     */
                   ref[I_MAX];     /* indices of the reference reflexes    */
                                   /* lindex.(lind1,lind2,control)         */

     int ndom;                     /* number of domains                    */
     struct domain superl[D_MAX];  /* basic superlattice vectors           */
                                   /* in units of a[1],a[2]                */
                                   /* domain.(lind11,lind12,lind21,lind22) */

     int bg;                       /* 0: no background subtraction         */
     float range,rel_range;        /* range for maximum search             */
     struct vector scale,rel_scale;/* scaling factors for integration area */
     float angle;                  /*angle between the broadening direction*/
                                   /* of the reflexes and horizontal axis  */
     
     float int_norm[E_MAX],        /* beam current */
	   use_cur,b_smooth();         /* beam current currently used */
     int mod_cur;                  /* 0: use stored beam current 
                                      1: smooth stored beam current 
                                     -1: use mod_cur as beam current */

/* variables from subtest.var */
     float cos_min, cos_max;       /* min/max cosine for reference spots */
     float verh, acci, accb;       /* integration area ratios */
     int distance;                 /* min distance for reference spots */
     int update;                   /* QQ -> step for updating output files */
     int step;                     /* step for fimax */
     int trip_max;                 /* max number of triples for recalib. */
     float scale_min, range_min;   /* min for scale/range */
     float s2n_good, s2n_bad;      /* thresholds for good/bad spots */
     int ref_min;                  /* min no. of predef spots for recalib. */
     float sec_range;              /* range(2nd loop)=range(1st)/sec_range */

/* auxiliaries */
     int i,j,k,nnref; 
     int val,flag;
     float fval;

/* inputs from command line: */
     int verb = 0;                 /* verb&VERBOSE -> more info about IVrun*/
                                   /* verb&QUICK -> quick and quiet mode  */
     int change_inp = 0;           /* change_inp=YES -> change subtest.inp*/
     char *inp_ext;                /* input etc. */

/* open statements */
     FILE *fopen(), *cur_stream, *smcur_stream, *iv_stream;

/***************************************************************************/

     file_move("i_v_dat", "i_v_bak");
     file_move("beam_raw", "beam_raw.bak");
     file_move("beam_smo", "beam_smo.bak");

/***************************************************************************
                      MAIN BEFORE ENERGY - LOOP 
***************************************************************************/

/* decode arguments */
     while ( --argc && **++argv == '-')
     {
	while (*++*argv != '\0')  switch (**argv)
	{
	   case 'h': system("more $IV_READ_ME"); break;
	   case 'v': verb |= VERBOSE; break;
	   case 'q': verb |= QUICK; break;
	   case 'b':
	      if ( --argc && isdigit(**++argv) ) mod_cur = atoi(*argv);
	      else ERR_EXIT(mkiv : wrong arguments for beam current.)
	      break;
	   case 'c':
	      change_inp = YES; 
	      if ( !--argc || **++argv == '-' )  inp_ext = "-\0";
	      else  inp_ext = *argv;
	      break;
	   default : ERR_EXIT(mkiv : wrong arguments);
        }
     }
     if (argc) ERR_EXIT(mkiv : wrong arguments);

/* Change input file, if necessary */
     if (change_inp == YES)
     {	 
        if (*inp_ext != '-')
        {
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
           fprintf(stderr, "***error (mkiv_main): editing of input not available on Windows\n");
        }
#else
     	   system("cp $IV_INPUTS/header subtest.inp");
           sprintf(fname ,"cat $IV_INPUTS/beams.%s >> subtest.inp", inp_ext);
           system(fname);
        }
        system("vi subtest.inp");
#endif
     }


/* Some necessary inputs and inversion of superlattice matrix */
     readinp(verb,
	     fname,               /* name of the reference image */
	     maskname,		  /* name of the file containing mask */
	     &nstart,&nstop,      /* number of first and last input file */
	     &e_step,  		  /* stepsize for energy-loop */
	     &center,             /* center of the image */
	     &router,&rinner,     /* outer and inner radius of the screen */
	     &bg,                 /* 0: no background subtraction */
	     &mod_cur,            /* mode of current treatment */
	     &k_10, 	          /* parallel length of (1,0) */
	     &ndom,superl,        /* superstructure matrix */
	     &ndesi,desi,         /* number and list of spots */
	     &nref,ref,           /* spots used for basis recalibration */
	     &nexcl,excl          /* spots without max. search */
	     );

     readvar(verb, fname,
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
	     &rel_range,
	     &rel_scale, &angle,
	     &ratio );

/* Read mask, if existing, else use router,rinner for masking */
     imask = readimage(maskname);
     if (imask != NULL ) printf("\nusing %s for masking. \n\n",maskname);
     else                printf("\nusing router,rinner for masking. \n\n");

/* Read image */
     image=readimage(fname);
     if ( image == NULL )   ERR_EXIT(could not read image!);

/* Read energy and beam current from file header */
     en_old=en_ref = (float)(image->ispare1 &0xFFFF) / (image->ispare1 >>16);
     *int_norm = (float)(image->ispare2 &0xFFFF) / (image->ispare2 >>16); 

/* Neccessary conversions for plotting ( -> 1 byte = 256 colors) */
     VPRINT("conversion to 1_BYTE\n");
     QQ lvconvert(image, 1, 1, 0, 255., 1., 0);
  
/* Draw the mask or rinner, router and the center of the image */
     VPRINT("draw LEED-screen boundaries \n");
     QQ drawbound(image,imask,&center,router,rinner,1);

/* Plot LEED-image */
     VPRINT("editimage\n");
     QQ system("editimage -i ima.byte -use_cmap 0 -x 490 -y 10 -update 3 &");

/* Input of undesired reflexes */
     man_inp(&nexcl, excl, verb);
	
/* Allocate memory for the input reference spots for the first frame */
     spot = (struct spot *)malloc( 20* sizeof(struct spot));
     if (spot==NULL) ERR_EXIT(mkiv : memory allocation for ref_inp failed)

/* Input of reference spots */
     ref_inp(&nspot,spot,verb);     
     while ( nspot<3 ) ref_inp(&nspot,spot,0);

/* Compute basic vectors and origin */
     VPRINT("calculate new base and origin\n");
     calcbase(nspot,spot,   /* input: no.of and list of spots */
              a,            /* output: rec.lattice vectors, origin */
	      ratio,        /* ratio radius/camera distance */
	      distance/4,   /* min distance for recalibr. spots */
	      trip_max,     /* max number of triples for recalib */
	      -1.);         /* cor_fac<0 : no recorrection, no input-orig.*/

/* Free located memory */
     free(spot);

/* Allocate memory for struct spot array using highest energy 400eV */
     calca(&kappa, &en_old, 450., a, 
	   &range, 0., rel_range, &scale, 0., rel_scale);
     spot = mem4spots( &spot_max, a, ndom, superl, image);
     calca(&kappa, &en_old, en_ref, a, 
	   &range, 0., rel_range, &scale, 0., rel_scale);
     VPRINT("number of spots memory was allocated to : %d \n", spot_max);

     if (verb&QUICK) goto loop;
     VPRINT("orig: %.0f,%.0f ,",a[0].yy,a[0].xx);
     VPRINT("a1:%.0f,%.0f ,a2:%.0f,%.0f\n", a[1].yy,a[1].xx,a[2].yy,a[2].xx);

/* Compute reciprocal lattice and make a table of reflexes containing : 
   indices,positions,k-value,cos_th */
     VPRINT("compute reciprocal lattice\n");
     calcspotpos( a, ndom, superl, spot_max, &nspot, spot, nexcl, excl,
		 imask, router, 0., &center, &radius, 500., ratio, k_10);
     VPRINT("New lattice - sin: %6.4f, radius:%7.1f, \n",
	    k_10*H2M/sqrt(en_old), radius);
     VPRINT("spots: %d\n", nspot);

     quicksort(spot,spot+ nspot -1);

/* Hier fehlt ein Teil, der die Indizes ausgedruckt hat */
     

/* Set the spot.control bits according to the lists desi,ref */
     VPRINT("set control according to desi,ref \n");
     QQ setcontrol(nspot,spot, ndesi,desi, nref,ref); 

/* Mark measurable reflexes */
     VPRINT("draw circles around reflexes\n");
     QQ mark_reflex(nspot, spot, image,    /* list of spots, image */
                    THICK, range, WHITE,   /* width,radius,color of circles*/
                    1);                    /* draw indices */

     freeimage(image);

/***************************************************************************
                              ENERGY - LOOP 
***************************************************************************/

loop:
/* Open files "i_v_dat", "beam_raw" and "beam_smo" */
     iv_stream = fopen("i_v_dat","w");
      if (iv_stream == NULL) ERR_EXIT(i_v_dat: fopen failed!);
     cur_stream = fopen("beam_raw","w");
      if (cur_stream == NULL) ERR_EXIT(beam_raw: fopen failed!);
     smcur_stream = fopen("beam_smo","w");
      if (smcur_stream == NULL) ERR_EXIT(beam_smo: fopen failed!);
 
/* Make list header */
     fprintf(iv_stream,"      ");
     for ( i=0; i<ndesi; i++) fprintf(iv_stream,"%10.2f",desi[i].lind1);
     fprintf(iv_stream,"\n      ");
     for ( i=0; i<ndesi; i++) fprintf(iv_stream,"%10.2f",desi[i].lind2);
     fprintf(iv_stream,"\n\n");

/* Loop for data files */
     for (numb = nstart; ((numb<=nstop)&&(e_step>0)) || ((numb>=nstop)&&(e_step<0)) ; numb+=e_step)
     {
/* Read image */
	image = readimage( filename(fname,numb) );

/* Read energy from file header */
        energy = (float)(image->ispare1&0xFFFF)/(image->ispare1>>16);

/* Compute contraction of new basic vectors, range and scale */
	calca(&kappa, &en_old, energy, a, 
	      &range, range_min, rel_range, &scale, scale_min, rel_scale);

/* use current from file and smooth it when desired */
        int_norm[numb]=(float)(image->ispare2&0xFFFF)/(image->ispare2>>16);
	use_cur = int_norm[numb];
	if (mod_cur<0) use_cur =b_smooth(int_norm,numb,nstart,nstop,e_step,10);
	if (mod_cur>0) use_cur =(float)mod_cur;

	printf("\n\n");
        QQ printf("#####################################################\n");
        QQ printf("\t\t FILE NAME: %s\n",fname);
        printf("\t\t ENERGY: %6.1f eV \n",energy);
        QQ printf("\t\t CURRENT: %7.1f (%7.1f)nA\n",int_norm[numb],use_cur);
        printf("#####################################################\n");
        printf("New range for Max. search: %.1f pixels.\n", range);
	printf("Elliptical integration area : (%.1f*%.1f) pixels.\n",
	       scale.xx, scale.yy);
        VPRINT("orig: %.0f,%.0f , a1: %.0f,",a[0].yy,a[0].xx,a[1].yy);
        VPRINT("%.0f , a2: %.0f,%.0f \n",a[1].xx,a[2].yy,a[2].xx);

/* Necessary conversions of image  -> 2 byte = (unsigned short) */
        VPRINT("conversion to 2_BYTE\n");
        lvconvert(image, VFF_TYP_2_BYTE,  0, 0, 0, 1., 0);

/* Compute new reciprocal lattice */
        calcspotpos(a,ndom,superl,spot_max,&nspot,spot, nexcl,excl,
		    imask,router+range,rinner-range,&center,
		    &radius, energy,ratio,k_10);
	QQ printf("New lattice : sin: %6.4f, radius:%7.1f, \n",
		  k_10*H2M/sqrt(energy), radius);
 
/* Sort the table in increasing order of k_par */
        QQ quicksort(spot,spot+ nspot -1);
  
/* Set the spot.control bits according to the lists desi,ref */
	setcontrol(nspot,spot, ndesi,desi, nref,ref);

/* Find spot maxima */
        printf("%d spots measurable. \n", nspot);
   QQ { printf("    Max. search for: \n");
        for (i=0; i<nspot; i++)
	   printf("(%5.2f,%5.2f)%1x; ",
	          spot[i].lind1, spot[i].lind2, spot[i].control);         
        printf("\n");
      }
/* For spots not found in list excl, the maximum is determined */
        VPRINT("find maximum \n");
        fimax4(nspot,spot,       /* number and list of measurable spots */
	       step,             /* step size for max search */
	       range,image);     /* range for max search, LEED image */

/* Find the exact positions (i.e. center of gravity) of all wanted reflexes*/
        VPRINT("calculate center of intensity \n");
	calcoi(nspot,spot,       /* number and list of measurable spots */
               range, image);     /* range for cog search, LEED image */

/* Compute total intensity of the reflexes */
	if(bg <=1)   ERR_EXIT(nomore implemented bg!);

        VPRINT("get intensities of spots \n");
        get_int(nspot,spot,        /* number and list of measurable spots */
                image,imask,       /* LEED image, mask */
                &scale,            /* half axes of the ellip. integr. area*/
	        angle,             /* angle of int.area versus horizontal */
                use_cur,           /* primary beam current (normalization)*/
                bg-1,              /* (1,!=1)=(y/n)background subtraction */
                s2n_good,verb,     /* if s2n < s2n_good -> bad spot */
		verh,acci,accb);   /* integr.area ratios */

/* For spots with bad s/n-ratio, use calculated position */
        VPRINT("spots with bad s/n again : \n");
	for( i=0; i<nspot; i++)
	{
	   if ( PYTH ( spot[i].xx - spot[i].x0 , spot[i].yy - spot[i].y0 )
		< range/sec_range && sec_range<2 ) 
	      spot[i].control |= SPOT_GOOD_S2N;
	   if ( spot[i].control & SPOT_GOOD_S2N ) continue;
	   spot[i].xx = spot[i].x0;
	   spot[i].yy = spot[i].y0;
	}

/* Second run for spots with bad s/n : use range/sec_range, step=1 */
        fimax4(nspot,spot,1,range/sec_range,image);
	calcoi(nspot,spot,range,image);
        get_int(nspot, spot, image, imask, &scale, angle,
		use_cur, bg-1, s2n_bad, verb, verh,acci,accb);

/* Necessary conversions for plotting ( -> 1 byte = 256 colors ) */
     QQ {  VPRINT("conversion -> 1_BYTE\n");
           lvconvert(image, VFF_TYP_1_BYTE, 1, 0, 255., 1., 0);
	   image->map_enable = VFF_MAP_OPTIONAL;
	   image->color_space_model = VFF_CM_NONE;

/* Draw integration area boundaries into image */
           VPRINT("draw ellipses around found positions\n");
	   drawbound(image,imask,&center,router,rinner,0);
	   drawell(nspot,spot,image,a,&scale,angle,verh);
	   mark_reflex(nspot,spot, image, THICK, range, WHITE, 0);
	} 	  

/* Save intensity values */
	for ( i=0; i<ndesi; i++) {
	   k = desi[i].control;
	   if (k==-1)  inty[i] = INT_OUT;
	   else        inty[i] = spot[k].intensity;
        }

/* Calculate basis with true reflex positions */
	nnref = 0;
        for( i=0; i<nspot; i++)
	{
	   if( spot[i].s2n < s2n_ref ||
	       spot[i].cos_th < cos_min || spot[i].cos_th > cos_max )
	      spot[i--] = spot[--nspot];
	   else
	      if( spot[i].control & SPOT_REF ) nnref++;
	}

/* not enough satisfiing spots found -> no recalibration possible */
	if(nspot < 3)  printf("No basis recalibration possible. \n");

/* use reference spots */
	else {
	  if( nnref >= ref_min ) {
	     printf("Basis recalibration, S/N > %.2f ",s2n_ref);
	     printf("(%d predefined spots)\n",nnref);
	     for( i=0; i<nspot; i++)
	        if( !( spot[i].control & SPOT_REF ) ) 
		   spot[i--] = spot[--nspot];
          }

/* not enough predef. spots with satisfiing s2n ratio -> use most intense 
   spots for recalibration */
	  else printf("Basis recalibration, S/N > %.2f (%d spots)\n",
		      s2n_ref, nspot);

/* enough satisfiing spots found -> recalibration */
          newa[0] = a[0];
          i = calcbase(nspot,spot,  /* input: reference spots */
                       newa,        /* output: basis, input+output: origin */
                       ratio,       /* ratio radius/distance  */
		       distance,    /* min distance for recalibr.  spots */
	               trip_max,    /* max number of triples for recalib */
	               0.);         /* recorrect with cos_th  */
           
/* Allow  delta = sqrt( fabs(en_old - energy) ) * range / 4.
   as variation of the origin and the lattice vectors */

          if (!i) printf("Sorry, no triples for recalibration found.\n");
          else {
	    for ( k=0; k<=2; k++) {
	       da[k].xx = newa[k].xx - a[k].xx;
	       da[k].yy = newa[k].yy - a[k].yy;
	       da[k].len = PYTH( da[k].xx, da[k].yy );
	    }
	    if( da[0].len>2. || da[1].len>2. || da[2].len>2. )
	         flag = DEV_BIG;
            else flag = DEV_SMALL;
	    QQ printf("Used %d triples of spots for recalibration.\n",i);
	    printf("dorig:(%.0f,%.0f); ", da[0].yy, da[0].xx);
	    printf("da1:(%.0f,%.0f); ", da[1].yy, da[1].xx);
	    printf("da2:(%.0f,%.0f);\n", da[2].yy, da[2].xx);

/* take the new values for the origin and the basic lattice vectors */
  	    for (i=0; i<3; i++) a[i] = newa[i];
          }  /* if i ... */
        }  /* if nnref ... */

/* if the deviation between old and new a[] is too big, repeat frame */
        if ( flag==DEV_BIG && numb-=e_step)
           printf("same frame again because of too big base/orig-deviation\n");

/* update files "beam_raw", "beam_smo" and "i_v_dat" */
        else {
          VPRINT("updating output files i_v_dat and beam_cur... \n");
     	  fprintf(cur_stream,"%f\t%f\n",energy,int_norm[numb]);
	  fprintf(smcur_stream,"%f\t%f\n",energy,use_cur);
          fprintf(iv_stream,"%6.1f",energy);
          for ( i=0; i<ndesi; i++ ) fprintf(iv_stream," %10.5f",inty[i]);
	  fprintf(iv_stream,"\n");
	  QQ fflush(NULL);       /* flush all buffered output files */
	  else if ( (nstop - numb)%update == 0 ) fflush(NULL);
        }
        freeimage(image);

/* end of energy loop */
     }

/* release memory that was allocated for spot structure array */
     free(spot);

/* close output files */
     fclose(iv_stream);
     fclose(cur_stream);
}
/***************************************************************************/
