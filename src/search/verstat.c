/***********************************************************************
GH/28.09.95
 Check some statistics in the vertex file
 Modified:
 GH/28.09.95
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "search.h"
#include "sr_alloc.h"

#define CONTROL
#define ERROR

#define EXIT_ON_ERROR

/* only for compatibility with SEARCH project */

struct sratom_str *sr_atoms;
struct search_str *sr_search;
char *sr_project;

/**********************************************************************/

int main(int argc, char *argv[])
{

int i_arg;
int i_par, j_par;
int ndim, mpar;

real faux;
real *dev, *avg, *y, **p;

char inp_file[STRSZ];
char linebuffer[STRSZ];

FILE *inp_stream;

/*********************************************************************
  Decode arguments:

    -i <inp_file> - (mandatory input file) overlayer parameters of all 
                    parameters.
*********************************************************************/

 strncpy(inp_file,"---", STRSZ);

 for (i_arg = 1; i_arg < argc; i_arg++)
 {
  if(*argv[i_arg] != '-')
  {
#ifdef ERROR
   fprintf(STDERR,"*** error (verstat):\tsyntax error:\n");
   fprintf(STDERR,"\tusage: \tverstat -i <inp_file>\n");
#endif
   exit(1);
  }
  else
  {
/* Read parameter input file */
   if(strncmp(argv[i_arg], "-i", 2) == 0)
   {
    i_arg++;
    strncpy(inp_file, argv[i_arg], STRSZ);
   }
  } /* else */
 }  /* for i_arg */

/*********************************************************************
  Check existence of inp_file.
*********************************************************************/

 if(strncmp(inp_file, "---", 3) == 0)
 {
#ifdef ERROR
   fprintf(STDERR,
   "*** error (verstat): no parameter input file (option -i) specified\n");
#endif
   exit(1);
 }

/***********************************************************************
  Open input and read first line (i.e. ndim)
  (Close afterwards)
***********************************************************************/

 if( (inp_stream = fopen(inp_file, "r")) == NULL)
 {
#ifdef ERROR
   fprintf(STDERR,
   " *** error (sr_rdver): could not open file \"%s\"\n", inp_file);
#endif
#ifdef EXIT_ON_ERROR
   exit(1);
#else
   return(-1);
#endif
 }

 while ( *fgets(linebuffer, STRSZ, inp_stream) == '#');
 sscanf(linebuffer, "%d", &ndim);

 fclose(inp_stream);

/***********************************************************************
  Read rest of input file
***********************************************************************/

 mpar = ndim+1;
 y = sr_alloc_vector((size_t)mpar);
 p = sr_alloc_matrix((size_t)mpar, (size_t)ndim);
 if (y == NULL || p == NULL)
 {
   sr_free_vector(y);
   sr_free_matrix(p);
   fprintf(STDERR, "*** error (verstat): allocation failure\n");
   return 1;
 }

 sr_rdver(inp_file, y, p, ndim);

/***********************************************************************
  Find min/max values
***********************************************************************/

 dev = sr_alloc_vector((size_t)ndim);
 avg = sr_alloc_vector((size_t)ndim);
 if (dev == NULL || avg == NULL)
 {
   sr_free_vector(dev);
   sr_free_vector(avg);
   sr_free_vector(y);
   sr_free_matrix(p);
   fprintf(STDERR, "*** error (verstat): allocation failure\n");
   return 1;
 }

 fprintf(STDOUT,"\n");

 faux = y[1];
 for (i_par = 2; i_par <= mpar; i_par++ )
 { faux = MIN(faux, y[i_par]); }
 fprintf(STDOUT,"min: %7.4f :", faux);

 for (j_par = 1; j_par <= ndim; j_par++ )
 {
   faux = p[1][j_par];
   for (i_par = 2; i_par <= mpar; i_par++ )
   {  faux = MIN(faux, p[i_par][j_par]); }
   fprintf(STDOUT," %6.3f", faux);
 }
 fprintf(STDOUT,"\n");

 faux = y[1];
 for (i_par = 2; i_par <= mpar; i_par++ )
 { faux = MAX(faux, y[i_par]); }
 fprintf(STDOUT,"max: %7.4f :", faux);

 for (j_par = 1; j_par <= ndim; j_par++ )
 {
   faux = p[1][j_par];
   for (i_par = 2; i_par <= mpar; i_par++ )
   {  faux = MAX(faux, p[i_par][j_par]); }
   fprintf(STDOUT," %6.3f", faux);
 }
 fprintf(STDOUT,"\n\n"); 

/***********************************************************************
  Find average values
***********************************************************************/

 faux = 0.;
 for (i_par = 1; i_par <= mpar; i_par++ )
 { faux += y[i_par]; }
 avg[0] = faux/mpar;
 fprintf(STDOUT,"avg: %7.4f :", avg[0]);

 for (j_par = 1; j_par <= ndim; j_par++ )
 {
   faux = 0.;
   for (i_par = 1; i_par <= mpar; i_par++ )
   { faux += p[i_par][j_par]; }
   avg[j_par] = faux/mpar;
   fprintf(STDOUT," %6.3f", avg[j_par]);
 }
 fprintf(STDOUT,"\n");

/***********************************************************************
  Calculate standard deviation
***********************************************************************/

 dev[0] = 0.;
 for (i_par = 1; i_par <= mpar; i_par++ )
 { 
   faux = y[i_par] - avg[0];
   dev[0] += faux*faux; 
 }
 dev[0] = R_sqrt(dev[0] / mpar);
 fprintf(STDOUT,"dev: %7.4f :", dev[0]);
 
 for (j_par = 1; j_par <= ndim; j_par++ )
 {
   dev[j_par] = 0.;
   for (i_par = 1; i_par <= mpar; i_par++ )
   { 
     faux = p[i_par][j_par] - avg[j_par]; 
     dev[j_par] += faux*faux;
   }
   dev[j_par] = R_sqrt(dev[j_par] / mpar);
   fprintf(STDOUT," %6.3f", dev[j_par]);
 }
 fprintf(STDOUT,"\n");

 sr_free_vector(dev);
 sr_free_vector(avg);
 sr_free_vector(y);
 sr_free_matrix(p);

 return 0;
}  /* end of main */

/**********************************************************************/
