/*********************************************************************
 *                        MATSHOW.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/11.08.94 - Diagonal matrices
 *   GH/23.01.95 - Does not display 1x1 matrices! (Don't know why)
 *   GH/19.09.00 - Change CFORM to CFORM_2 (no more abs. value)
 *********************************************************************/

/*! \file
 *
 * Prints the elements of a matrix.
 */

#include <math.h>
#include <stdio.h>

#include "mat.h"

static const char* RFORM = "%6.3f ";
static const char* R_DIAFORM = "  *.** ";

static const char* CFORM_2 = "(%7.4f,%7.4f) ";
static const char* CFORM_ZERO = "  -.----, -.----  ";
static const real MIN_VALUE = 0.00005;

enum { CMAXCOL = 16};
static const char* C_DIAFORM = "( *.**, *.**) ";

/*!
 * Prints the elements of matrix \p M to \p stream
 *
 * \param stream Where the output matrix will be written to.
 * \param[in] M Matrix to print.
 *
 * \return A string of the matrix output written to \p stream .
 *
 * \warning If the matrix does not exist then the function returns
 * a zero length string(after printing an error message
 * if #ERROR_LOG is defined).
 *
 * \note No output will be written if \p stream is \c NULL
 * (but a string of the output will still be returned).
 */
const char *matshow(FILE *stream, const mat M)
{
  size_t i_r, i_c, len = 0;

  static char *str = NULL;
  char mat_header[STRSZ] = "";
  char fmt[STRSZ] = "";

  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    return "\0";
  }

  /* allocate and initialise string */
  if (M->mat_type == NUM_REAL) sprintf(fmt, RFORM, 0.);
  else /* assume complex */  sprintf(fmt, CFORM_2, 0., 1.);
  sprintf(mat_header, "(%d rows) x (%d columns):\n", M->rows, M->cols);
  size_t max_len = strlen(mat_header) + (M->rows * M->cols * (strlen(fmt)+20));

  CLEED_REALLOC(mat_header, max_len* sizeof(char));

  len = sprintf(str, mat_header);
  switch(M->mat_type)
  {
    case(MAT_NORMAL): case(MAT_SQUARE):
    {
      switch(M->num_type)
      {
        case(NUM_REAL):
        {
          for (i_r = 1; i_r <= M->rows; i_r ++ )
          {
            for (i_c = 1; i_c <= M->cols; i_c ++)
            {
              len += sprintf(str+len, RFORM, *rmatel(i_r,i_c,M));
            }
            len += sprintf(str+len, "\n");
          }
          len += sprintf(str+len, "\n");

          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (i_r = 1; i_r <= M->rows; i_r ++ )
          {
            fprintf(STDOUT, "%3d:    ", i_r);
            for (i_c = 1; i_c <= M->cols; i_c ++)
            {
              if( (cleed_real_fabs(*rmatel(i_r, i_c, M)) < MIN_VALUE) &&
                  (cleed_real_fabs(*imatel(i_r, i_c, M)) < MIN_VALUE) )
              {
                len += sprintf(str+len, CFORM_ZERO);
              }
              else
              {
                len += sprintf(str+len, CFORM_2, *rmatel(i_r, i_c, M),
                    *imatel(i_r, i_c, M));
              }
              if( (i_c % CMAXCOL == 0) && (i_c < M->cols) )
              {
                len += sprintf(str+len, "\n  (%3d) ", i_c+1);
              }
            }
            len += sprintf(str+len, "\n");
          }
          len += sprintf(str+len, "\n");

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
          for (i_r = 1; i_r <= M->cols; i_r ++ )
          {
            for (i_c = 1; i_c <= M->cols; i_c ++ )
            {
              if(i_r == i_c) len += sprintf(str+len, RFORM, M->rel[i_r]);
              else           len += sprintf(str+len, R_DIAFORM);
            }
            len += sprintf(str+len, "\n");
          }
          len += sprintf(str+len, "\n");

          break;
        } /* case REAL */

        case(NUM_COMPLEX):
        {
          for (i_r = 1; i_r <= M->cols; i_r ++ )
          {
            for (i_c = 1; i_c <= M->cols; i_c ++ )
            {
              if(i_r == i_c)
              {
                len += sprintf(str+len, CFORM_2, M->rel[i_r], M->iel[i_r]);
              }
              else
                len += sprintf(str+len, C_DIAFORM);
            }
            len += sprintf(str+len, "\n");
          }
          len += sprintf(str+len, "\n");

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

    case (MAT_SCALAR): default:
      ERROR_MSG("Unsupported type for matrix M\n");
      return "";

  }  /* switch mat_type */

  /* print to stream if valid */
  if (stream)
    fprintf(stream, str);

  return str;

}  /* end of function matshow */
