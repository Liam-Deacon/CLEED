/********************************************************************
 GH/25.08.95
 file contains function:

   int cr_lorentz( struct rfivcur *iv_cur, real vi, char *ctr)

Do a Lorentzian smooth for experimental or theoretiacl IV curves 

Changes:
  GH/05.10.92
********************************************************************/
/*
#define WRITE
#define CONTROL
*/

#define ERROR

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "crfac.h"          /* specific definitions etc. */

#define  EPSILON   0.001    /* determines the integration range 
                               for smooth */

int cr_lorentz( struct crivcur *iv_cur, real vi, char *ctr)

/********************************************************************
 Do a Lorentzian smooth for experimental or theoretiacl IV curve
 (equidistant or not):

          I(E') = c * S { I(E')* vi dE /[(E-E')^2 + vi^2] } .

	  c = 1/ S { vi dE /[(E-E')^2 + vi^2] }.
 
 If the IV curve is not sorted yet (iv_cur->the/exp_sort = 0), the 
 routine cr_(t)sort will be called to perform sorting.

 parameters: iv_cur: data structure containing all essential data
	     such as theor. IV curves. The theoretical IV curves
	     wiil be modified after return.

	     vi: width of Lorentzian.

	     ctr: "t" - smooth theoretical data.
	          "e" - smooth experimental data.

 return value: 1, if successful.
	       I_FAIL, if failed.
********************************************************************/

{
int i, i_hi, i_lo;
int i_sum;
int n_range;

real faux;
real e_step;
real norm_sum;             /* norm_sum is the sum over the prefactors 
			      of all summands in the Lorentz sum. I.e.
			      dividing the sum by this number leads to
			      normalisation */
real *intbuf;             /* this array is used to store original
                              intensity values */
real *prefac;

#ifdef WRITE
FILE *str_old, *str_new;
#endif

/*
 First check if vi is nonzero
*/

 if (vi < ENG_TOLERANCE)
 {
#ifdef ERROR
  fprintf(STDERR,"*** error (cr_lorentz): Vi is too small\n");
#endif
  return(I_FAIL); 
 }


/*
  Sort IV curve if not yet done
*/

 if( (!iv_cur->the_sort) || (!iv_cur->exp_sort) )
   cr_sort(iv_cur);

 switch (ctr[0])
 {
/*
 smooth theoretical IV curve
*/
  case ('t'):
  {

#ifdef WRITE /* open control output if required */
    str_old = fopen("rflorentz.old.the","w");
    str_new = fopen("rflorentz.new.the","w");
#endif

    if (iv_cur->the_equidist)
    {
      e_step = iv_cur->the_list[1].energy - iv_cur->the_list[0].energy;

      if (e_step < ENG_TOLERANCE)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): e_step is too small\n");
#endif
        return(I_FAIL);
      }

/* Find energy range for integral */
      faux = vi * R_sqrt(1./EPSILON - 1.);
      n_range = (int) R_nint(faux / e_step);

#ifdef CONTROL
        fprintf(STDCTR,"(cr_lorentz): (the) e_step: %.1f n_range: %d\n", 
                e_step, n_range);
#endif

      if( (prefac  = (real *)malloc(n_range * sizeof(real)) ) == NULL)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): allocation error (prefac)\n");
#endif
        exit(1);
      }

      if( (intbuf = (real *)malloc(iv_cur->the_leng * sizeof(real)) ) == NULL)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): allocation error (intbuf)\n");
#endif
        exit(1);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = e_step * vi / ( SQUARE(e_step*i) + SQUARE(vi) );
      }

/*
 scan over all energies
*/
      for (i = 0; i < iv_cur->the_leng; i++ )
      {
/*
   store original intensities and calculate
    first element of integral sum
*/
        intbuf[i] = iv_cur->the_list[i].intens;
        iv_cur->the_list[i].intens *= prefac[0];
        norm_sum = prefac[0];

/* upper branch: */
        i_hi = MIN(i+n_range,iv_cur->the_leng);
        for (i_sum = i+1; i_sum < i_hi; i_sum++)
        {
          iv_cur->the_list[i].intens +=
             iv_cur->the_list[i_sum].intens * prefac[i_sum-i];
          norm_sum += prefac[i_sum-i];
        }  /* upper branch */

/* lower branch: */
        i_lo=MAX(i-n_range+1, 0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->the_list[i].intens += 
             intbuf[i_sum] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        }  /* lower branch */

/* normalize intensity */
        iv_cur->the_list[i].intens /= norm_sum;

#ifdef WRITE
        fprintf(str_old,"%f %f\n", iv_cur->the_list[i].energy,
                intbuf[i] );
        fprintf(str_new,"%f %f\n", iv_cur->the_list[i].energy,
                iv_cur->the_list[i].intens);
#endif
      }   /* energy scan */

/* set smooth flag */
      iv_cur->the_smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    }    /* if equidistant */
    else
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (cr_lorentz): theor. IV curve is not equidistant");
#endif
      return(I_FAIL);
    }
    break;
  }     /* end: case 't' */

/*
 smooth experimental IV curve
*/
  case ('e'):
  {
#ifdef WRITE /* open control output if required */

    str_old = fopen("rflorentz.old.exp","w");
    str_new = fopen("rflorentz.new.exp","w");

#endif

/*
    if (iv_cur->exp_equidist)
*/
    if (1)
    {
      e_step = iv_cur->exp_list[1].energy - iv_cur->exp_list[0].energy;
      if (e_step < ENG_TOLERANCE)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): e_step is too small\n");
#endif
        return(I_FAIL); 
      }

/* Find energy range for integral */
      faux = vi * R_sqrt(1./EPSILON - 1.);
      n_range = (int) R_nint(faux / e_step);

#ifdef CONTROL
        fprintf(STDCTR,"(cr_lorentz): (exp) e_step: %.1f n_range: %d\n", 
                e_step, n_range);
#endif

      if( (prefac  = (real *)malloc(n_range * sizeof(real)) ) == NULL)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): allocation error (prefac)\n");
#endif
        exit(1);
      }

      if( (intbuf = (real *)malloc(iv_cur->exp_leng * sizeof(real)) ) == NULL)
      {
#ifdef ERROR
        fprintf(STDERR,"*** error (cr_lorentz): allocation error (intbuf)\n");
#endif
        exit(1);
      }

      for (i = 0; i < n_range; i ++)
      {
        prefac[i]  = e_step * vi / ( SQUARE(e_step*i) + SQUARE(vi) );
      }
/*
 scan over all energies
*/
      for (i = 0; i< iv_cur->exp_leng; i++ )
      {
/* 
   store original intensities and calculate
   first element of integral sum 
*/
        intbuf[i] = iv_cur->exp_list[i].intens;
        iv_cur->exp_list[i].intens *= prefac[0];
        norm_sum = prefac[0];


/* upper branch: */
        i_hi=MIN(i+n_range, iv_cur->exp_leng);
        for (i_sum = i+1; i_sum < i_hi; i_sum++)
        {
          iv_cur->exp_list[i].intens += 
	     iv_cur->exp_list[i_sum].intens * prefac[i_sum-i];
          norm_sum += prefac[i_sum-i];
        }  /* upper branch */

/* lower branch: */
        i_lo=MAX(i-n_range+1, 0);
        for (i_sum = i_lo; i_sum < i; i_sum++)
        {
          iv_cur->exp_list[i].intens += intbuf[i] * prefac[i-i_sum];
          norm_sum += prefac[i-i_sum];
        }  /* lower branch */

/* normalize intensity */
        iv_cur->exp_list[i].intens /= norm_sum;

#ifdef WRITE
        fprintf(str_old,"%f %f\n", iv_cur->exp_list[i].energy, 
	        intbuf[i] );
        fprintf(str_new,"%f %f\n", iv_cur->exp_list[i].energy, 
	        iv_cur->exp_list[i].intens);
#endif
      }   /* energy scan */

/* set smooth flag */
      iv_cur->exp_smooth = SM_LORENTZ;

      free(prefac);
      free(intbuf);
    }    /* if equidistant */
    else
    {
#ifdef ERROR
      fprintf(STDERR,
      "*** error (cr_lorentz): expt. IV curve is not equidistant");
#endif
      return(I_FAIL); 
    }
    break;
  }     /* case 'e' */
 }      /* switch */

#ifdef WRITE /* print control output if required */
 fflush(str_old);
 close(str_old);
 fflush(str_new);
 close(str_new);
#endif

 return (1);
}
