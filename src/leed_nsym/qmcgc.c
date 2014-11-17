/**********************************************************************
 *                          QMCGC.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/03.09.97 - return type of show_cg_coef() is void.
 *   GH/22.09.00 - add gaunt and blm
 *********************************************************************/

/*! \file
 *
 * Quantum mechanical operations involving Clebsh Gordon coefficients (C.G.C.).
 *
 * The file contains the functions: mk_cg_coef() , show_cg_coef() , cg() ,
 * cg_info() , blm() and gaunt()
 *
 */
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "mat.h"
#include "qm.h"

#ifdef WARNING
#define WARN_LEVEL 1.0
/* if a C.G-C exceeds this level, a warning message will be printed */
#endif

#define UNUSED     -1

static double *cg_coef  = NULL;      /*!< Clebsh Gordan coefficients */

static int l_max_coef = UNUSED;      /*!< l_max for the C.G.C */
static int st_fac1    = UNUSED;
static int st_fac2    = UNUSED;

/*!
 * Calculates all Clebsh Gordan coefficients
 * C( l1, m1, l2, m2, l3, m3) for 0 <= l1 <= 2*l_max, 0 <= l2,l3 <= l_max.
 *
 * Storage Scheme
 * ==============
 *
 * The storage scheme is:
 *
 * C(l1,m1,l2,m2,l3,m3) -> cg_coef + (l1 * (l1 + 1)/2 + m1pm) * st_fac1 +
 *                                   (l2 * (l2 + 1)   + m2pm) * st_fac2 +
 *                                   l3/2;
 *
 * Since C(l1,m1,l2,m2,l3,m3) = C(l1,-m1,l2,-m2,l3,-m3) only the coefficients
 * for positive values of m1 are stored (m2 can be positive of negative).
 * The coefficients of -m1 and m2 are the same as for m1 and -m2.
 *
 * Only half of the possible values of l3 lead to nonzero C.G.-C's because of
 * the condition:
 *  @f[ l_1 + l_2 + l_3 = \mbox{even} @f]
 *
 * Thus only these are stored (offset 0: l3 = 0/1, 1: l3 = 2/3 etc.)
 *
 * Only one valid value of m3 exists per (m1,m2)-pair because of the condition:
 *  @f[ m_1 = m_2 + m_3 @f]
 *
 * The ranges of @f$ l_1, l_2, l_3 @f$ are:
 *
 * @f[  0 <= l_1  <= 2*l_{max},  0 <= m_1 <= l_1          @f]
 * @f[  0 <= l_2,l_3 <= l_{max},  -l_2 <= m_2 <= l_2      @f]
 *
 * Memory requirements for the static array cg_coef:
 *
 * @code
 * (2*l_max+1)*(2*l_max+2)/2 * (l_max+1)^2 * (l_max/2+1) * sizeof(double)
 * @endcode
 *
 * l_max =  6 :    142,688 bytes
 * l_max =  8 :    495,720 bytes
 * l_max = 10 :  1,341,648 bytes
 * l_max = 12 :  3,075,800 bytes
 * l_max = 14 :  6,264,000 bytes
 * l_max = 16 : 11,673,288 bytes
 *
 * \param l_max maximum angular momentum @f$ l_{max} @f$ for output.
 * \return Integer representing state of C.G. calculations.
 * \retval -1  if an error occurred (and if #EXIT_ON_ERROR is not defined).
 * \retval  0  if C.G. had been calculated before (l_max_coef >= l_max)
 * \retval  1  if C.G. were calculated in this call of
 * mk_cg_coef(l_max_coef == l_max)
 * \retval >1  number of warnings raised.
 */
int mk_cg_coef(size_t l_max)
{
  int l1, l2, l3;          /* angular momentum quantum numbers */
  int m1, m2, m3;
  int m1pm, m2pm, m3pm;
  int lll;
  int m2_min, m2_max;

  int l1_s, l2_s, l3_s;    /* permutated indices */
  int i_st1, i_st2;        /* storage address */

  int i_op, i_warn;
  int i, i_min, i_max;
  int iaux;

  double *fac;             /* list of factorials */
  double fac_m, sum_m;     /* factors in the calculation of the C.G.C.'s */
  double fac_l, fac_ls;
  double sign;

  /*****************************************
   * Check if the C.G.C had been
   * calculated already:
   *****************************************/

  /* If yes, return 0. */
  if( (l_max <= l_max_coef) && (cg_coef != NULL) ) return(0);

  /* If not, allocate memory for cg_coef */
  iaux = (2*l_max + 1)*(2*l_max + 2)/2 * (l_max + 1)*(l_max + 1) * (l_max/2 + 1);

  #ifdef CONTROL
  fprintf(STDCTR, "(mk_cg_coef): cg_coef[%d] (%.3f Mb) for l_max = %2d\n",
                      iaux, (real)iaux*sizeof(double) / MBYTE, l_max );
  #endif

  if (cg_coef != NULL) free(cg_coef);
  cg_coef = (double *) calloc (iaux, sizeof(double));
  if (cg_coef == NULL)
  {
    #ifdef ERROR
    fprintf(STDERR, "(mk_cg_coef): allocation error: cg_coef[%d] = %d bytes\n",
            iaux, iaux*sizeof(double) );
    #endif

    #ifdef EXIT_ON_ERROR
    exit(1);
    #else
    return(-1);
    #endif
  }

  /* Static variables used to retrieve the C.G.C's */
  st_fac1 = (l_max + 1)*(l_max + 1) * (l_max/2 + 1);
  st_fac2 = (l_max/2 + 1);
  l_max_coef = l_max;

  /* Produce a list of factorials */
  iaux = 4*l_max + 1;
  fac = (double *) malloc( (iaux + 1) * sizeof(double) );
 
  for (fac[0] = 1. , i = 1; i <= iaux; i ++ )
  {
    fac[i] = fac[i-1] * i;
  }

  /* Start calculation of C.G. coefficients:
   * Loops over l2,l3,l1,m1,m2
   */
  i_op = 0; i_warn = 0;
  for (l2 = 0; l2 <= l_max; l2 ++)
  {
    for (l3 = 0; l3 <= l2; l3 ++)
    {
      for (l1 = (l2-l3); l1 <= l2+l3; l1 += 2)
        /*
         * The sum l1+l2+l3 must be even, i.e. for given l1 and l3, l2 can be
         * incremented by 2. For the start value l2 = |l3-l1| this
         * condition is always fulfilled.
         */
      {
        /* Calculate fac_l */
        lll = (l1+l2+l3)/2;
        fac_l = fac[2*(lll - l3)] * fac[lll] /
            (fac[2*lll + 1] * fac[lll -l1] * fac[lll -l2] * fac[lll -l3]);

        for (m1pm = 0; m1pm <= l1; m1pm ++)
        {
          m2_min = MAX((m1pm-l3),-l2);
          m2_max = MIN((m1pm+l3), l2);
 
          for ( m2pm = m2_min ; m2pm <= m2_max; m2pm ++)
            /* The condition |m1| >= |m2| >= |m1-m2| is not automatically
             * fulfilled
             */
          {
            m3pm = m1pm-m2pm;
            m1 = abs(m1pm);
            m2 = abs(m2pm);
            m3 = abs(m3pm);

            #ifdef CONTROL_MK1
            fprintf(STDCTR,
              "(mk_cg_coef):     > [%1d,%1d(%1d)%1d,%1d(%1d)%1d,%1d(%1d)]\n",
                                   l1,m1,m1pm,l2,m2,m2pm,l3,m3,m3pm);
            #endif

            /* Find the address in storage space cp_coef:
             * C(l2,m2,l3,m3) and C(l3,m3,l2,m2) are identical:
             */
            i_st1 = (l1 * (l1 + 1)/2 + m1pm) * st_fac1 +
                    (l2 * (l2 + 1)   + m2pm) * st_fac2 + l3/2;
            i_st2 = (l1 * (l1 + 1)/2 + m1pm) * st_fac1 +
                    (l3 * (l3 + 1)   + m3pm) * st_fac2 + l2/2;

            /* Check the correct order of m1,m2 and m3.
             * Change the order if necessary.
             * l1_s, l2_s, l3_s are used for a different purpose now !
             */
            fac_ls = fac_l;
            l1_s = l1; l2_s = l2; l3_s = l3;

            if ((m2 > m1) || (m3 > m1))
            {
              if (m3 > m2)
              {
                l1_s = l3; m1 = m3;
                l3_s = l1; m3 = abs(m1pm);
                l2_s = l2;
              }
              else
              {
                l1_s = l2; m1 = m2;
                l2_s = l1; m2 = abs(m1pm);
                l3_s = l3;
              }
              /* Check if l2_s is still bigger than l3_s.
               * If not, exchange l2_s and l3_s.
               * fac_l needs to be recalculated.
               */
              if(l2_s < l3_s)
              {
                iaux = l2_s; l2_s = l3_s; l3_s = iaux;
                iaux = m2;   m2   = m3;   m3   = iaux;
              }
  
              fac_ls = fac[2*(lll - l3_s)] * fac[lll] /
                  (fac[2*lll + 1]*fac[lll -l1_s]*fac[lll -l2_s]*fac[lll -l3_s]);
            }

            #ifdef CONTROL_MK1
            fprintf(STDCTR,
                   "[%1d,%1d,%1d,%1d,%1d,%1d] > [%1d,%1d,%1d,%1d,%1d,%1d]",
                    l1,m1pm,l2,m2pm,l3,m3pm, l1_s,m1,l2_s,m2,l3_s,m3);
            fprintf(STDCTR," fac_ls = %e\n", fac_ls);
            #endif

            /* Calculate fac_m:
             * According to Pendry's book
             * a factor (4*PI)^-1/2 must be multiplied with the plain
             * coefficients.
             *
             * ((4*PI)^-1/2 = .28209478969020929917)
             */
            fac_m  = fac[l3_s - m3] * fac[l1_s - m1] / fac[l2_s - m2];
            fac_m *= fac[l2_s + m2] * fac[l3_s + m3] / fac[l1_s + m1];
            fac_m *= (2*l1_s + 1.) * (2*l2_s + 1.) * (2*l3_s + 1.);
            fac_m =  sqrt(fac_m)*.28209478969020929917;

            /*
               Calculate sum_m:
               i_min, i_max are the boundaries for summation over i.
            */
            i_min = MAX(0, (-l2_s + l3_s - m1));
            i_max = MIN(l1_s - m1, l3_s - m3);
            i_max = MIN( (l2_s + l3_s - m1), i_max);
            sign = M1P(i_min);

            for(i = i_min, sum_m = 0.; i<= i_max; i++, sign = -sign )
            {
              sum_m += sign * fac[l1_s + m1 + i] * fac[l2_s + l3_s - m1 - i] /
                            (fac[l1_s - m1 - i] * fac[l2_s - l3_s + m1 + i] * 
                             fac[l3_s - m3 - i] * fac[i]);
            }

            /*
             * Calculate the C.G. coefficient from the factors
             * fac_ls, fac_m and sum_m.
             *
             * Sign compatible with BLM:
             * sign = M1P(lll - l2_s - m3 + (m1+m2+m3)/2);
             *
             * Sign compatible with CA:
             * sign = M1P(- l2_s - m3 - m3pm + (m1+m2+m3)/2);
             */
            sign = M1P(lll - l2_s - m3 + (m1+m2+m3)/2);
            cg_coef[i_st1] = sign * fac_ls * fac_m * sum_m;
            cg_coef[i_st2] = sign * fac_ls * fac_m * sum_m;

            #ifdef WARNING
            if (cg_coef[i_st1] > WARN_LEVEL)
            {
              fprintf(STDWAR, "* warning (mk_cg_coef): "
                  "CG-C[%2d %2d %2d %2d %2d %2d] = %9.6f\n",
                  l1, m1pm, l2, m2pm, l3, m3pm, cg_coef[i_st1]);
              i_warn ++;
            }
            #endif

            #ifdef CONTROL_MK
            fprintf(STDCTR, " CG-C[%2d%2d%2d%2d%2d%2d] = %9.6f",
                      l1, m1pm, l2, m2pm, l3, m3pm, cg_coef[i_st1]);
            fprintf(STDCTR, "  A*C = %9.6f; B = %9.1f", fac_l * fac_m, sum_m);
            fprintf(STDCTR, "  i_st1: %d\n", i_st1);
            #endif
            i_op++;

          } /* m2 */
        } /* m3 */
      } /* l3 */
    } /* l2 */
  } /* l1 */

  #ifdef CONTROL
  fprintf(STDCTR, "(mk_cg_coef): number of operations = %d\n", i_op);
  #endif

  #ifdef WARNING
  if (i_warn)
  {
    fprintf(STDWAR, "* (mk_cg_coef): number of warnings   = %d\n", i_warn);
  }
  #endif

  free(fac);

  return(1+i_warn);
}  /* end of function mk_cg_coef */


/*!
 * Show all Clebsh Gordan coefficients in the list #cg_coef .
 *
 * C( l1, m1, l2, m2, l3, m3) for l1 <= 2*l_max_coef, l2,l3 <= l_max_coef.
 */
void show_cg_coef()
{
  int l1, l2, l3;               /* angular momentum quantum numbers */
  int m1, m2, m3;
  int st_off1, st_off2;
  int l23_max;

  int i_st;


  for(l1 = 0; l1 <= 2* l_max_coef; l1 ++)
  {
    for(m1 = 0; m1 <= l1; m1 ++)
    {
      st_off1 = (l1*(l1+1)/2 + m1) * st_fac1;

      l23_max = MIN(l1,l_max_coef);
      for(l2 = 0; l2 <= l23_max; l2 ++)
      {
        for(m2 = -l2; m2 <= l2; m2 ++)
        {
          st_off2 = st_off1 + (l2*(l2+1) + m2) * st_fac2;
         
          for(l3 = ODD(l1+l2)?1:0; l3 <= l23_max; l3 += 2)
          {
            m3 = m1-m2;
            if (abs(m3) <= l3)
            {
              i_st = st_off2 + l3/2;
              fprintf(STDOUT, "cg_coef[%2d%2d%2d%2d%2d%2d] = %9.6f (%d)\n",
                      l1, m1, l2, m2, l3, m3, cg_coef[i_st], i_st );
            }
            else
            {
              i_st = st_off2 + l3/2;
              if (!IS_EQUAL_REAL(cg_coef[i_st], 0.))
              {
                fprintf(STDOUT, "cg_coef[%2d%2d%2d%2d%2d%2d] = ** %9.6f (%d)\n",
                        l1, m1, l2, m2, l3, m3, cg_coef[i_st], i_st );
              }
            }
          }   /* l3 */
        }   /* m2 */
      }  /* l2*/
    }   /* m1 */
  }   /* l1 */
}  /* end of function show_cg_coef */


/*!
 * Returns the Clebsh Gordan coefficient C(l1, m1, l2, m2, l3, m3)
 * for l1 <= 2* l_max_coef, l2/l3 <= l_max_coef
 *
 * It is the reverse operation to the storage scheme used in mk_cg_coef()
 *
 * The function does not check if the quantum numbers are within the limits
 * given by \c l_max_coef . This has to be checked outside the function using
 * cg_info() .
 *
 * \param l1
 * \param m1
 * \param l2
 * \param m2
 * \param l3
 * \param m3
 * \return
 * \see mk_cg_coef()
 */
double cg (int l1, int m1, int l2, int m2, int l3, int m3)
{
  int i_st;

  /*
   * First: trivial results:
   * l1 + l2 + l3 not even
   * m1 != m2 + m3
   */

  if ( ODD(l1 + l2 + l3) ) return (0.);
  if ( m1 != m2 + m3)      return (0.);

  /* Find the address in storage space cp_coef:
   * Only C.G.C's for m1 >= 0 are stored. If m1 < 0, look for
   * C.G.C.[-m1,-m2] = C.G.C.[m1,m2]
   */
  if (m1 < 0) { m1 = -m1; m2 = -m2; }

  #ifdef CONTROL_CG
  fprintf(STDCTR, "(cg:) l1:%2d, m1:%2d, l2:%2d, m2:%2d, l3:%2d, m3:%2d\n",
           l1,m1,l2,m2,l3,m3 );
  #endif
 
  i_st = (l1 * (l1 + 1)/2 + m1) * st_fac1 +
         (l2 * (l2 + 1)   + m2) * st_fac2 + l3/2;

  return(cg_coef[i_st]);
}  /* end of function cg */

double *cg_info(int l1, int m1, int l2, int m2,
                int* l_max, int* inc1, int* inc2)

/*!
 * Returns relevant storage information about the array #cg_coef
 *
 * \param l1
 * \param m1
 * \param l2
 * \param m2
 * \param[out] l_max Maximum angular momentum for which C.G. coefficients are
 * stored.
 * \param[out] inc1 increment when increasing m1 by 1.
 * \param[out] inc2 increment when increasing m2 by 1.
 * \return Address of Clebsch-Gordon coefficient C(l1, m1, l2, m2, 0/1, m2)
 * in memory.
 */
{
  *l_max = l_max_coef;
  *inc1  = st_fac1;
  *inc2  = st_fac2;
  
  return (cg_coef + (l1 * (l1 + 1)/2 + m1) * st_fac1 + 
                    (l2 * (l2 + 1)   + m2) * st_fac2   );
}  /* end of function cg_info */

/*!
 * Returns the same value as Pendry's function BLM:
 *
 * BLM(l1,m1,l2,m2,l3,m3) = S Y(l1,m1) * Y(l2,m2) * Y(l3,m3)
 *                        = (-1)^m3 S Y(l1,m1) * Y(l2,m2) * Y(l3,-m3)
 *                        = cg(l2,-m2,l1,m1,l3,m3)
 *
 * for l2 <= 2* l_max_coef, l1/l3 <= l_max_coef
 */
double blm (int l1, int m1, int l2, int m2, int l3, int m3)
{
  return(cg(l2,-m2,l1,m1,l3,m3));
}  /* end of function blm */

/*!
 * Returns Gaunt's integral:
 * (-1)^m3 S Y(l1,m1) * Y(l2,m2) * Y*(l3,m3) = cg(l2,m2,l1,-m1,l3,m3)
 *
 * for l2 <= 2* l_max_coef, l1/l3 <= l_max_coef
 *
 * \param l1
 * \param m1
 * \param l2
 * \param m2
 * \param l3
 * \param m3
 * \return
 */
double gaunt (int l1, int m1, int l2, int m2, int l3, int m3)
{
  return(cg(l2,m2,l1,-m1,l3,m3));
}  /* end of function gaunt */
