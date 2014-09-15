/***********************************************************************
 GH/23.10.95

 file contains function:

  int sr_rdver(char * ver_file, real *y, real **p, int ndim)

 Read vertex for simplex algorithm from backup file.

 Changes:

GH/11.09.95 - Creation
GH/28.09.95 - Do not check dimensions if ndim is -1.
GH/23.10.95 - fix bug in the fgets calls (n_str instead of STRSZ)

***********************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "search.h"

#ifdef ERROR
#ifdef EXIT_ON_ERROR
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver): allocation error (%s)\n",x); \
        exit(1)
#else
#define ALLERR(x) \
        fprintf(STDERR," *** error (sr_rdver): allocation error (%s)\n",x); \
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

int sr_rdver(char * ver_file, real *y, real **p, int ndim)

/***********************************************************************

 Read vertex for simplex algorithm from backup file.
 
INPUT:
  char *ver_file - (input) input file containing start geometry for 
                 SEARCH.

  real *y - (output) function values at the vertex positions.
  real **p - (output) coordinates of the vertex.

  int ndim - (input) dimension of y and p. If ndim < 0, do not check
             dimensions.

NOTE: memory for y and p must be allocated before entering the function.
             y = vector(1, ndim)
             p = matrix(1, ndim+1,1, ndim)

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
   fprintf(STDERR,
   " *** error (sr_rdver): could not open file \"%s\"\n",ver_file);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1);
#endif
 }

#ifdef CONTROL_X
 fprintf(STDCTR,"(sr_rdver): Reading file \"%s\"\n",ver_file);
#endif

 while ( *fgets(linebuffer, n_str, ver_stream) == '#');
 sscanf(linebuffer, "%d %d", &i_par, &m_par);

 if( ndim < 0)
 {
#ifdef WARNING
   fprintf(STDWAR,
   " * warning (sr_rdver): ndim < 0: dimensions are not checked\n");
#endif
   ndim = i_par;
 }
 else if( (i_par != ndim) || (m_par != ndim + 1) )
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (sr_rdver): dimensions do not match: %d/%d, %d/%d\n",
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
 i_par = 1;
 while( (fgets(linebuffer, n_str, ver_stream) != NULL) && (i_par <= m_par) )
 {

#ifdef CONTROL_X
   fprintf(STDCTR,"(%s): %s", ver_file, linebuffer);
#endif

   if( *linebuffer != '#')
   {
     i_str = 0;
     while(linebuffer[i_str] == ' ') i_str ++;
#ifdef REAL_IS_DOUBLE
     sscanf(linebuffer+i_str,"%le", y + i_par);
#endif
#ifdef REAL_IS_FLOAT
     sscanf(linebuffer+i_str,"%e", y + i_par);
#endif

     for(j_par = 1; j_par <= ndim; j_par ++)
     {
       while(linebuffer[i_str] != ' ') i_str ++;
       while(linebuffer[i_str] == ' ') i_str ++;
#ifdef REAL_IS_DOUBLE
       sscanf(linebuffer+i_str,"%le", p[i_par] + j_par);
#endif
#ifdef REAL_IS_FLOAT
       sscanf(linebuffer+i_str,"%e", p[i_par] + j_par);
#endif
     }

     i_par ++;
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
 fprintf(STDCTR,"(sr_rdver): vertex read from \"%s\":\n", ver_file);
 for (i_par = 1; i_par<= m_par; i_par++)
 {
   fprintf(STDCTR,"(%2d) %7.4f :", i_par, y[i_par]);
   for(j_par=1; j_par<= ndim; j_par++) 
     fprintf(STDCTR," %6.3f", p[i_par][j_par]);
   fprintf(STDCTR,"\n");
 }
 fflush(STDCTR);
#endif

 return(1);
}  /* end of function sr_rdver */
/************************************************************************/
