/*********************************************************************
 *                        MATSHOWABS.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/16.08.94 - Creation
 *********************************************************************/

/*! \file
 *
 * Displays the modulus of the elements of a matrix.
 */

#include <math.h>  
#include <stdio.h>

#include "mat.h"

static const real MINVAL = 1.e-6;
static const char* ZFORM = " ---  ";
static const size_t RMAXCOL = 50;
static const char* RFORM = "%5.3f ";
static const char* R_DIAFORM = "  *.** ";

/*!
 * Prints the modulus of the elements of matrix \p M to \p stream
 *
 * \param[in] stream Output stream to print matrix to.
 * \param[in] M Matrix to print modulus of elements.
 *
 * \note If \p stream is \c NULL then no output will be written.
 * \warning \p M must be a valid matrix.
 * \warning The function will exit with an \c ERROR_MSG upon
 * encountering any errors.
 */
const char *matshowabs(FILE *stream, const mat M)
{
  size_t i_r, i_c, len = 0;
  size_t maxcol;

  real faux;

  static char *str = NULL;
  char mat_header[STRSZ] = "";
  char fmt[STRSZ] = "";

  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    return "";
  }

  /* allocate and initialise string */
  if (M->mat_type == NUM_REAL) sprintf(fmt, RFORM, 0.);
  len = sprintf(mat_header, "(%d rows) x (%d columns):\n", M->rows, M->cols);
  size_t max_len = len + (M->rows * M->cols * (strlen(fmt)+20));

  CLEED_REALLOC(mat_header, max_len * sizeof(char));

  len = sprintf(str, mat_header);
  maxcol = MIN(M->cols, RMAXCOL);
  len += sprintf(str+len,
                 "(matshowabs): (%d rows) x (%d columns); --- < %.0e:\n",
                 M->rows, M->cols, MINVAL);
  switch(M->mat_type)
  {
    case(MAT_NORMAL): case(MAT_SQUARE):
    {
      switch(M->num_type)
      {
        case(NUM_REAL):
        {
          for (i_r = 1; i_r <= M->rows; ++i_r )
          {
            for (i_c = 1; i_c <= maxcol; ++i_c)
            {
              if( (faux = cleed_real_fabs(RMATEL(i_r, i_c, M))) > MINVAL)
              {
                len += sprintf(stream, RFORM, faux);
              }
              else len += sprintf(stream, ZFORM);
            }
            len += sprintf(stream, "\n");
          }
          len += sprintf(stream, "\n");

          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (i_r = 1; i_r <= M->rows; ++i_r )
          {
            for (i_c = 1; i_c <= maxcol; ++i_c)
            {
              if( (faux = cri_abs(*rmatel(i_r, i_c, M),
                                  *imatel(i_r, i_c, M))) > MINVAL)
              {
                len += sprintf(stream, RFORM, faux);
              }
              else len += sprintf(stream, ZFORM);
            }
            len += sprintf(stream, "\n");
          }
          len += sprintf(stream, "\n");

          break;
        } /* case CLEED_COMPLEX */

        case(NUM_IMAG): case(NUM_MASK): default:
          ERROR_MSG("Unsupported data type for matrix M (%s)\n",
                    strmtype(M->num_type));
          return "";
          break;

      }  /* switch num_type */
      break;
    }  /* MAT_NORMAL, MAT_SQUARE */

    case(MAT_DIAG):
    {
      switch(M->num_type)
      {
        case(NUM_REAL):
        {
          for (i_r = 1; i_r <= M->cols; ++i_r )
          {
            for (i_c = 1; i_c <= maxcol; ++i_c)
            {
              if(i_r == i_c)
                len += sprintf(stream, RFORM, cleed_real_fabs(M->rel[i_r]));
              else
                len += sprintf(stream, R_DIAFORM);
            }
            len += sprintf(stream, "\n");
          }
          len += sprintf(stream, "\n");

          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (i_r = 1; i_r <= M->cols; ++i_r )
          {
            for (i_c = 1; i_c <= maxcol; ++i_c)
            {
              if(i_r == i_c)
              {
                len += sprintf(stream, RFORM, cri_abs(M->rel[i_r], M->iel[i_r]));
              }
              else len += sprintf(stream, R_DIAFORM);
            }
            len += sprintf(stream, "\n");
          }
          len += sprintf(stream, "\n");

          break;
        } /* case CLEED_COMPLEX */

        case(NUM_IMAG): case(NUM_MASK): default:
          ERROR_MSG("Unsupported data type for matrix M (%s)\n",
                    strmtype(M->num_type));
          return "";
          break;

      }  /* switch num_type */

      break;
    }  /* MAT_DIAG */

    case(MAT_SCALAR): default:
      ERROR_MSG("Unsupported matrix type\n");
      return "";
      break;
  }  /* switch mat_type */

  /* write output if stream is valid */
  fprintf(stream, str);

  return str;

}  /* end of function matshow */
