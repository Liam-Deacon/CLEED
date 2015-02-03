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
 * Prints the modulus of the elements of matrix \p M to #STDOUT
 *
 * \param[in] M Matrix to print modulus of elements.
 */
void matshowabs(const mat M)
{
  size_t i_r, i_c;
  size_t maxcol;

  real faux;
  
  if (matcheck(M) < 1)
  {
    ERROR_MSG("matrix does not exist \n");
    return;
  }
  else
  {
    maxcol = MIN(M->cols, RMAXCOL);
    fprintf(STDOUT, "(matshowabs): (%d rows) x (%d columns); --- < %.0e:\n",
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
                if( (faux = R_fabs(RMATEL(i_r, i_c, M))) > MINVAL)
                {
                  fprintf(STDOUT, RFORM, faux);
                }
                else fprintf(STDOUT, ZFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

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
                  fprintf(STDOUT, RFORM, faux);
                }
                else fprintf(STDOUT, ZFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

            break;
          } /* case CLEED_COMPLEX */
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
                if(i_r == i_c) fprintf(STDOUT, RFORM, R_fabs(M->rel[i_r]));
                else           fprintf(STDOUT, R_DIAFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

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
                  fprintf(STDOUT, RFORM, cri_abs(M->rel[i_r], M->iel[i_r]));
                }
                else fprintf(STDOUT, R_DIAFORM);
              }
              fprintf(STDOUT, "\n");
            }
            fprintf(STDOUT, "\n");

            break;
          } /* case CLEED_COMPLEX */
        }  /* switch num_type */

        break;
      }  /* MAT_DIAG */
    }  /* switch mat_type */
  }   /* else */

}  /* end of function matshow */
