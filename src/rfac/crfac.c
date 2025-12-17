/*********************************************************************
GH/29.08.95
  CRFAC

  Program calculates different R factors.
*********************************************************************/
#include <math.h>
#include <malloc.h>
#include <strings.h>

#include "crfac.h"
#include "gh_stddef.h"

/*********************************************************************/

int main(int argc, char *argv[])

/**********************************************************************
  Program calculates different R factors.
**********************************************************************/

{

int i_list;

real r_min, s_min, e_range;
real rr;

/*
 main structures:
*/
struct crargs  args;              /* program parameters from argument list */
	struct crivcur *iv_cur;           /* input data */
	
	char rfversion[STRSZ];            /* current program version */
	char t[2] = "t";
	char e[2] = "e";

/* print program version */
 rf_version(rfversion);
#ifdef CONTROL
 fprintf(STDCTR,"\n\t%s\n\n", rfversion);
#endif

/**********************************************************************
  Read arguments and set up program parameters (args)
**********************************************************************/
 args = cr_rdargs(argc, argv);

/**********************************************************************
  Read data from files 
**********************************************************************/
 iv_cur = cr_input(args.ctrfile, args.thefile);

#ifdef CONTROL
 fprintf(STDCTR,"crfac: after cr_input\n");
#endif

/**********************************************************************
  Smooth (and sort) IV curves
  Prepare cubic spline
**********************************************************************/

 for(i_list = 0; iv_cur[i_list].group_id != I_END_OF_LIST; i_list ++)
 {
   cr_lorentz(iv_cur+i_list, args.vi / 2., e);     /* theoretical */
   cr_lorentz(iv_cur+i_list, args.vi / 2., t);     /* experimental */

#ifdef CONTROL
   fprintf(STDCTR,"before spline\n");
#endif

/* prepare cubic spline */
   cr_spline((iv_cur+i_list)->exp_list, (iv_cur+i_list)->exp_leng);
   (iv_cur+i_list)->exp_spline = 1;
   cr_spline((iv_cur+i_list)->the_list, (iv_cur+i_list)->the_leng);
   (iv_cur+i_list)->the_spline = 1;
 }  /* for i_list */
 
#ifdef CONTROL
 fprintf(STDCTR,"before cr_rmin\n");
#endif
 
 r_min = cr_rmin(iv_cur, &args, &r_min, &s_min, &e_range);
 rr = R_sqrt(args.vi * 8. / e_range);

#ifdef CONTROL
 if(args.r_type == RP_FACTOR)       fprintf(STDCTR,"Rp = ");
 else if (args.r_type == R1_FACTOR) fprintf(STDCTR,"R1 = ");
 else if (args.r_type == R2_FACTOR) fprintf(STDCTR,"R2 = ");
 else if (args.r_type == RB_FACTOR) fprintf(STDCTR,"Rb1 = ");
 fprintf(STDCTR, "%.6f, RR = %.6f (shift = %4.1f, eng. overlap = %.1f)\n", 
 r_min, rr, s_min, e_range);
#endif

 fprintf(STDOUT,"%.6f %.6f %.2f %.2f\t\t#  ", r_min, rr, s_min, e_range);
 if(args.r_type == RP_FACTOR)       fprintf(STDCTR,"Rp  RR  shift  range\n");
 else if (args.r_type == R1_FACTOR) fprintf(STDCTR,"R1  RR  shift  range\n");
 else if (args.r_type == R2_FACTOR) fprintf(STDCTR,"R2  RR  shift  range\n");
 else if (args.r_type == RB_FACTOR) fprintf(STDCTR,"Rb1 RR  shift  range\n");
 return 0;
}      /* end of main */
