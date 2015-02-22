/*********************************************************************
 *                        SRRDVER.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/11.09.95 - Creation
 *   GH/28.09.95 - Do not check dimensions if n_dim is -1.
 *   GH/23.10.95 - fix bug in the fgets calls (n_str instead of STRSZ)
 *   LD/30.06.14 - modified for GSL input (enable by adding USE_GSL
 *                 to defines when compiling).
 *********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Read vertex for simplex algorithm from backup file.
 *
 * \note To enable the GNU Scientific Library interface, compile with \c
 * USE_GSL defined.
 */

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "csearch.h"

/*!
 * Read vertex for simplex algorithm from backup file.
 *
 * \param[in] ver_file input filename containing start geometry for SEARCH.
 *
 * \param[out] y parameter vector of function values at the vertex positions.
 * \param[out] p matrix containing coordinates of the vertex.
 *
 * \param n_dim dimension of \p y and \p p. If n_dim < 0, do not check
 * dimensions.
 *
 * \warning Memory for \p y and \p p must be allocated before entering
 * the function.
 *
 * \return code indicating success.
 * \retval SR_SUCCESS if successful.
 * \retval \ref search_error value if failed and \c EXIT_ON_ERROR is not defined.
 *
 * \note exits with \ref search_error code if failed and \c EXIT_ON_ERROR
 * defined.
 */
int sr_rdver(const char *ver_file, cleed_vector *y, cleed_basic_matrix *p, int n_dim)
{
  FILE *ver_stream;

  char fmt_buffer[STRSZ];
  char linebuffer[STRSZ];                   /* input buffer */
  size_t i_str, n_str;

  size_t i_par, j_par;                        /* counter, dummy  variables */
  size_t m_par;

  real faux = 0.;

  /* START INPUT: Open and Read vertex file */
  if( (ver_stream = fopen(ver_file, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", ver_file);
    ERROR_EXIT_RETURN(SR_FILE_IO_ERROR, SR_FILE_IO_ERROR);
  }

  CONTROL_MSG(CONTROL_X, "Reading file \"%s\"\n", ver_file);

  sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);

  while ( *fgets(linebuffer, STRSZ, ver_stream) == '#');
  sscanf(linebuffer, "%u %u", &i_par, &m_par);

  if( n_dim < 0)
  {
    WARNING_MSG("n_dim < 0: dimensions are not checked\n");
    n_dim = (int)i_par;
  }
  else if( (i_par != (size_t)n_dim) || (m_par != (size_t)n_dim + 1) )
  {
    ERROR_MSG("dimensions do not match: %u/%u, %u/%u\n",
              i_par, n_dim, m_par, n_dim + 1);
    ERROR_EXIT_RETURN(SR_INVALID_VERTEX_FILE, SR_INVALID_VERTEX_FILE);
  }

  n_str = m_par * 15;

  i_par = 0;
  while( (fgets(linebuffer, (int)n_str, ver_stream) != NULL) && (i_par <= m_par) )
  {
    CONTROL_MSG(CONTROL_X, "(%s): %s", ver_file, linebuffer);

    if( *linebuffer != '#')
    {
      i_str = 0;
      while(linebuffer[i_str] == ' ') i_str ++;

      sscanf(linebuffer+i_str, fmt_buffer, &faux);
      cleed_vector_set(y, i_par, faux);

      for(j_par = 0; j_par < (size_t)n_dim; j_par ++)
      {
        while(linebuffer[i_str] != ' ') i_str ++;
        while(linebuffer[i_str] == ' ') i_str ++;
        sscanf(linebuffer+i_str, fmt_buffer, &faux);
        cleed_basic_matrix_set(p, i_par, j_par, (size_t)n_dim, faux);
      }

      i_par ++;

    } /* if != '#' */

  }   /* while: read input file */

# if CONTROL_X
  fprintf(STDCTR, "\n");
# endif

  /* END OF INPUT: Close input file.*/
  fclose(ver_stream);

# if CONTROL
  fprintf(STDCTR, "(sr_rdver): vertex read from \"%s\":\n", ver_file);
  for (i_par = 0; i_par < m_par; i_par++)
  {
    fprintf(STDCTR, "(%2d) %7.4f :", i_par, cleed_vector_get(y, i_par));
    for(j_par=0; j_par < (size_t)n_dim; j_par++)
    {
      fprintf(STDCTR, " %6.3f", cleed_basic_matrix_get(p, i_par, j_par, (size_t)n_dim));
    }
    fprintf(STDCTR, "\n");
  }
  fflush(STDCTR);
# endif

  return(SR_SUCCESS);
}  /* end of function sr_rdver */
