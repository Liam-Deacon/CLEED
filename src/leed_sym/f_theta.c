#include <stdio.h>
#include <strings.h>

#include "gh_stddef.h"
#include "leed.h"

#define VERSION 1.0

char line_buffer[STRSZ];
/*========================================================================*/

ftheta(int argc, char *argv[])
{

mat Ylm;

int i_str, i_arg;
int l, l_max;
int i_del, i_ylm;
int iaux;

real energy;
real delta2[21];
real theta, f_theta_r, f_theta_i, cos_theta;
real norm;
real faux_r, faux_i;

char * in_file;

FILE *in_stream;
FILE *out_stream;

/* Preset out_stream, l_max */
 in_stream = stdin;
 out_stream = stdout;
 l_max = 5;
 energy = 100. / HART;

/*
 Check command line and decode arguments
*/
 for (i_arg = 1; i_arg < argc; i_arg++)
 {
  if(*argv[i_arg] != '-')
  {
   printf("\n\tsyntax error:\n");
   printf("\tusage: \tf_theta -i <input> -o <output> -e <energy> -l<l_max>\n");
   exit(1);
  }
  else
  {
   if(strncmp(argv[i_arg], "-i", 2) == 0)
   {
/* Open input file */
    i_arg++;
    in_file = argv[i_arg];
    if ((in_stream = fopen(argv[i_arg],"r")) == NULL)
    {
     printf("open failed: %s",argv[i_arg]);
     exit(1);
    }
   }
/* Open output file */
   if(strncmp(argv[i_arg], "-o", 2) == 0)
   {
    i_arg++;
    close(out_stream); 
    if ((out_stream = fopen(argv[i_arg],"w")) == NULL)
    {
     printf("open failed: %s",argv[i_arg]);
     exit(1);
    }
   }
/* read l_max and energy */
   if(strncmp(argv[i_arg], "-e", 2) == 0)
   {
    i_arg++;
    energy = (real)atof(argv[i_arg]) / HART;
   }
   if(strncmp(argv[i_arg], "-l", 2) == 0)
   {
    i_arg++;
    l_max = atoi(argv[i_arg]);
   }
  } /* else */
 }  /* for i_arg */

 fprintf(out_stream,"# F_THETA: calculate |f(theta)|^2 (version %3.1f)\n",
         VERSION);

/* read first line in input file (and compare with l_max) */
 fgets(line_buffer, STRSZ, in_stream);
 sscanf(line_buffer, "%d %d", &i_str, &iaux);
 if( l_max > iaux) l_max = iaux;
 fprintf(out_stream,"# l_max: %d\n", l_max);
 fprintf(stdout,"# l_max: %d\n", l_max);

 while (fgets(line_buffer, STRSZ, in_stream) != NULL )
 {
#ifdef CLEED_REAL_IS_DOUBLE
       sscanf(line_buffer, "%le", &faux_r);
#endif
#ifdef CLEED_REAL_IS_FLOAT
       sscanf(line_buffer, "%e", &faux_r);
#endif
/*
   fprintf(stdout,"# test energy: %.2f\n", faux_r);
*/
   fgets(line_buffer, STRSZ, in_stream);
   if( faux_r >= energy)
   {
     energy = faux_r;
     fprintf(out_stream,"# energy: %.2f\n", faux_r);
     fprintf(stdout,"# energy: %.2f H", faux_r);
     faux_r *= HART;
     fprintf(stdout," = %.2f eV\n", faux_r);
     for( i_str = 0, l = 0; l <= l_max; l++)
     {
#ifdef CLEED_REAL_IS_DOUBLE
       sscanf(line_buffer + i_str, "%le", delta2+l);
#endif
#ifdef CLEED_REAL_IS_FLOAT
       sscanf(line_buffer + i_str, "%e", delta2+l);
#endif
       delta2[l] *= 2.;
       while((line_buffer[i_str]==' ') || (line_buffer[i_str]=='-')) i_str ++;
       while((line_buffer[i_str]!=' ') && (line_buffer[i_str]!='-')) i_str ++;
     }
     break;
   }
 }

 fprintf(stdout,"#");
 for(l = 0; l <= l_max; l ++)
 fprintf(stdout,":%.3f", delta2[l] * 0.5);
 fprintf(stdout,":\n");


 Ylm = NULL;
 for( theta = 0; theta <= 180.; theta += 1.)
 {
   cos_theta = R_cos(theta*DEG_TO_RAD);
   Ylm = r_ylm( Ylm, cos_theta, 0, l_max);
  
   for(f_theta_r = 0., f_theta_i = 0., l = 0, i_ylm = 1; 
       l <= l_max; l ++, i_del ++, i_ylm += 2*l)
   {
     faux_r = (2*l + 1);
     faux_r = R_sqrt(faux_r);

     faux_i = faux_r * (R_cos(delta2[l]) - 1.);
     faux_r = - faux_r *  R_sin(delta2[l]);
     faux_i *= Ylm->rel[i_ylm];
     faux_r *= Ylm->rel[i_ylm];

     f_theta_r += faux_r;
     f_theta_i += faux_i;
   }
/*
   printf("theta: %.1f f_theta: %.3f \n", theta, f_theta);
*/
   if (theta == 0.) 
   {
     norm = f_theta_r*f_theta_r + f_theta_i*f_theta_i;
     f_theta_r = 1.;
   }
   else
   {
     f_theta_r = f_theta_r*f_theta_r + f_theta_i*f_theta_i;
     f_theta_r /= norm;
   }
   fprintf(out_stream, " %e %e\n", theta, f_theta_r);
 }
}
