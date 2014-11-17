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

#define RFORM     "%6.3f "
#define R_DIAFORM "  *.** "

#define CFORM_2    "(%7.4f,%7.4f) "
#define CFORM_ZERO "  -.----, -.----  "
#define MIN_VALUE  0.00005

#define CMAXCOL  16
#define C_DIAFORM "( *.**, *.**) "

/*!
 * Prints the elements of matrix \p M to #STDOUT
 *
 * \param[in] M Matrix to print.
 *
 * \note If the matrix does not exist then the function returns (after
 * printing an error message if #ERROR is defined).
 */
void matshow(const mat M)
{
  size_t i_r, i_c;

  if (matcheck(M) < 1)
  {
    #ifdef ERROR
    fprintf(STDERR, "*** error (matshow): matrix does not exist \n");
    #endif

    return;
  }
  else
  {
    fprintf(STDOUT, "(%d rows) x (%d columns):\n", M->rows, M->cols);
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
                fprintf(STDOUT, RFORM, RMATEL(i_r,i_c,M));
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

            break;
          } /* case REAL */

          case(NUM_COMPLEX):
          {
            for (i_r = 1; i_r <= M->rows; i_r ++ )
            {
              fprintf(STDOUT, "%3d:    ", i_r);
              for (i_c = 1; i_c <= M->cols; i_c ++)
              {
                if( (R_fabs(RMATEL(i_r, i_c, M)) < MIN_VALUE) &&
                    (R_fabs(IMATEL(i_r, i_c, M)) < MIN_VALUE) )
                {
                  fprintf(STDOUT, CFORM_ZERO);
                }
                else
                {
                  fprintf(STDOUT, CFORM_2, RMATEL(i_r, i_c, M),
                      IMATEL(i_r, i_c, M));
                }
                if( (i_c % CMAXCOL == 0) && (i_c < M->cols) )
                {
                  fprintf(STDOUT, "\n  (%3d) ", i_c+1);
                }
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

            break;
          } /* case COMPLEX */

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
                if(i_r == i_c) fprintf(STDOUT, RFORM, M->rel[i_r]);
                else           fprintf(STDOUT, R_DIAFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

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
                  fprintf(STDOUT, CFORM_2, M->rel[i_r], M->iel[i_r]);
                }
                else           fprintf(STDOUT, C_DIAFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

            break;
          } /* case COMPLEX */

        }  /* switch num_type */

        break;
      }  /* MAT_DIAG */

    }  /* switch mat_type */

  }   /* else */

}  /* end of function matshow */
