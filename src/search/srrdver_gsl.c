/***********************************************************************
 GH/23.10.95

 file contains function:

  int sr_rdver_gsl(char *ver_file, gsl_vector *y, gsl_matrix *p, int ndim)

 Read vertex for simplex algorithm from backup file.

 Changes:

GH/11.09.95 - Creation
GH/28.09.95 - Do not check dimensions if ndim is -1.
GH/23.10.95 - fix bug in the fgets calls (n_str instead of STRSZ)
LD/30.06.14 - modified for GSL input.

***********************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "search.h"

#ifdef ERROR
#ifdef EXIT_ON_ERROR
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver_gsl): allocation error (%s)\n",x); \
        exit(1)
#else
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver_gsl): allocation error (%s)\n",x); \
        return(-1)
#endif
#else      /* ifdef ERROR */
#ifdef EXIT_ON_ERROR
#define ALLERR(x) exit(1)
#else
#define ALLERR(x) return(-1)
#endif
#endif

/**********************************************************************/

int sr_rdver_gsl(
    const char *ver_file, 
    gsl_vector *y, 
    gsl_matrix *p
    )

/***********************************************************************

 Read vertex for simplex algorithm from backup file.
 
INPUT:
  char *ver_file - (input) input file containing start geometry for 
                 SEARCH.

  gsl_vector *y - (output) function values at the vertex positions.
  gsl_matrix *p - (output) coordinates of the vertex.


NOTE: memory for y and p must be allocated before entering the function.
             y = gsl_vector_alloc(ndim)
             p = gsl_matrix_alloc(ndim+1, ndim)

DESIGN:

FUNCTION CALLS:

RETURN VALUES:

    1 if ok.
   -1 if failed (and EXIT_ON_ERROR is not defined)

***********************************************************************/
{

FILE *ver_stream;

char *linebuffer;                        /* input buffer */
int n_str, i_str;

int i_par, j_par;                        /* counter, dummy  variables */
int m_par;

real faux;

int ndim = y->size;

/********************************************************************
  First: allocate linebuffer
********************************************************************/

 n_str = STRSZ;
 linebuffer = (char *)malloc(n_str * sizeof(char));

/********************************************************************
  START INPUT
  Open and Read vertex file
********************************************************************/

 if( (ver_stream = fopen(ver_file, "r")) == NULL)
 {
   #ifdef ERROR
     fprintf(STDERR, " *** error (sr_rdver_gsl):"
             " could not open file \"%s\"\n", ver_file);
   #endif
   #ifdef EXIT_ON_ERROR
     exit(1);
   #else
     return(-1);
   #endif
 }

 #ifdef CONTROL_X
   fprintf(STDCTR, "(sr_rdver_gsl): Reading file \"%s\"\n", ver_file);
 #endif

 while ( *fgets(linebuffer, n_str, ver_stream) == '#');
 sscanf(linebuffer, "%d %d", &i_par, &m_par);

 if( (i_par != ndim) || (m_par != ndim + 1) )
 {
   #ifdef ERROR
     fprintf(STDERR, " *** error (sr_rdver_gsl):"
             " dimensions do not match: %d/%d, %d/%d\n",
             i_par, ndim, m_par, ndim + 1);
   #endif
   #ifdef EXIT_ON_ERROR
     exit(1);
   #else
     return(-1);
   #endif
 }

 n_str = m_par * 15;
 free(linebuffer);
 linebuffer = (char *)malloc(n_str * sizeof(char));
 i_par = 0;
 while( (fgets(linebuffer, n_str, ver_stream) != NULL) && (i_par < m_par) )
 {

   #ifdef CONTROL_X
     fprintf(STDCTR,"(%s): %s", ver_file, linebuffer);
   #endif

   if( *linebuffer != '#')
   {
     i_str = 0;
     while(linebuffer[i_str] == ' ') i_str ++;
     #ifdef REAL_IS_DOUBLE
       sscanf(linebuffer+i_str, "%le", &faux);
       gsl_vector_set(y, i_par, faux);
     #endif
     #ifdef REAL_IS_FLOAT
       sscanf(linebuffer+i_str, "%e", &faux);
       gsl_vector_set(y, i_par, faux);
     #endif

     for(j_par = 0; j_par < ndim; j_par ++)
     {
       while(linebuffer[i_str] != ' ') i_str ++;
       while(linebuffer[i_str] == ' ') i_str ++;
       #ifdef REAL_IS_DOUBLE
         sscanf(linebuffer+i_str, "%le", &faux);
         gsl_matrix_set(p, i_par, j_par, faux);
       #endif
       #ifdef REAL_IS_FLOAT
        sscanf(linebuffer+i_str, "%e", &faux);
        gsl_matrix_set(p, i_par, j_par, faux);
       #endif
     }

     i_par++;
   } /* if != '#' */
 }   /* while: read input file */

   #ifdef CONTROL_X
     fprintf(STDCTR,"\n");
   #endif
/************************************************************************
  END OF INPUT
  Close input file.
*************************************************************************/
 fclose(ver_stream);

#ifdef CONTROL
 fprintf(STDCTR,"(sr_rdver_gsl): vertex read from \"%s\":\n", ver_file);
 for (i_par = 0; i_par < m_par; i_par++)
 {
   fprintf(STDCTR,"(%2d) %7.4f :", i_par, gsl_vector_get(y, i_par));
   for(j_par = 0; j_par < ndim; j_par++) 
     fprintf(STDCTR," %6.3f", gsl_matrix_get(p, i_par, j_par));
   fprintf(STDCTR,"\n");
 }
 fflush(STDCTR);
#endif

 return(1);
}  /* end of function sr_rdver */
/************************************************************************/
