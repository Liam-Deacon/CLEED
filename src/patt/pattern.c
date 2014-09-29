/****************************************************************************
                        pattern.c
FUNCTIONS
	patt_read(FILE *, pattern *) - read LEED pattern from input file.   

RETURNS
    0: success
   -1: read failure

INPUT FILE
	The input file specifies the coordinate system, substrate (GS) and
superstructure (SS), as well as any SS domains. The file has the following
format:
	* lines beginning with '#' are ignored by the program.
	* lines beginning with 'c' are treated as comments, note that
	  the first 'c' comment block will be printed as a legend.
	* real space lattice vectors a1 & a2 have the form: <x> <y> a[1-2]
	* radius is the maximum radius of Ewald construction in units of: a[1-2]
    * number of domains
    * superstructure matrix M1 & M2 of the form: <x> <y> M[1-2]
    * domain mirroring or rotation: Sx, Sy or R<#>

BUGS
	Please report any bugs (with a description on how to reproduce
the problem) to liam.deacon@diamond.ac.uk

AUTHOR(S)
	Liam Deacon <liam.deacon@diamond.ac.uk>

CHANGE-LOG
2014-07-13/LD - initial implementation

***************************************************************************/

#include "pattern.h"
#include "patt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <math.h>


pattern_t *pattern_alloc(size_t n_domains)
{
  pattern_t *pat = (pattern_t*) malloc(sizeof(pattern_t));
  if (pat == NULL) return NULL;
  if (n_domains > 0)
  {
    pat->M_SS = (matrix_2x2_t*) malloc(sizeof(matrix_2x2_t) * (n_domains+1));
    if (pat->M_SS == NULL)
    {
      free(pat);
      return NULL;
    }
  }
  else {
    pat->M_SS = NULL;
  }
  pat->n_domains = n_domains;
  return(pat);
}

pattern_t *pattern_init(size_t n_domains)
{ 
  pattern_t *pat = (pattern_t*) malloc(sizeof(pattern_t));
  if (pat == NULL)
    return NULL;
  
  pat->M_SS = (matrix_2x2_t*) malloc(sizeof(matrix_2x2_t) * (n_domains+1));
  if (pat->M_SS == NULL)
  {
    free(pat);
    return NULL;
  }
  
  pat->n_domains = n_domains;
  pat->title = "\0";
  pat->radius = 1.;
  pat->a1.x = 0.;
  pat->a1.y = 0.;
  pat->a2.x = 0.;
  pat->a2.y = 0.;
  
  return(pat);
}

void pattern_free(pattern_t *pat)
{
  if (pat->title != NULL)
  {
    free(pat->title);
  }
  if (pat->M_SS != NULL) free(pat->M_SS);
  free(pat);
  pat = NULL;
}

double pattern_get_radius(pattern_t *pat)
{ return(pat->radius);}

void pattern_set_a1(pattern_t *pat, basis_vector_t *a1)
{ pat->a1.x = a1->x; pat->a1.y = a1->y;}

void pattern_set_a2(pattern_t *pat, basis_vector_t *a2)
{ pat->a2.x = a2->x; pat->a2.y = a2->y;}

void pattern_set_radius(pattern_t *pat, double radius)
{ pat->radius = radius;}

int pattern_set_max_domains(pattern_t *pat, size_t n_domains)
{ 
  char *str = pat->title;
  pattern_t *t = pat;
  pattern_t *tmp;
  
  if (n_domains > pat->n_domains) /* grow the number of elements */
  {
    matrix_2x2_t *temp = pat->M_SS;
    (matrix_2x2_t*) realloc(pat->M_SS, n_domains*sizeof(matrix_2x2_t));
    
    /* check if realloc successful */
    if (pat->M_SS != NULL)
      pat->n_domains = n_domains;
    else
    {
      pat->M_SS = temp;
      return(-3);
    }
    
    tmp = realloc(pat, sizeof(pattern_t) + (n_domains*sizeof(matrix_2x2_t)));
    if (tmp == NULL)
    {
      pat = t;
      return(-2);
    }
    
    pat->title = str;
    
    return(0);
  } else {
    return(0);
  }
  return(-1);
}

size_t pattern_get_n_domains(pattern_t *pat)
{ return(pat->n_domains);}

void pattern_set_title(pattern_t *pat, char *title)
{
  if (pat->title != NULL) free(pat->title);
  pat->title = (char *)malloc(sizeof(char) * strlen(title));
  strcpy(pat->title, title);
}

const char *pattern_get_title(pattern_t *pat)
{ return(pat->title);}

pattern_t *pattern_read(FILE *file)
{
  pattern_t *pat;
  basis_vector_t a1;
  basis_vector_t a2;
  double radius;
  size_t n_dom;
  size_t i_dom;
  int err;
  
  /* SS matrix variables */
  double aux2;
  double det;
  int is_stdin = 0;
  double phi;                               /* Rotation angle */
  double M11=1., M12=0., M21=0., M22=1.;    /* SS matrix */
  double N11=0., N12=0., N21=0., N22=0.;    /* Rotation  matrix */
  double m11, m12, m21, m22;
  
  char *line_buffer = (char*) malloc(STRSZ*sizeof(char));
  char *comments = (char*) malloc(STRSZ*sizeof(char));

/* check input file is open for reading */
  if (file == NULL) return NULL;  /* cannot read file */
  
  if (fileno(file) == fileno(stdin)) is_stdin = 1;
  
  if (is_stdin) printf("Enter comment lines starting with 'c'\n");
  
  line_buffer[0] = '\0';
  comments[0] = '\0';
  
  /* skip comment lines */
  while ((fgets(line_buffer, STRSZ, stdin) != NULL) && 
         (line_buffer[0] == '#' || line_buffer[0] == 'c'))
  {  
    switch (*line_buffer)
    {
      case '#':
        break;
      
      case 'c':
        if (strlen(comments) == 0) {
          strcpy(comments, line_buffer);
        } else {
          (char *) realloc(comments, strlen(comments)+STRSZ);
          strcat(comments, line_buffer);
        }
        break;
        
      default:
        break;
    }
  }
  
/*
 GS SPOTS 
*/

  /* read real space lattice vectors */
  if (is_stdin) printf("Enter vector a1 (x y): ");
    
  fget_nocomm(line_buffer, file, stdout);
  sscanf(line_buffer, "%lf %lf", &a1.x, &a1.y);
  
  if (is_stdin) printf("Enter vector a2 (x y): ");
    
  fget_nocomm(line_buffer, file, stdout);
  sscanf(line_buffer, "%lf %lf", &a2.x, &a2.y);
  
  /* 
  calculate max length in k-space (radius is the max. k-distance in
  units of the longest rec. lattice vector) 
  rescale spot size
  */
  if (is_stdin) printf("Enter radius: ");
    
  fget_nocomm(line_buffer, file, stdout);
  sscanf(line_buffer, "%lf", &radius);
  
/*
 SS SPOT
*/

  /* Domains */  
  if (is_stdin) printf("Enter no. domains: ");
    
  fget_nocomm(line_buffer, file, stdout);
  sscanf(line_buffer, "%u", &n_dom);

  /* allocate memory for pattern */
  pat = pattern_init(n_dom);
  err = pattern_set_max_domains(pat, n_dom); 
  if (err)
  {
    printf("error: %i\n", err);
    exit(err);
  }

  /* set GS values */
  pattern_set_title(pat, comments);
  pattern_set_radius(pat, radius);
  pattern_set_a1(pat, &a1);
  pattern_set_a2(pat, &a2);
  
  /* loop over domains */
  for (i_dom = 0; i_dom < n_dom; i_dom++)
  {
    if (is_stdin) 
    {
      printf("Enter domain %u parameters\n(may be rotation 'R<num>', 'S', "
             "or a 2x2 Matrix spread over two lines):\n", i_dom+1);
    }
    fget_nocomm(line_buffer, file, stdout);
    
    switch (*line_buffer)
    {
      case 'R':
      {
        sscanf(line_buffer+1, "%lf", &phi);
        phi *= 0.0174532925;
        det  =  a1.x*a2.y - a1.y*a2.x;
        aux2 = (a1.x*a2.x + a1.y*a2.y) / det;

        N11 =  cos(phi) - aux2*sin(phi);
        N12 =  sin(phi) * (a1.x*a1.x + a1.y*a1.y) / det;
        N21 = -sin(phi) * (a2.x*a2.x + a2.y*a2.y) / det;
        N22 =  cos(phi) + aux2*sin(phi);

        m11 = M11*N11 + M12*N21;
        m12 = M11*N12 + M12*N22;
        m21 = M21*N11 + M22*N21;
        m22 = M21*N12 + M22*N22;

        M11 = m11; 
        M12 = m12; 
        M21 = m21; 
        M22 = m22;

        break;
      } /* case 'R' */

      case 'S':
      {
        det  =  a1.x*a2.y - a1.y*a2.x;
        aux2 = (a1.x*a2.y + a1.y*a2.x) / det;

        if (line_buffer[1] == 'x')
        {
          N11 = aux2;
          N12 = -2 * a1.x*a1.y / det;
          N21 =  2 * a2.x*a2.y / det;
          N22 = -aux2;
        }
        if (line_buffer[1] == 'y')
        {
          N11 = - aux2;
          N12 =  2 * a1.x*a1.y / det;
          N21 = -2 * a2.x*a2.y / det;
          N22 = aux2;
        }

        m11 = M11*N11 + M12*N21;
        m12 = M11*N12 + M12*N22;
        m21 = M21*N11 + M22*N21;
        m22 = M21*N12 + M22*N22;

        M11 = m11; 
        M12 = m12; 
        M21 = m21; 
        M22 = m22;

        break;
      } /* case 'S' */

      default:
      {
        sscanf(line_buffer, "%lf %lf", &M11, &M12);
        fget_nocomm(line_buffer, file, stdout);
        sscanf(line_buffer, "%lf %lf", &M21, &M22);

        m11 = M11; 
        m12 = M12; 
        m21 = M21; 
        m22 = M22;

        N11 = N22 = 1.; 
        N12 = N21 = 0.;
        
        break;
      } /* default */
      
    } /* switch */
    
    /* add superstructure matrix */
    matrix_2x2_set_all(&pat->M_SS[i_dom], m11, m12, m21, m22);
    
  } /* i_dom */
  
  return(pat);
}

void pattern_printf(FILE *stream, pattern_t *pat)
{
  size_t i;

  if (pat == NULL) return;
 
  fprintf(stream, "%s\n", pat->title);
  fprintf(stream, "a1 = (%9.6f, %9.6f)\n", pat->a1.x, pat->a1.y);
  fprintf(stream, "a2 = (%9.6f, %9.6f)\n", pat->a2.x, pat->a2.y);
  fprintf(stream, "radius = %9.6f\n", pat->radius);
  fprintf(stream, "n_domains = %u\n", pat->n_domains);
  for (i =0; i < pat->n_domains; i++)
  { 
    fprintf(stream, "\ndomain %u:\n", i); 
    matrix_2x2_printf(stream, &pat->M_SS[i]);
  }
}

void pattern_set_superstructure_matrix(pattern_t *pat, 
        matrix_2x2_t *mat, size_t domain)
{
  if (domain >= pat->n_domains) return;
  pat->M_SS[domain].M11 = mat->M11; 
  pat->M_SS[domain].M12 = mat->M12; 
  pat->M_SS[domain].M21 = mat->M21;
  pat->M_SS[domain].M22 = mat->M22;
}

const matrix_2x2_t *get_superstructure_matrix(const pattern_t *pat, size_t domain)
{
  if (domain < pat->n_domains) return(&pat->M_SS[domain]);
  else return(NULL);
}

bool pattern_domain_is_commensurate(const pattern_t *pat, size_t domain)
{
  if (domain > pat->n_domains) return(false);
  else
  {
    double m11, m12, m21, m22;
    m11 = pat->M_SS[domain].M11;
    m12 = pat->M_SS[domain].M12;
    m21 = pat->M_SS[domain].M21;
    m22 = pat->M_SS[domain].M22;
    return(!( ( fabs(m11) - (int)(fabs(m11)+0.1) > 0.05) ||
              ( fabs(m12) - (int)(fabs(m12)+0.1) > 0.05) || 
              ( fabs(m21) - (int)(fabs(m21)+0.1) > 0.05) || 
              ( fabs(m22) - (int)(fabs(m22)+0.1) > 0.05) ));
  }
}

/*! \fn spots_t *pattern_calculate_substrate_spots(const pattern_t *pat)
 *  \brief Calculate LEED spots substrate 
 *  \param *pat Pointer to pattern_t structure.
 *
 */ 
spots_t *pattern_calculate_substrate_spots(const pattern_t *pat)
{
  spots_t *spots;
  double a1[2];
  double a2[2];
  double xi, yi;
  double radius = pat->radius;
  /* double spot_radius = SPOT_GS; */
  
  int h, k, h_max, k_max;
  size_t i_spot = 0, n_spots = 0;

  /* scale spot sizes by radius */
  /* if (rg_flag == true) spot_radius /= radius; */
 
  if( (pat->a1.x*pat->a1.x + pat->a1.y*pat->a1.y) > 
            (pat->a2.x*pat->a2.x + pat->a2.y*pat->a2.y))
  {
    radius *= sqrt(pat->a1.x*pat->a1.x + pat->a1.y*pat->a1.y);
  }
  else
  {
    radius *= sqrt(pat->a2.x*pat->a2.x + pat->a2.y*pat->a2.y);
  }
  
  radius = MAX_RADIUS / radius;

  /* calculate GS vectors */
  a1[0] =  radius*pat->a2.y;
  a1[1] = -radius*pat->a2.x;
  a2[0] = -radius*pat->a1.y;
  a2[1] =  radius*pat->a1.x;
  
  #if (DEBUG == 1)
  printf("a1: %.1f %.1f\n", a1[0], a1[1]);
  printf("a2: %.1f %.1f\n", a2[0], a2[1]);
  #endif 
 
  /* define max. values */
  h_max = 5 * (int)( MAX_RADIUS / sqrt(a1[0]*a1[0] + a1[1]*a1[1]) );
  k_max = 5 * (int)( MAX_RADIUS / sqrt(a2[0]*a2[0] + a2[1]*a2[1]) );
  
  /* first pass to calculate number of spots */
  for (h = -h_max; h <= h_max; h ++)
  {
    for (k = -k_max; k <= k_max; k ++)
    {
      xi = h * a1[0] + k * a2[0];
      yi = h * a1[1] + k * a2[1];
      if ( ((xi*xi + yi*yi) <= MAX_RADIUS*MAX_RADIUS) || 
            (pattern_is_square(pat))) n_spots++;
    }
  }
  /* initialise spots */
  spots = spots_init(n_spots);
  if (spots == NULL) return NULL;
  
  /* calculate substrate spots */
  for (h = -h_max; h <= h_max; h ++)
  {
    for (k = -k_max; k <= k_max; k ++)
    {
      xi = h * a1[0] + k * a2[0];
      yi = h * a1[1] + k * a2[1];
      if ( ((xi*xi + yi*yi) <= MAX_RADIUS*MAX_RADIUS) || 
            (pattern_is_square(pat)))
      {
        spots->spots[i_spot].index.h = h;
        spots->spots[i_spot].index.k = k;
        spots->spots[i_spot].x = xi;
        spots->spots[i_spot].y = yi;
        i_spot++;
      }
        
    }
  }
  
  return(spots);
  
}

spots_t *pattern_calculate_superstructure_spots(const pattern_t *pat,
                                                size_t domain)
{
  spots_t *spots;
  double a1[2], a2[2];                /* substrate basis vectors */
  double b1[2], b2[2];                /* superstructure basis vectors */
  double xi, yi;
  double x=0, y=0;
  double aux1;
  float det;                          /* Det of SS-matrix */
  int smax_1, smax_2;
  double ind_1, ind_2;                /* SS index */
  double m11, m12, m21, m22;
  double radius = pat->radius;
  double spot_radius = SPOT_GS;
  bool commensurate;
  
  int h, k, h_max, k_max;
  int s1, s2;
  size_t i_spot = 0, n_spots = 0;

  bool rs_flag = false;

  if (domain > pat->n_domains) 
  {
    return NULL; /* invalid domain index */
  }
  
  /* scale spot sizes by radius */
  if (rs_flag == true)
    spot_radius /= radius;
 
  if( (pat->a1.x*pat->a1.x + pat->a1.y*pat->a1.y) > 
            (pat->a2.x*pat->a2.x + pat->a2.y*pat->a2.y))
  {
    radius *= sqrt(pat->a1.x*pat->a1.x + pat->a1.y*pat->a1.y);
  }
  else
  {
    radius *= sqrt(pat->a2.x*pat->a2.x + pat->a2.y*pat->a2.y);
  }
  
  radius = MAX_RADIUS / radius;

  /* calculate GS vectors */
  a1[0] =  radius*pat->a2.y;
  a1[1] = -radius*pat->a2.x;
  a2[0] = -radius*pat->a1.y;
  a2[1] =  radius*pat->a1.x;

  /* define max. values */
  h_max = 5 * (int)( MAX_RADIUS / sqrt(a1[0]*a1[0] + a1[1]*a1[1]) );
  k_max = 5 * (int)( MAX_RADIUS / sqrt(a2[0]*a2[0] + a2[1]*a2[1]) );

  /*
   * SS SPOT
   */
   
  /* set local superstructure matrix elements */
  m11 = pat->M_SS[domain].M11;
  m12 = pat->M_SS[domain].M12;
  m21 = pat->M_SS[domain].M21;
  m22 = pat->M_SS[domain].M22;

  /* check whether commensurate superstructure */
  commensurate = pattern_domain_is_commensurate(pat, domain);

  /* determinant of matrix */
  det  = m11*m22 - m12*m21;
  aux1 = 1/det;
  det = fabs(det);

  /* calculate SS vectors */
  b1[0] = aux1 *(m22*a1[0] - m21*a2[0]);
  b1[1] = aux1 *(m22*a1[1] - m21*a2[1]);
  b2[0] = aux1 *(m11*a2[0] - m12*a1[0]);
  b2[1] = aux1 *(m11*a2[1] - m12*a1[1]);

  /* 
   * SS spots 
   */
   
  /* define max. values */
  smax_1 = 5* (int)( MAX_RADIUS / sqrt(b1[0]*b1[0] + b1[1]*b1[1]) );
  smax_2 = 5* (int)( MAX_RADIUS / sqrt(b2[0]*b2[0] + b2[1]*b2[1]) );

  /* determine number of spots */
  for (s1 = -smax_1; s1 < smax_1; s1 ++)
  {
    for (s2 = -smax_2; s2 < smax_2; s2 ++)
    {
      if (((x*x + y*y) <= MAX_RADIUS*MAX_RADIUS) || (pattern_is_square(pat)))
      {
        n_spots++;
      }
    }
  }
  
  /* initialise spots */
  spots = spots_init(n_spots);

  if (!commensurate)
  {
  
    /* write SS spot positions to output */
    for (s1 = -smax_1; s1 < smax_1; s1 ++)
    {
      for (s2 = -smax_2; s2 < smax_2; s2 ++)
      {
        x = s1 * b1[0] + s2 * b2[0];
        y = s1 * b1[1] + s2 * b2[1];
        if ( ((x*x + y*y) <= MAX_RADIUS*MAX_RADIUS) || (pattern_is_square(pat))) 
        {

          ind_1 = (s1*m22 - s2*m12);
          ind_2 = (s2*m11 - s1*m21);
          /*
           * write indices
           */

          spots->spots[i_spot].index.h = ind_1;
          spots->spots[i_spot].index.k = ind_2;
          spots->spots[i_spot].x = x;
          spots->spots[i_spot].y = y;
          i_spot++;
        }
 
      }
    }
  
  }
/* 
 * If the Mii are not integer, the superstructure is incommensurate.
 * In this case the multiple scattering spots must be calculated
 * separately.
 */

  if (!commensurate)
  {
    /* add multiple scattering SS spots to list */
    for (h = -h_max; h <= h_max; h ++)
    {
      for (k = -k_max; k <= k_max; k ++)
      {
        if( (h != 0) || (k != 0) )
        {
          xi = h * a1[0] + k * a2[0];
          yi = h * a1[1] + k * a2[1];
          
          if( (xi*xi + yi*yi) <= MAX_RADIUS*MAX_RADIUS) 
          {
            for (s1 = -smax_1; s1 < smax_1; s1 ++)
            {
              for (s2 = -smax_2; s2 < smax_2; s2 ++)
              {
                x = xi + s1 * b1[0] + s2 * b2[0];
                y = yi + s1 * b1[1] + s2 * b2[1];
              
                if( ((x*x + y*y) <= MAX_RADIUS*MAX_RADIUS) &&
                    ((s1 != 0) || (s2 != 0)) )
                {
                  spots->spots[i_spot].index.h = h;
                  spots->spots[i_spot].index.k = k;
                  spots->spots[i_spot].x = x;
                  spots->spots[i_spot].y = y;
                  i_spot++;
                  
                /*
                  fprintf(out_stream, " %.1f %.1f %.1f 0 360 "
                          "arc gsave %s %s grestore stroke\n", 
                          x, y, spot_ss, ps_color(color_flag, ii, 
                          ifiles, SPOT_SS, i_dom), fill_ss);
                */
                }
        
              } /* for s2 */
            } /* for s1 */
          }  /* if < MAX_RADIUS */
        }   /* if != 0  */
      }    /* for i1/2 */
    }
  }     /* if fabs .. */
  #if (DEBUG == 1)
  else printf("commensurate\n");
  #endif
  
  return(spots);
  
}

bool pattern_is_square(const pattern_t *pat)
{
  return(pat->square);
}
