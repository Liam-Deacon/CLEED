/*********************************************************************
 *                        PATTERN.C
 *
 *  Copyright 2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   2014-07-13/LD - initial implementation
 ***************************************************************************/

/*! \file
 *  \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * File contains functions for handling #pattern objects.
 */

#include "pattern.h"
#include "patt.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <math.h>

/*!
 * Allocates memory for #pattern instance with \p n_domains number of domains.
 *
 * \param n_domains The number of superstructure domains to allocate to
 * @pattern::M_SS member of #pattern instance.
 * \return Pointer to the #pattern instance.
 * \retval \c NULL if memory cannot be allocated.
 */
pattern *pattern_alloc(size_t n_domains)
{
  pattern *pat = (pattern*) malloc(sizeof(pattern));
  if (pat == NULL) return NULL;
  if (n_domains > 0)
  {
    pat->M_SS = (matrix_2x2*) malloc(sizeof(matrix_2x2) * (n_domains+1));
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

/*!
 * Allocates memory for #pattern instance with \p n_domains number of domains
 * and initializes its members to suitable default values.
 *
 * \param n_domains The number of superstructure domains to allocate to
 * @pattern::M_SS member of #pattern instance.
 * \return Pointer to the #pattern instance.
 * \retval \c NULL if memory cannot be allocated.
 */
pattern *pattern_init(size_t n_domains)
{ 
  pattern *pat = (pattern*) malloc(sizeof(pattern));
  if (pat == NULL)
    return NULL;
  
  pat->M_SS = (matrix_2x2*) malloc(sizeof(matrix_2x2) * (n_domains+1));
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

/*!
 * Frees the #pattern instance \p pat from memory. It also removes its members
 * @pattern::title and @pattern::M_SS from the memory stack.
 *
 * \param pat Pointer to #pattern instance to free from memory.
 */
void pattern_free(pattern *pat)
{
  if (pat->title != NULL)
  {
    free(pat->title);
  }
  if (pat->M_SS != NULL) free(pat->M_SS);
  free(pat);
  pat = NULL;
}

/*!
 * Returns the @pattern::radius value of \p pat
 *
 * \param[in] pat #pattern instance
 * \return radius of \p pat
 */
inline double pattern_get_radius(const pattern *pat)
{ return(pat->radius);}

/*!
 * Assigns (x,y,z) of \p a1 to @pattern::a1 of \p pat .
 *
 * \param[in,out] pat #pattern instance to modify.
 * \param[in] a1 The basis vector \f$ \vec{a_1} \f$
 */
inline void pattern_set_a1(pattern *pat, const basis_vector *a1)
{ pat->a1.x = a1->x; pat->a1.y = a1->y;}

/*!
 * Assigns (x,y,z) of \p a2 to @pattern::a2 of \p pat .
 *
 * \param[in,out] pat #pattern instance to modify.
 * \param[in] a2 The basis vector \f$ \vec{a_2} \f$
 */
inline void pattern_set_a2(pattern *pat, const basis_vector *a2)
{ pat->a2.x = a2->x; pat->a2.y = a2->y;}

/*!
 * Assigns the @pattern::radius member of \p pat to the value of \p radius
 *
 * \param[in,out] pat #pattern instance to modify.
 * \param radius Default (global) radius of spots in pattern.
 */
inline void pattern_set_radius(pattern *pat, double radius)
{ pat->radius = radius;}

/*!
 * Assigns the maximum number of domains for the #pattern instance \p pat
 *
 * \param[in,out] pat #pattern instance to modify.
 * \param n_domains Maximum number of domains for \p pat
 * \return Integer representing function success.
 * \retval 0 Given on successful completion of function.
 * \retval 1 If memory cannot be allocated for @pattern::M_SS of \p pat
 * \retval 2 If memory of \p pat cannot be reallocated.
 */
int pattern_set_max_domains(pattern *pat, size_t n_domains)
{ 
  char *str = pat->title;
  pattern *t = pat;
  pattern *tmp;
  
  if (n_domains > pat->n_domains) /* grow the number of elements */
  {
    matrix_2x2 *temp = pat->M_SS;
    (matrix_2x2*) realloc(pat->M_SS, n_domains*sizeof(matrix_2x2));
    
    /* check if realloc successful */
    if (pat->M_SS != NULL)
      pat->n_domains = n_domains;
    else
    {
      pat->M_SS = temp;
      return(1);
    }
    
    tmp = realloc(pat, sizeof(pattern) + (n_domains*sizeof(matrix_2x2)));
    if (tmp == NULL)
    {
      pat = t;
      return(2);
    }
    
    pat->title = str;
    
  }

  return(0);
}

/*!
 * Returns the number of superstructure domains.
 *
 * \param[in] pat #pattern instance containing domain information.
 * \return number of superstructure domains of \p pat
 */
inline size_t pattern_get_n_domains(const pattern *pat)
{ return(pat->n_domains);}

/*!
 * Assigns the title string of \p pat
 *
 * \param[in,out] pat
 * \param[in] title string to assign to @pattern::title of \p pat
 * \note If @pattern::title pointer of \p pat is \c NULL then nothing will
 * be done.
 * \warning If the pointer to @pattern::title of \p pat is already initialized
 * then the memory will be freed before the pointer is reassigned.
 */
void pattern_set_title(pattern *pat, const char *title)
{
  if (pat->title != NULL) free(pat->title);
  pat->title = (char *)malloc(sizeof(char) * strlen(title));

  if (pat->title) strcpy(pat->title, title);
}

/*!
 * Returns the title string of \p pat
 *
 * \param pat #pattern instance to query.
 * \return pointer to @pattern::title of \p pat
 */
inline const char *pattern_get_title(const pattern *pat)
{ return((const char*)pat->title);}

/*!
 * Reads \p file to determine the LEED pattern parameters and returns a
 * #pattern instance corresponding to the data that is read.
 *
 * The input file specifies the coordinate system, substrate (GS) and
 * superstructure (SS), as well as any SS domains. The file has the following
 * format:
 * - lines beginning with '#' are ignored by the program.
 * - lines beginning with 'c' are treated as comments, note that
 *   the first 'c' comment block will be printed as a legend.
 * - real space lattice vectors \f$ \vec{a_1} \f$ & \f$ \vec{a_2} \f$
 *   have the form: \code <x> <y> a[1-2] \endcode
 * - radius of the maximum radius of Ewald construction in units of
 *   \c a1 or \c a2
 * - number of domains
 * - superstructure matrix M1 & M2 of the form: \code <x> <y> M[1-2] \endcode
 * - domain mirroring or rotation: \c Sx, \c Sy or \c R<#>
 *
 * \param file File pointer to read from e.g. \c stdin or an file opened with
 * fopen()
 * \return #pattern instance with members set to the values specified in \p file
 */
pattern *pattern_read(FILE *file)
{
  pattern *pat;
  basis_vector a1;
  basis_vector a2;
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
  
  /* GS SPOTS */

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
  
  /* SS SPOTS */

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

/*!
 * Prints member information of #pattern instance \p pat to the file \p stream
 *
 * \param stream File pointer to write to e.g. \c stdout
 * \param[in] pat #pattern instance to display.
 */
void pattern_printf(FILE *stream, const pattern *pat)
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

/*!
 * Assigns the superstructure matrix \p mat to \p pat for a given \p domain
 *
 * \param pat Pointer to LEED #pattern to be modified.
 * \param mat Pointer to the superstructure matrix.
 * \param domain Index for a given superstructure of \p pat
 */
void pattern_set_superstructure_matrix(pattern *pat, 
        const matrix_2x2 *mat, size_t domain)
{
  if (domain >= pat->n_domains) return;
  pat->M_SS[domain].M11 = mat->M11; 
  pat->M_SS[domain].M12 = mat->M12; 
  pat->M_SS[domain].M21 = mat->M21;
  pat->M_SS[domain].M22 = mat->M22;
}

inline const matrix_2x2 *get_superstructure_matrix(const pattern *pat, size_t domain)
{
  if (domain < pat->n_domains) return(&pat->M_SS[domain]);
  else return(NULL);
}

/*!
 * Determines whether the superstructure LEED pattern given by \p domain is
 * commensurate.
 *
 * \param pat Pointer to the #pattern instance containing all the superstructure
 * matrix for the given \p domain
 * \param domain The superstructure domain to query.
 * \return boolean representing whether the domain is commensurate or not.
 */
bool pattern_domain_is_commensurate(const pattern *pat, size_t domain)
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

/*!
 * Calculates the substrate spots in LEED #pattern \p pat . It calculates
 * the diffraction spot positions for a given (h,k) Miller index and adds them
 * to the list of spots if a given spot is within the Ewald sphere (the
 * @pattern::radius ).
 *
 * \param pat pattern instance containing all the parameters needed for the
 * calculations.
 * \return #spots instance of substrate spots for \p pat
 */
spots *pattern_calculate_substrate_spots(const pattern *pat)
{
  spots *spots;
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

/*!
 * Calculates the superstructure spots in LEED #pattern \p pat . It calculates
 * the diffraction spot positions for a given (h,k) Miller index and adds then
 * to the list of spots if a given spot is within the Ewald sphere (the
 * @pattern::radius ).
 *
 * \param pat pattern instance containing all the parameters needed for the
 * calculations.
 * \param domain The superstructure domain index to calculate spots for.
 * \return #spots instance of superstructure spots for the given
 * \p domain of \p pat
 */
spots *pattern_calculate_superstructure_spots(const pattern *pat, size_t domain)
{
  spots *spots;
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

/*!
 * Returns whether \p pat is a square pattern or a circular one.
 *
 * \param[in] pat #pattern instance to query.
 * \return boolean whether \p pat pattern is square.
 */
inline bool pattern_is_square(const pattern *pat)
{ return(pat->square); }
