/********************************************************************
  GH/11.11.92
  file contains function:

     struct crelist *cr_rdexpt( struct rfivcur *iv_cur, char *filename )
     
     Read experimental IV curve

Changes:
GH/24.06.94 - allocation error messages
GH/25.08.95 - return type is struct crelist *
********************************************************************/

#define WARNING
#define ERROR

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "crfac.h"       /* rf specific definitions */

struct crelist *cr_rdexpt( struct crivcur *iv_cur, char *filename )

/*********************************************************************
Read experimental IV curve from file specified by filename and check if
energy values are equidistant and sorted.

The order of the input file must be:
  <energy> <intensity>
Comments are indicated by a '#'.

parameters: - iv_cur: pointer to structure rfiv_cur. All available
	    information about the theoretical IV curve will be
	    stored in this structure.
	    - filename: file where the input should be read from.

return value: pointer to the IV curve (struct crelist). The
	      list is terminated by a pair (energy, intensity)
	      of negative values.
	      NULL, if failed;
*********************************************************************/

{

int i, lines, len;
long offs;
real max_int;

char line_buffer[STRSZ];
char *buffer;
struct crelist *list;

/* 
 copy input file to buffer
 count lines  without comments (i.e. number of data pairs)
 allocate memory for expt. IV curve
*/
#ifdef CONTROL
   fprintf(STDCTR,"(cr_rdexpt): opening \"%s\"\n", filename);
#endif
 if( (buffer = file2buffer(filename)) == NULL) 
 {
#ifdef ERROR
   fprintf(STDERR," *** error (cr_rdexpt): error in function file2buffer \n");
#endif
   return(NULL);
 }
 lines = rf_nclines(buffer);

#ifdef CONTROL_X
   fprintf(STDCTR,"(cr_rdexpt): file content:\n%s\n", buffer);
   fprintf(STDCTR,"(cr_rdexpt): lines = %d\n", lines);
#endif

 list = (struct crelist *) malloc((lines+1)* sizeof(struct crelist));
 if( list == NULL )
 {
#ifdef ERROR
   fprintf(STDERR,"*** error (cr_rdexpt): allocation error (list) \n");
#endif
   exit(1);
 }

#ifdef CONTROL_X
   fprintf(STDCTR,"(cr_rdexpt): after malloc\n");
#endif
/*
 write energy pairs to list
*/
 for ( i = 0, offs = 0L, max_int = 0., 
       iv_cur->exp_equidist = 1 , iv_cur->exp_sort = 1 ;
      ((len = bgets(buffer, offs, (long)STRSZ, line_buffer)) >-1) 
      && (i < lines);  offs += (long)len)
 {
    
  if(line_buffer[0] != '#')      /* no comment */
  {
    
#ifdef REAL_IS_DOUBLE
   if(sscanf(line_buffer,"%lf %lf",&list[i].energy, &list[i].intens) == 2)
#endif
#ifdef REAL_IS_FLOAT
   if(sscanf(line_buffer,"%f %f",&list[i].energy, &list[i].intens) == 2)
#endif

#ifdef CONTROL_XX
   fprintf(STDCTR,"(cr_rdexpt): (%s): %e %e\n", filename, 
                                      list[i].energy, list[i].intens);
#endif
    
   {
 /* search for max. intensity */
    if(list[i].intens > max_int) max_int = list[i].intens;

 /* check if list is sorted */
    if( (i > 0) && (list[i].energy < list[i-1].energy) )
     iv_cur->exp_sort = 0;

 /* check equidistance */
    if( (i > 1) &&
        (R_fabs ( (2*list[i-1].energy - list[i].energy - list[i-2].energy)) 
         >  ENG_TOLERANCE ) )
    {
     iv_cur->exp_equidist = 0;
#ifdef WARNING
     fprintf(STDWAR,
     "*** warning (cr_rdexpt): \"%s\" is not equidistant (No:%d)\n",
	    filename, i-1);
#endif
    }
 /* increment i */
    i++;
   }
  } 
 }
 if (i==0) printf("(cr_rdexpt): *** No input read from file \"%s\"\n",
		   filename);

#ifdef CONTROL
 else fprintf(STDCTR,"(cr_rdexpt): last pair(%d): %f %f\n", 
	  i-1, list[i-1].energy, list[i-1].intens);
 if(iv_cur->exp_equidist)
   fprintf(STDCTR,"(cr_rdexpt): expt. IV curve is equidistant\n");
 else
  fprintf(STDCTR,"(cr_rdexpt): expt. IV curve is not equidistant\n");
#endif

/* 
 write available information to structure iv_cur.
*/
 iv_cur->exp_leng = i;
 iv_cur->exp_first_eng = list[0].energy;
 iv_cur->exp_last_eng = list[i-1].energy;
 iv_cur->exp_max_int = max_int;

 #ifdef CONTROL
  fprintf(STDCTR,"(cr_rdexpt): iv_cur structure written\n");
#endif 
 
/*
 set last energy and intensities to termination value
*/
 list[i].energy = FEND_OF_LIST;  /* termination */
 list[i].intens = FEND_OF_LIST;  /* termination */

#ifdef CONTROL
  fprintf(STDCTR,"(cr_rdexpt): list terminations set\n");
#endif 
/*
 free previously allocated memory;
 return;
*/
 free(buffer);
 return(list);
}
