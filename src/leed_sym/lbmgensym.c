/*********************************************************************
  WB/06.10.98 
  file contains function:

  leed_beam_gen_sym 
  
  Generate only beam sets which are not equivalent by symmetry.

Changes:

GH/30.08.97 - Copy from leed_beam_gen
GH/03.09.97 - include symmetrisation of beams:
              reduce number of beam sets according to ROTATIONAL SYMM.
              calculate phase prefactors for matrix elements.
WB/15.06.98 - remove mistakes in the case that beam set is equiv. to itself.
WB/17.08.98 - k_x_sym ... for composite layers
WB/20.08.98 - take k_x ... to create ein_b ...
WB/01.09.98 - try to input mirror planes
WB/03.09.98 - luft fuer 4,3,2,1 Achse aber nicht perfekt
WB/04.09.98 - change sign of angles and add 2PI for negative angles
              luft fuer 4 perfekt 
              luft fr 1 perfekt falls man die richtige Ebene whlt
              (mu aber unabhngig von der Ebene sei da alle gleich)
              statt i_c + i_d beams+i->n_eqb_s ....
              luft fuer 1Ebene falls 45 90(nur 0 nicht so gut)
WB/07.09.98 - bezug auf einheitlichen Winkel(zur 1.Ebene)
              geht perfekt
            - neqb_b (set 0) = 2*n_mir 
*********************************************************************/

#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "leed.h"

int leed_beam_gen_sym(leed_beam ** p_beams, 
               leed_crystal *b_par, 
               leed_crystal *o_par,
               leed_var *v_par, real eng_max)

/************************************************************************

 Set up a list of all non-equivalent beams used within the energy loop.
 
 INPUT:

  leed_beam ** p_beams - (output) pointer to the list of beams to be 
                included at the maximum energy (eng_max).
                (for details see "leed_def.h").
  leed_crystal *b_par - all necessary structural parameters (bulk, 
                for details see "leed_def.h").
  leed_crystal *o_par - all necessary structural parameters (overlayers,
                for details see "leed_def.h").
  leed_var *v_par - all necessary parameters that change during the 
                energy loop (for details see "leed_def.h").
                used: 
                real theta, phi - incident k-vector.
                real epsilon - parameter determining the cutoff radius for 
                k_par. (maximum amplitude which can propagate between two 
                layers).
  real eng_max - maximum energy in the energy loop (in Hartree).

 DESIGN:
 
  The order of the output list is: 
    - increasing modulus of momentum transfer k_par (lowest k_par first)
    - increasing 1st index (lowest 1st index first for the same k_par)
    - increasing 2nd index (lowest 2nd index first for the same 1st index 
      and k_par)

 RETURN VALUE:

  int n_set - number of beam sets in the list pointed to by p_beams.
  -1          if failed

*************************************************************************/
{
  int iaux, m, i_layers, i, j;
  int n1, n2;
  int n1_max, n2_max;
  int offset;
  int i_c, i_d, i_e;
  int i_beams, i_set, j_set, n_set;        /* beam sets */
  int l_max, n_mir;                        /* symmetry */
  int n_rot, i_rot;
  int ctrol;

  real k_x_mir[13];                        /* for sorting the */
  real k_y_mir[13];                        /* sym equivalent beam */
  real k_p_mir[13];                        /* with respect to bulk- */
  real k_x_store[13];                      /* and overlayer scattering */
  real k_y_store[13];
  real k_p_store[13];
  real vaux[2];
  real integ[2];

  real alpha[3];
  real faux_r, faux_i, pref1, pref2;
  real k_max, k_max_2;
  real wedge;

  real k_in[3];
  real k_r, k_i;

  real g1_x, g1_y, g2_x, g2_y;             /*reziprocal lattice vector*/
  real a1_x, a1_y, a2_x, a2_y;             /*lattice vector*/
  real k_x, k_y;
  real a1, a2;
  real m11, m12, m21, m22;
  real det;                              /*first det(a1,a2) than det(g1,g2)*/
  real equalset1, equalset2;

  real R_m[5];                           /* mirror -matrix*/
  real **R_n;                            /* Rot.-matrix */

  leed_beam *beams, beam_aux;
  leed_beam *bm_off;

  /**********************************************************************
   * Some often used values:
   * - indices of the basic superstructure vectors (mij)
   * - reciprocal (1x1) lattice vectors (g1/2_x/y)
   * - k_in at max. energy (k_in)
   **********************************************************************/

  m11 = b_par->m_recip[1], m12 = b_par->m_recip[2];
  m21 = b_par->m_recip[3], m22 = b_par->m_recip[4];

  g1_x = b_par->a_1[1]; g1_y = b_par->a_1[2];
  g2_x = b_par->a_1[3]; g2_y = b_par->a_1[4];

  faux_r = cleed_real_sin(v_par->theta) * cleed_real_sqrt(2*eng_max);
  k_in[0] = faux_r;
  k_in[1] = faux_r * cleed_real_cos(v_par->phi);
  k_in[2] = faux_r * cleed_real_sin(v_par->phi);

  n_mir = b_par->n_mir;
  n_rot = b_par->n_rot;

  CONTROL_MSG(CONTROL_X, "m11 = %.2f m12 = %.2f\nm21 = %.2f m22 = %.2f\n",
              m11, m12, m21, m22);

  /* Allocate storage space:
   * - Determine k_max (square of max k_par) from epsilon and dmin.
   * - Determine the max. number of beams within this radius (iaux)
   *   and allocate memory for the beam list. (The formula given in
   *   VHT p. 24 is not an upper limit. A prefactor 1/(PI*PI)
   *   (0.10132118) is saver than 1/(4*PI) ).
   */

  eng_max -= v_par->vr;
  faux_r = cleed_real_log(v_par->epsilon) / b_par->dmin;
  k_max_2 = faux_r*faux_r + 2*eng_max;
  k_max = cleed_real_sqrt(k_max_2);
  l_max = v_par->l_max;

  /* n1 is for allocation */
  /* n2 is the n_eqb_b value for set 0 */
  if(n_rot > n_mir)
    n1 = n2 = n_rot;
  else
  {
    n1 = n_mir;
    n2 = 2 * n_mir;
  }
  CONTROL_MSG(CONTROL, "n1%d n2%d\n", n1, n2);
  
  iaux =  2 + (int)( 0.10132118 * b_par->rel_area_sup *
                    b_par->area * k_max_2 /n1);

  if (*p_beams == NULL)
    *p_beams = (leed_beam *)calloc(iaux, sizeof(leed_beam));
  else
    *p_beams = (leed_beam *)realloc(*p_beams, iaux*sizeof(leed_beam));

  if(*p_beams == NULL)
  {
    ERROR_MSG("failed to allocate p_beams.\n");
    exit(1);
  }
  else
  {
    beams = *p_beams;
  }
  CONTROL_MSG(CONTROL_X, "dmin  = %.2f, epsilon = %.2e\n",
              b_par->dmin * BOHR, v_par->epsilon);
  CONTROL_MSG(CONTROL_X, "k_max = %.2f, max. No of beams = %2d\n", k_max, iaux);

  /**********************************************************************
   * Determine the total number of beam sets (n_set)
   * and offsets (store in bm_off)
   * Each beam set is represented exactly once within the first BZ
   * (i.e. within the diamond: (0,0)(1,0)(0,1)(1,1))
   * => raster through the first BZ and store all fractional
   *  order beams in the array bm_off.
   **********************************************************************/
 
  n_set = (int) cleed_real_nint(b_par->rel_area_sup);
  bm_off = (leed_beam *)calloc(n_set, sizeof(leed_beam));

  bm_off[0].ind_1 = 0.;
  bm_off[0].ind_2 = 0.;
  bm_off[0].b_ind_1 = 0;
  bm_off[0].b_ind_2 = 0;
  bm_off[0].k_r[1] = 0.;
  bm_off[0].k_r[2] = 0.;
  bm_off[0].n_eqb_s = 1;
  bm_off[0].n_eqb_b = n2;

  CONTROL_MSG(CONTROL_X, "set %d: index_a (%5.2f %5.2f), "
                         "index_b (%2d %2d), real (%5.2f %5.2f)\n",
             i_set=0, bm_off[0].ind_1,   bm_off[0].ind_2,
                      bm_off[0].b_ind_1, bm_off[0].b_ind_2,
                      bm_off[0].k_r[1],  bm_off[0].k_r[2]);

  for(n1 = -n_set, i_set = 1; n1 <= n_set; n1++)
    for(n2 = -n_set; (n2 <= n_set) && (i_set < n_set); n2++)
    {
      k_x = n1*m11 + n2*m21;
      k_y = n1*m12 + n2*m22;
      if( (k_x >= 0.) && (k_x + K_TOLERANCE < 1.) &&
          (k_y >= 0.) && (k_y + K_TOLERANCE < 1.) &&
          (cleed_real_hypot(k_x, k_y) > K_TOLERANCE)          )
      {
        bm_off[i_set].ind_1 = k_x;
        bm_off[i_set].ind_2 = k_y;
        bm_off[i_set].b_ind_1 = n1;
        bm_off[i_set].b_ind_2 = n2;
        bm_off[i_set].k_r[1] = k_x*g1_x + k_y*g2_x;
        bm_off[i_set].k_r[2] = k_x*g1_y + k_y*g2_y;
        bm_off[i_set].n_eqb_s = 1;
        bm_off[i_set].n_eqb_b = 1;

        CONTROL_MSG(CONTROL_X, "set %d: index_a (%5.2f %5.2f), ",
                            "index_b (%2d %2d), real (%5.2f %5.2f)\n",
             i_set, bm_off[i_set].ind_1,   bm_off[i_set].ind_2,
                    bm_off[i_set].b_ind_1, bm_off[i_set].b_ind_2,
                    bm_off[i_set].k_r[1],  bm_off[i_set].k_r[2]);

        i_set ++;
      } /* if */
    } /* for n1,n2 */
 
  if( i_set != n_set)
  {
    WARNING_MSG("wrong number of beam sets found.\n"
               "                    found: %d, should be: %d\n", i_set, n_set);
  }

  /* Find the beam sets which are equivalent through symmetry. (Rotation or MIRROR) */
  a1_x = b_par->a[1];
  a1_y = b_par->a[3];
  a2_x = b_par->a[2];
  a2_y = b_par->a[4];

  det = a1_x*a2_y - a1_y*a2_x;
  det = 1. / det;

  switch(b_par->symmetry)
  {

    /*! CASE ROTATIONSYMMETRY !!!!!
     * here we have to call the function leed_beam_gen_rot() also in case of
     * NOSYM, because we must allocate and set the variables eout_b/s and
     * ein_b/s . Otherwise the program couldn't work. These variables are
     * needed in the routines bravelsym() and complsym()
     */
    case HEX_3ROT:
    case REC_2ROT:
    case MONO_2ROT:
    case SQ_2ROT:
    case SQ_4ROT:
    case NOSYM:
    {
      R_n = leed_beam_get_rotation_matrices(n_rot);

      /* Scan through beam sets and mark equivalent sets:
       * - beam set 0 (integral order beams) cannot be equivalent to any fractional
       *   order beam set, but is n_rot-fold symmetric.
       * - check whether (ind_1(i),ind_2(i)) R^-1 is an integer vector.
       * - n_eqb_s is used to count the number of equivalent beam sets,
       */
      for(i_set = 1; i_set < n_set; i_set ++)
      {

        if( bm_off[i_set].n_eqb_s != 0 )
        {
          for(i_rot = 1; i_rot < n_rot; i_rot ++)
          {
            /* calculate R^-1 = At^-1 R At */
            m11 =   R_n[i_rot][1]*a1_x*a2_y + R_n[i_rot][2]*a1_y*a2_y
                  - R_n[i_rot][3]*a1_x*a2_x - R_n[i_rot][4]*a1_y*a2_x;
            m11 *= det;
            m21 = - R_n[i_rot][1]*a1_x*a1_y - R_n[i_rot][2]*a1_y*a1_y
                  + R_n[i_rot][3]*a1_x*a1_x + R_n[i_rot][4]*a1_y*a1_x;
            m21 *= det;
            m12 =   R_n[i_rot][1]*a2_x*a2_y + R_n[i_rot][2]*a2_y*a2_y
                  - R_n[i_rot][3]*a2_x*a2_x - R_n[i_rot][4]*a2_y*a2_x;
            m12 *= det;
            m22 = - R_n[i_rot][1]*a2_x*a1_y - R_n[i_rot][2]*a2_y*a1_y
                  + R_n[i_rot][3]*a2_x*a1_x + R_n[i_rot][4]*a2_y*a1_x;
            m22 *= det;
      
            /* calculate (ind_1(i),ind_2(i)) R^-1 */
            k_x = bm_off[i_set].ind_1 * m11 + bm_off[i_set].ind_2 * m21;
            k_y = bm_off[i_set].ind_1 * m12 + bm_off[i_set].ind_2 * m22;

            /* If (ind_1(i),ind_2(i)) R^-1 - ((ind_1(i),ind_2(i)) is integer,
             * the beam set is equivalent to itself. Mark this in n_eqb_b.
             */
            faux_r = k_x - bm_off[i_set].ind_1;
            if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
            {
              faux_r = k_y - bm_off[i_set].ind_2;
              if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
              {
                bm_off[i_set].n_eqb_b ++;
                CONTROL_MSG(CONTROL_X,
                    "set %d has rotation symmetry (rot = %d*2pi/%d)\n",
                    i_set, i_rot, n_rot);
              }
            }

            /* If (ind_1(i),ind_2(i)) R^-1 - ((ind_1(j),ind_2(j)) is integer,
             * the beam sets i and j are equivalent. Mark this in n_eqb_s.
             */
            for(j_set = i_set+1; j_set < n_set; j_set ++)
            {
              if( bm_off[i_set].n_eqb_s != 0 && (bm_off+j_set)->n_eqb_s != 0)
              {
                faux_r = k_x - (bm_off+j_set)->ind_1;
                if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE )    /* integer ? */
                {
                  faux_r = k_y - (bm_off+j_set)->ind_2;
                  if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE )  /* integer ? */
                  {
                    bm_off[i_set].n_eqb_s ++;
                    (bm_off+j_set)->n_eqb_s = 0;
                    CONTROL_MSG(CONTROL_X,
                        "sets %d and %d are equivalent (rot = %d*2pi/%d)\n",
                        i_set, j_set, i_rot, n_rot);
                  }
                }
              } /* if n_eqb_s */
            } /* for j_set */
          } /* for i_rot */
        } /* if != 0 */
        if( (bm_off[i_set].n_eqb_b != 1)&&((n_rot % bm_off[i_set].n_eqb_b) !=0) )
        {
          /* The number of equivalent beams can only be equal to
           * or an integer fraction of n_rot */
          ERROR_MSG("mismatch in symmetries\n");
          exit(1);
        }
      } /* for i_set */

      /* At this point:
       * ind_1, ind_2 = beam indices of one beam in the beam set.
       * n_eqb_s == 0 <=> this beam set can be omitted because it is represented
       *                  by another set
       *         != 0 <=> this beam set represents other beam sets. The number of
       *                  beams is n_eqb_s * n_eqb_b.
       * n_eqb_b = number of beams within the beam set related by symmetry.
       */

#if CONTROL
      CONTROL_MSG(CONTROL, "%d unsymmetrised beam sets:\n", n_set);
      for(i_set = 0; i_set < n_set; i_set ++)
      {
        fprintf(STDCTR, "(leed_beam_gen_rot) set %d:(%5.2f %5.2f) ",
                  i_set, bm_off[i_set].ind_1, bm_off[i_set].ind_2);
        if( bm_off[i_set].n_eqb_s > 0)
        {
          fprintf(STDCTR, "represents %lu sets (%lu beam(s)).\n",
              bm_off[i_set].n_eqb_s, bm_off[i_set].n_eqb_b);
        }
        else
        {
          fprintf(STDCTR, "\t\tis equivalent to %lu (%5.2f %5.2f).\n",
                          bm_off[i_set].n_eqb_b,
                          bm_off[i_set].ind_1,
                          bm_off[i_set].ind_2);
        }
      } /* for i_set */
#endif 

    /* Remove the equivalent beam sets from the list beam_off.
     * - set No 0 (i.o. beams) will always be included in the list,
     *   therefore start loop at 1.
     * - set n_eqb_s to real number of beams (n_eqb_b * n_eqb_s)
     */
    (bm_off+0)->n_eqb_s=(bm_off+0)->n_eqb_s*(bm_off+0)->n_eqb_b;

    for(i_set = 1, j_set = 1; i_set < n_set; i_set ++)
    {
      if( bm_off[i_set].n_eqb_s > 0)
      {
       bm_off[i_set].n_eqb_s =
           bm_off[i_set].n_eqb_s * bm_off[i_set].n_eqb_b;

#if WARNING_LOG
       if( bm_off[i_set].n_eqb_s != n_rot)
       {
         WARNING_MSG("n_eqb_s * n_eqb_b = %lu "
                     "!= n_rot = %d for beam set (%5.2f %5.2f)\n",
                     bm_off[i_set].n_eqb_s, n_rot, bm_off[i_set].ind_1,
                     bm_off[i_set].ind_2);
       }
#endif
       if( i_set > j_set )
         memcpy( bm_off+j_set, bm_off+i_set, sizeof(leed_beam) );
       j_set ++;
      }
    } /* for i_set */
    n_set = j_set;

#if CONTROL
    CONTROL_MSG(CONTROL, "%d sets in symmetrised list of beamsets:\n", n_set);
    for(i_set = 0; i_set < n_set; i_set ++)
    {
      CONTROL_MSG(CONTROL, "set %d: (%5.2f %5.2f), (%ld %ld)\n",
                 i_set, bm_off[i_set].ind_1,  bm_off[i_set].ind_2,
                        bm_off[i_set].n_eqb_s,bm_off[i_set].n_eqb_b);
    } /* for i_set */
#endif


    /* Find the beams within the radius defined by k_max:
     * - determine boundaries for beam indices n1 and n2
     * - loop over beam sets and indices.
     */

    a1 = cleed_real_hypot(g1_x, g1_y); /* a1 = length of g1 */
    a2 = cleed_real_hypot(g2_x, g2_y); /* a2 = length of g2 */

    faux_r = cleed_real_fabs((g1_x*g2_x + g1_y*g2_y)/a1); /* a2 * cos(a1,a2) */
    faux_i = cleed_real_fabs((g1_x*g2_y - g1_y*g2_x)/a1); /* a2 * sin(a1,a2) */

    /*
     * n2_max = k_max / (sin(a1,a2) * a2) + k_in/a2
     * n1_max = k_max / a1 + n2_max * (cos(a1,a2) *a2)/a1 + k_in/a1
     */
    n2_max = 2 + (int)(k_max/faux_i + k_in[0]/a2);
    n1_max = 2 + (int)( k_max/a1 + n2_max * faux_r/ a1 + k_in[0]/a1);

    CONTROL_MSG(CONTROL_X, "n1_max = %2d, n2_max = %2d ,kmax = %f\n",
                n1_max, n2_max,k_max_2);

    /* k_r, k_i is now defined by the complex energy */
    cri_sqrt(&k_r, &k_i, 2.*eng_max, 2.*v_par->eng_i);

    /* LOOP OVER BEAM SETS:
     * Create a list of all beams with the appropriate settings
     * ordered according to their beam sets and indices.
     */
    for(i_set = 0, i_beams = 0, offset = 0;i_set < n_set; 
        i_set ++, offset = i_beams)
    {
      /* Find the beams within the radius defined by k_max and
       * within the wedge defined by symmetry.
       * (loop over beam indices)
       */
      wedge = PI / bm_off[i_set].n_eqb_b;      /* half the wedge, use +/- */

      for(n1 = -n1_max; n1 <= n1_max; n1 ++)
        for(n2 = -n2_max; n2 <= n2_max; n2 ++)
        {
          k_x = n1*g1_x + n2*g2_x + k_in[1] + bm_off[i_set].k_r[1];
          k_y = n1*g1_y + n2*g2_y + k_in[2] + bm_off[i_set].k_r[2];

          faux_r = SQUARE(k_x) + SQUARE(k_y);      /* length of vector */
          faux_i = cleed_real_atan2(k_y, k_x);              /* angle of vector */

          if(faux_r <= k_max_2)
          {
            if( (bm_off[i_set].n_eqb_b == 1)    ||
                ((IS_EQUAL_REAL(k_x, 0.)) && (IS_EQUAL_REAL(k_y, 0.)))    ||
                ((( wedge - DEG_TO_RAD) > faux_i ) &&
                    ( faux_i > (-wedge - DEG_TO_RAD)))  )
              /* ~ 1 deg tolerance by adding +/- DEG_TO_RAD */
            {
              for(i=0; i<12; i++)
              {
                beams[i_beams].k_x_sym[i] = 0.;
                beams[i_beams].k_y_sym[i] = 0.;
                beams[i_beams].k_p_sym[i] = 0.;
              }

              beams[i_beams].ind_1 = (real)n1 + bm_off[i_set].ind_1;
              beams[i_beams].ind_2 = (real)n2 + bm_off[i_set].ind_2;

              k_x = n1*g1_x + n2*g2_x + bm_off[i_set].k_r[1];
              k_y = n1*g1_y + n2*g2_y + bm_off[i_set].k_r[2];

              /* rotate beamvec */
              if(n_rot == 4)
              {
                beams[i_beams].k_x_sym[1]= k_x * R_n[2][1] + k_y * R_n[2][2];
                beams[i_beams].k_y_sym[1]= k_x * R_n[2][3] + k_y * R_n[2][4];
                beams[i_beams].k_p_sym[1]= 2 * 2 * PI/n_rot;
                beams[i_beams].k_x_sym[2]= k_x * R_n[1][1] + k_y * R_n[1][2];
                beams[i_beams].k_y_sym[2]= k_x * R_n[1][3] + k_y * R_n[1][4];
                beams[i_beams].k_p_sym[2]= 1 * 2 * PI/n_rot;
                beams[i_beams].k_x_sym[3]= k_x * R_n[3][1] + k_y * R_n[3][2];
                beams[i_beams].k_y_sym[3]= k_x * R_n[3][3] + k_y * R_n[3][4];
                beams[i_beams].k_p_sym[3]= 3 * 2 * PI/n_rot;
              }
              else /* n_rot 1,2,3 */
              {
                for(i_rot = 1; i_rot < n_rot; i_rot ++)
                {
                  beams[i_beams].k_x_sym[i_rot]= k_x * R_n[i_rot][1]
                                               + k_y * R_n[i_rot][2];
                  beams[i_beams].k_y_sym[i_rot]= k_x * R_n[i_rot][3]
                                               + k_y * R_n[i_rot][4];
                  beams[i_beams].k_p_sym[i_rot]= i_rot * 2 * PI/n_rot;
                }
              }

              /* k_par, k_x/y, without the parallel component of k_in */
              beams[i_beams].k_x_sym[0]= k_x;
              beams[i_beams].k_y_sym[0]= k_y;
              beams[i_beams].k_p_sym[0]= 0.;
              beams[i_beams].k_r[1] = k_x;
              beams[i_beams].k_r[2] = k_y;
              beams[i_beams].k_par  = SQUARE(k_x) + SQUARE(k_y);

              beams[i_beams].k_i[1] = 0.;
              beams[i_beams].k_i[2] = 0.;

              beams[i_beams].n_eqb_s = bm_off[i_set].n_eqb_s;
              beams[i_beams].n_eqb_b = bm_off[i_set].n_eqb_b;


              /* OUTPUT from all beams and her symmetry related */
#if CONTROL_X
              fprintf(STDCTR, "**********************\n");
              fprintf(STDCTR, "beam%d (%f %f)(%f %f): neqb_s %lu  neqb_b %lu \n",
                      i_beams,beams[i_beams].ind_1, beams[i_beams].ind_2,
                      beams[i_beams].k_x_sym[0] * BOHR,
                      beams[i_beams].k_y_sym[0] * BOHR,
                      beams[i_beams].n_eqb_s, beams[i_beams].n_eqb_b);
              fprintf(STDCTR, "symmetry related beams:\n");
              for(i = 1 ;i < n_rot; i++)
              {
                fprintf(STDCTR, " k_x_sym %f k_y_sym %f k_p_sym %f\n",
                        beams[i_beams].k_x_sym[i] * BOHR ,
                        beams[i_beams].k_y_sym[i] * BOHR ,
                        beams[i_beams].k_p_sym[i]/DEG_TO_RAD);
              }
              fprintf(STDCTR, "**********************\n\n");
#endif

/*************************************************************************
  Symmetry related information (ONLY ROTATION):
  n_eqb_s = number of equivalent beams for overlayers (numer of sets times
            the number of beams per set).
  n_eqb_b = number of equivalent beams for bulk layers (number of equivalent
            beams in this set).

  *eout_b/s = phase factor for outgoing beams due to rotation:
            sqrt(n_rot) exp(+is*g')).

  *ein_b/s = phase factor for incoming beams due to rotation: 
            1/sqrt(n_eqb) S exp(-i(m*phi + s*g))
            The information relevant for i_layers and m will be stroed in 
            ein_b/s[ nlayers*i_layers + l_max + m ]
**************************************************************************/

              /* Akz_r = (area of the unit cell)^-1 */
              beams[i_beams].Akz_r = 1./b_par->area;

              beams[i_beams].eout_b_r =
                  (real*)malloc((b_par->n_layers)*sizeof(real));
              beams[i_beams].eout_b_i =
                  (real*)malloc((b_par->n_layers)*sizeof(real));
              beams[i_beams].eout_s_r =
                  (real*)malloc((o_par->n_layers)*sizeof(real));
              beams[i_beams].eout_s_i =
                  (real*)malloc((o_par->n_layers)*sizeof(real));

              beams[i_beams].ein_b_r =
                  (real *)malloc((2*l_max+1)*(b_par->n_layers)*sizeof(real));
              beams[i_beams].ein_b_i =
                  (real *)malloc((2*l_max+1)*(b_par->n_layers)*sizeof(real));
              beams[i_beams].ein_s_r =
                  (real *)malloc((2*l_max+1)*(o_par->n_layers)*sizeof(real));
              beams[i_beams].ein_s_i =
                  (real *)malloc((2*l_max+1)*(o_par->n_layers)*sizeof(real));


              /* bulk layers */

              for(i_layers = 0, iaux = 0; i_layers < b_par->n_layers; i_layers++)
              {
                a1_x = (b_par->layers + i_layers)->reg_shift[1];
                a1_y = (b_par->layers + i_layers)->reg_shift[2];
                pref2 = cleed_real_sqrt((real) bm_off[i_set].n_eqb_b );

                faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                        + a1_y * beams[i_beams].k_y_sym[0];
                cri_expi( &faux_r, &faux_i, faux_r, 0.);
         
                *(beams[i_beams].eout_b_r + i_layers) = pref2 * faux_r;
                *(beams[i_beams].eout_b_i + i_layers) = pref2 * faux_i;

                pref2 = 1./cleed_real_sqrt((real) bm_off[i_set].n_eqb_b );
                for(m = -l_max; m <= l_max; m ++, iaux ++)
                {
                  if( bm_off[i_set].n_eqb_b == 1)
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_b_r + iaux,
                              beams[i_beams].ein_b_i + iaux,
                              -faux_r, 0.);

                  }
                  else
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_b_r + iaux,
                              beams[i_beams].ein_b_i + iaux,
                              -faux_r, 0.);

                    for(i_rot = 1; i_rot < bm_off[i_set].n_eqb_b; i_rot ++)
                    {
                      a2_x = beams[i_beams].k_x_sym[i_rot];
                      a2_y = beams[i_beams].k_y_sym[i_rot];
                      pref1 = m * beams[i_beams].k_p_sym[i_rot];

                      faux_r = a1_x * a2_x + a1_y * a2_y + pref1;
                      cri_expi(&faux_r, &faux_i, -faux_r, 0.);
                      *(beams[i_beams].ein_b_r + iaux) += faux_r;
                      *(beams[i_beams].ein_b_i + iaux) += faux_i;
                    }
                  }  /* else */

                  *(beams[i_beams].ein_b_r + iaux) *= pref2;
                  *(beams[i_beams].ein_b_i + iaux) *= pref2;

                }  /* for m */
              }  /* for i_layers */

              /* superlattice (full rotational symmetry), use o_par */
              for(i_layers = 0, iaux = 0; i_layers < o_par->n_layers; i_layers++)
              {
                a1_x = (o_par->layers + i_layers)->reg_shift[1];
                a1_y = (o_par->layers + i_layers)->reg_shift[2];
                pref2 = cleed_real_sqrt((real) bm_off[i_set].n_eqb_s);

                faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                        + a1_y * beams[i_beams].k_y_sym[0];
                cri_expi( &faux_r, &faux_i, faux_r, 0.);
         
                *(beams[i_beams].eout_s_r + i_layers) = pref2 * faux_r;
                *(beams[i_beams].eout_s_i + i_layers) = pref2 * faux_i;

          
                pref2 = 1./cleed_real_sqrt((real) bm_off[i_set].n_eqb_s );
                for(m = -l_max; m <= l_max; m ++, iaux ++)
                {
                  if( bm_off[i_set].n_eqb_s == 1)
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_s_r + iaux,
                              beams[i_beams].ein_s_i + iaux,
                              -faux_r, 0.);
                  }
                  else
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_s_r + iaux,
                              beams[i_beams].ein_s_i + iaux,
                              -faux_r, 0.);

                    for(i_rot = 1; i_rot < n_rot; i_rot ++)
                    {
                      a2_x = beams[i_beams].k_x_sym[i_rot];
                      a2_y = beams[i_beams].k_y_sym[i_rot];
                      pref1 = m * beams[i_beams].k_p_sym[i_rot];

                      faux_r = a1_x * a2_x + a1_y * a2_y + pref1;
                      cri_expi(&faux_r, &faux_i, -faux_r, 0.);
                      *(beams[i_beams].ein_s_r + iaux) += faux_r;
                      *(beams[i_beams].ein_s_i + iaux) += faux_i;
                    }

                    *(beams[i_beams].ein_s_r + iaux) *= pref2;
                    *(beams[i_beams].ein_s_i + iaux) *= pref2;
                  }  /* else */
                }  /* for m */
              }  /* for i_layers */

              /* beam set */

              beams[i_beams].set = i_set;
              i_beams ++;

            } /*if wedge...**/
          } /* if inside k_max */
        } /* for n1/n2 */

      /* 1st pass: Sort the beams according to the parallel component
       * (i.e. smallest k_par first)
       */
      CONTROL_MSG(CONTROL_X, "SORTING %2d beams in set %d:\n", i_beams - offset, i_set);

      for(n1 = offset; n1 < i_beams; n1 ++)
      {
        for(n2 = n1+1; n2 < i_beams; n2 ++)
        {
          if((beams + n2)->k_par < (beams + n1)->k_par )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
        } /* n2 */
     } /* n1 */

      /* 2nd pass: Sort the beams according to the 1st and 2nd index
       * (i.e. smallest indices first)
       */
      for(n1 = offset; n1 < i_beams; n1 ++)
      {
        for(n2 = n1+1;
            fabs( (beams + n2)->k_par - (beams + n1)->k_par ) < K_TOLERANCE;
            n2++)
        {
          if((beams + n2)->ind_1 < (beams + n1)->ind_1 )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
          if( IS_EQUAL_REAL((beams + n2)->ind_1, (beams + n1)->ind_1) &&
              ((beams + n2)->ind_2 < (beams + n1)->ind_2 )  )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
        } /* n2 */
      } /* n1 */
    } /* for i_set */

    /* Reset the symmetry structure elements for (0,0) beam (always
     * first in the list) (0,0) is an exception since it can only
     * represent 1 beam, although the symmetry might be higher.
     */
    (beams + 0)->n_eqb_b = 1;
    (beams + 0)->n_eqb_s = 1;

    /* bulk layers */
    for(i_layers = 0, iaux = 0; i_layers < b_par->n_layers; i_layers ++)
    {
      *((beams+0)->eout_b_r + i_layers) = 1.;
      *((beams+0)->eout_b_i + i_layers) = 0.;

      for(m = -l_max; m <= l_max; m ++, iaux ++)
      {
        *((beams+0)->ein_b_r + iaux) = 1.;
        *((beams+0)->ein_b_i + iaux) = 0.;
      } /* for m */
    } /* for i_layers */

    /* overlayers */
    for(i_layers = 0, iaux = 0; i_layers < o_par->n_layers; i_layers ++)
    {
      *((beams+0)->eout_s_r + i_layers) = 1.;
      *((beams+0)->eout_s_i + i_layers) = 0.;

      for(m = -l_max; m <= l_max; m ++, iaux ++)
      {
        *((beams+0)->ein_s_r + iaux) = 1.;
        *((beams+0)->ein_s_i + iaux) = 0.;
      }
    } /* for i_layers */

    /* Set k_par of the last element of the list to the terminating value.
     * free R_n.
     */
      beams[i_beams].k_par = F_END_OF_LIST;
      leed_beam_rotation_matrix_free(R_n);

      break;
    }  /* case rotsymmetry */


    /* CASE MIRRORSYMMETRY */
    case HEX_3MIR:
    case HEX_1MIR:
    case SQ_1MIR:
    case REC_1MIR:
    case MONO_1MIR:
    case REC_2MIR:
    case SQ_2MIR:
    case SQ_4MIR:
    {

      for(i_set = 1; i_set < n_set; i_set ++)
      {
        if( bm_off[i_set].n_eqb_s != 0 )
        {
          equalset1 = bm_off[i_set].ind_1;
          equalset2 = bm_off[i_set].ind_2;

          for(j = 0; j < 2; j++)
          {
            for(i = 0; i < n_mir - j; i++)
            {
              k_x = bm_off[i_set].k_r[1];
              k_y = bm_off[i_set].k_r[2];

              R_m[1] = cleed_real_cos(2* b_par->alpha[i]);
              R_m[2] = cleed_real_sin(2* b_par->alpha[i]);
              R_m[3] = R_m[2];
              R_m[4] = - R_m[1];


              /* calculate R^-1 = At^-1 R At */
              m11 = R_m[1]*a1_x*a2_y + R_m[2]*a1_y*a2_y
                  - R_m[3]*a1_x*a2_x - R_m[4]*a1_y*a2_x;
              m11 *= det;
              m21 = - R_m[1]*a1_x*a1_y - R_m[2]*a1_y*a1_y
                    + R_m[3]*a1_x*a1_x + R_m[4]*a1_y*a1_x;
              m21 *= det;
              m12 = R_m[1]*a2_x*a2_y + R_m[2]*a2_y*a2_y
                  - R_m[3]*a2_x*a2_x - R_m[4]*a2_y*a2_x;
              m12 *= det;
              m22 = - R_m[1]*a2_x*a1_y - R_m[2]*a2_y*a1_y
                    + R_m[3]*a2_x*a1_x + R_m[4]*a2_y*a1_x;
              m22 *= det;
 
              /* calculate (ind_1(i),ind_2(i)) R^-1 */
              k_x = equalset1 * m11 + equalset2 * m21;
              k_y = equalset1 * m12 + equalset2 * m22;

              CONTROL_MSG(CONTROL_X,
                  "k_x = %f , k_y = %f (ind_1 = %f ind_2 = %f)\n",
                  k_x, k_y, equalset1, equalset2);

              /* If (ind_1(i),ind_2(i)) R^-1 - ((ind_1(i),ind_2(i)) is integer,
               * the beam set is equivalent to itself. Mark this in n_eqb_b.
               */
              faux_r = k_x - bm_off[i_set].ind_1;

              CONTROL_MSG(CONTROL_X,
                  "faux_r = %f k_x = %f ind_1(set %d) = %f\n",
                  faux_r, k_x, i_set, bm_off[i_set].ind_1);

              if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
              {
                faux_r = k_y - bm_off[i_set].ind_2;

                CONTROL_MSG(CONTROL_X,
                    "faux_r = %f k_y = %f ind_2(set %d) = %f\n",
                    faux_r, k_y, i_set, bm_off[i_set].ind_2);

                if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
                {
                  bm_off[i_set].n_eqb_b ++;

                  CONTROL_MSG(CONTROL_X,
                      "set %d has mirror symmetry alpha = %f\n",
                      i_set, b_par->alpha[i]/DEG_TO_RAD);
                }
              }

              /* If (ind_1(i),ind_2(i)) R^-1 - ((ind_1(j),ind_2(j)) is integer,
               * the beam sets i and j are equivalent. Mark this in n_eqb_s.
               */
              for(j_set = i_set+1; j_set < n_set; j_set ++)
              {
                if( bm_off[i_set].n_eqb_s != 0 && (bm_off+j_set)->n_eqb_s != 0)
                {
                  faux_r = k_x - (bm_off+j_set)->ind_1;
                  if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
                  {
                    faux_r = k_y - (bm_off+j_set)->ind_2;
                    if( cleed_real_fabs(faux_r - cleed_real_nint(faux_r)) < K_TOLERANCE ) /* integer ? */
                    {
                      bm_off[i_set].n_eqb_s ++;
                      (bm_off+j_set)->n_eqb_s = 0;

                      CONTROL_MSG(CONTROL_X,
                          "sets %d and %d are equivalent (alpha = %f)\n",
                          i_set, j_set,b_par->alpha[i]/DEG_TO_RAD);
                    }
                  }
                } /* if n_eqb_s */
              } /* for j_set */

              equalset1 = k_x;
              equalset2 = k_y;

            } /*for n_mir*/
          } /*for j --- n_mir-j */
        } /*if n_eqb_s != 0 */
      } /*for i_set */

      /* At this point:
       * ind_1, ind_2 = beam indices of one beam in the beam set.
       * n_eqb_s == 0 <=> this beam set can be omitted because it is represented
       *                  by another set
       *         != 0 <=> this beam set represents other beam sets. The number of
       *                  beams is n_eqb_s * n_eqb_b.
       * n_eqb_b = number of beams within the beam set related by symmetry.
       */
#if CONTROL
      CONTROL_MSG(CONTROL, "%d unsymmetrised beam sets:\n", n_set);
      for(i_set = 0; i_set < n_set; i_set ++)
      {
        CONTROL_MSG(CONTROL, "set %d:(%5.2f %5.2f) ",
                i_set, bm_off[i_set].ind_1,   bm_off[i_set].ind_2);

        if(bm_off[i_set].n_eqb_s > 0)
        {
          fprintf(STDCTR,"represents %lu sets (%lu beam(s)).\n",
             bm_off[i_set].n_eqb_s, bm_off[i_set].n_eqb_b);
        }
        else
        {
          fprintf(STDCTR,"\t\tis equivalent to %lu (%5.2f %5.2f).\n",
             bm_off[i_set].n_eqb_b,
             (bm_off+ bm_off[i_set].n_eqb_b)->ind_1,
             (bm_off+ bm_off[i_set].n_eqb_b)->ind_2);
        }
      } /* for i_set */
#endif 

    /* Remove the equivalent beam sets from the list beam_off:
     * - set No 0 (i.o. beams) will always be included in the list,
     *   therefore start loop at 1.
     * - set n_eqb_s to real number of beams (n_eqb_b * n_eqb_s)
     */

    (bm_off+0)->n_eqb_s=(bm_off+0)->n_eqb_s*(bm_off+0)->n_eqb_b;

    for(i_set = 1, j_set = 1; i_set < n_set; i_set ++)
    {
      if( bm_off[i_set].n_eqb_s > 0)
      {
        bm_off[i_set].n_eqb_s = bm_off[i_set].n_eqb_s*bm_off[i_set].n_eqb_b;
#if WARNING_LOG
        if( bm_off[i_set].n_eqb_s != 2*n_mir)
        {
          WARNING_MSG("n_eqb_s * n_eqb_b = %lu "
                      "!= n_mir = %d for beam set (%5.2f %5.2f)\n",
                      bm_off[i_set].n_eqb_s,
                      2*n_mir, bm_off[i_set].ind_1, bm_off[i_set].ind_2);
        }
#endif
        if( i_set > j_set )
          memcpy( bm_off+j_set, bm_off+i_set, sizeof(leed_beam) );
        j_set ++;
      }
    } /* for i_set */
    n_set = j_set;

#if CONTROL
    CONTROL_MSG(CONTROL, "%d sets in symmetrised list of beamsets:\n", n_set);
    for(i_set = 0; i_set < n_set; i_set ++)
    {
      CONTROL_MSG(CONTROL, "set %d: (%5.2f %5.2f), (%lu %lu)\n",
                  i_set, bm_off[i_set].ind_1,  bm_off[i_set].ind_2,
                         bm_off[i_set].n_eqb_s,bm_off[i_set].n_eqb_b);
    } /* for i_set */
#endif


  /* Find the beams within the radius defined by k_max:
   * - determine boundaries for beam indices n1 and n2
   * - loop over beam sets and indices.
   */
  a1 = cleed_real_hypot(g1_x, g1_y); /* a1 = length of g1 */
  a2 = cleed_real_hypot(g2_x, g2_y); /* a2 = length of g2 */

  faux_r = cleed_real_fabs((g1_x*g2_x + g1_y*g2_y)/a1); /* a2 * cos(a1,a2) */
  faux_i = cleed_real_fabs((g1_x*g2_y - g1_y*g2_x)/a1); /* a2 * sin(a1,a2) */

  /*
   * n2_max = k_max / (sin(a1,a2) * a2) + k_in/a2
   * n1_max = k_max / a1 + n2_max * (cos(a1,a2) *a2)/a1 + k_in/a1
   */
  n2_max = 2 + (int)(k_max/faux_i + k_in[0]/a2);
  n1_max = 2 + (int)( k_max/a1 + n2_max * faux_r/ a1 + k_in[0]/a1);

  CONTROL_MSG(CONTROL_X, "n1_max = %2d, n2_max = %2d ,kmax = %f\n",
              n1_max, n2_max, k_max_2);

  /* k_r, k_i is now defined by the complex energy */
  cri_sqrt(&k_r, &k_i, 2.*eng_max, 2.*v_par->eng_i);


  /* LOOP OVER BEAM SETS:
   * Create a list of all beams with the appropriate settings
   * ordered according to their beam sets and indices.
   */
  det = g1_x*g2_y - g1_y*g2_x;

  for(i_set = 0, i_beams = 0, offset = 0;i_set < n_set;
      i_set ++, offset = i_beams)
  {
    /* Find the beams within the radius defined by k_max and
     * within the wedge defined by symmetry.
     * (loop over beam indices)
     */
      for(n1 = -n1_max; n1 <= n1_max; n1 ++)
        for(n2 = -n2_max; n2 <= n2_max; n2 ++)
        {
          k_x = n1*g1_x + n2*g2_x + k_in[1] + bm_off[i_set].k_r[1];
          k_y = n1*g1_y + n2*g2_y + k_in[2] + bm_off[i_set].k_r[2];

          faux_r = SQUARE(k_x) + SQUARE(k_y);      /* length of vector */
          faux_i = cleed_real_atan2(k_y, k_x);              /* angle of vector */
          wedge = faux_i - b_par->alpha[0];

          if(wedge < 0)
            wedge += 2*PI;

          if( faux_r <= k_max_2  &&
              ((IS_EQUAL_REAL(k_x, 0.) && IS_EQUAL_REAL(k_y, 0.)) ||
                  wedge <= (2*PI)/bm_off[i_set].n_eqb_b+GEO_TOLERANCE))
          {
            k_x = n1*g1_x + n2*g2_x + bm_off[i_set].k_r[1];
            k_y = n1*g1_y + n2*g2_y + bm_off[i_set].k_r[2];

            /* TEST IF the new BEAM is needed
             * if not : set ctrol to 1 and leave the ...if k_max without
             */
            ctrol = 0;
            for(i= offset ; i < i_beams ; i++)
            {
              for(j=0; j< (beams + i)->n_eqb_s; j++)
              {
                if( (cleed_real_fabs(k_x - (beams + i)->k_x_sym[j])< K_TOLERANCE) &&
                    (cleed_real_fabs(k_y - (beams + i)->k_y_sym[j])< K_TOLERANCE) )
                {
                  ctrol = 1;
                }
              }
            }

            if(ctrol != 1)
            {
              CONTROL_MSG(CONTROL_X, "(%5.2f %5.2f): %5.2f %5.2f\n",
                          n1 + bm_off[i_set].ind_1, n2 + bm_off[i_set].ind_2,
                          faux_r, cleed_real_atan2(k_y, k_x) + PI);

              for(i=0; i<12; i++)
              {
                beams[i_beams].k_x_sym[i] = 0.;
                beams[i_beams].k_y_sym[i] = 0.;
                beams[i_beams].k_p_sym[i] = 0.;
              }
     
              beams[i_beams].ind_1 = (real)n1 + bm_off[i_set].ind_1;
              beams[i_beams].ind_2 = (real)n2 + bm_off[i_set].ind_2;

              beams[i_beams].k_x_sym[0] = k_x;
              beams[i_beams].k_y_sym[0] = k_y;
              beams[i_beams].k_p_sym[0] = 0.;
              alpha[1] = cleed_real_atan2(k_y,k_x);

              k_x_mir[0] = k_x;
              k_y_mir[0] = k_y;
              k_p_mir[0] = 0.;

              /* REFLECT AND SORT */
              i_c = 0;
              i_d = 1;
              ctrol = 0;
              for(j=0; j<2; j++)
              {
                for(i = 0 ; i < n_mir - j; i++)
                {
                  R_m[1] = cleed_real_cos(2* b_par->alpha[i]);
                  R_m[2] = cleed_real_sin(2* b_par->alpha[i]);
                  R_m[3] = R_m[2];
                  R_m[4] = - R_m[1];

                  iaux = ctrol + 1;
                  ctrol++;
                  k_x_mir[iaux] = k_x_mir[iaux-1] * R_m[1]
                                + k_y_mir[iaux-1] * R_m[2];
                  k_y_mir[iaux] = k_x_mir[iaux-1] * R_m[3]
                                + k_y_mir[iaux-1] * R_m[4];

                  for(i_e = 0 ; i_e < iaux ; i_e++)
                  {
                    if(cleed_real_fabs(k_x_mir[iaux] - k_x_mir[i_e]) < K_TOLERANCE &&
                       cleed_real_fabs(k_y_mir[iaux] - k_y_mir[i_e]) < K_TOLERANCE    )
                    {
                      ctrol--;
                    }
                  }
         
                  if(iaux == ctrol)
                  {
                    alpha[2] = cleed_real_atan2(k_y_mir[iaux],k_x_mir[iaux]);
                    k_p_mir[iaux]= alpha[2] - alpha[1];
                    if(k_p_mir[iaux] < 0)
                      k_p_mir[iaux] += 2*PI;

                    vaux[0] = k_x - k_x_mir[iaux];
                    vaux[1] = k_y - k_y_mir[iaux];
                    integ[0] = cleed_real_fabs((vaux[1] * g1_x - vaux[0] * g1_y)/det);
                    integ[1] = cleed_real_fabs((vaux[1] * g2_x - vaux[0] * g2_y)/(-det));

                    vaux[0] = integ[0] - cleed_real_nint(integ[0]);
                    vaux[1] = integ[1] - cleed_real_nint(integ[1]);

                    if(cleed_real_fabs(vaux[0]) > K_TOLERANCE ||
                       cleed_real_fabs(vaux[1]) > K_TOLERANCE  )
                    {
                      k_x_store[i_c] = k_x_mir[iaux];
                      k_y_store[i_c] = k_y_mir[iaux];
                      k_p_store[i_c] = k_p_mir[iaux];
                      i_c++;
                    }
                    else
                    {
                      beams[i_beams].k_x_sym[i_d] = k_x_mir[iaux];
                      beams[i_beams].k_y_sym[i_d] = k_y_mir[iaux];
                      beams[i_beams].k_p_sym[i_d] = k_p_mir[iaux];
                      i_d++;
                    }
                  } /* if ctrol = iaux */
                } /* for i */
              } /* for j */

              for(i=0; i < i_c; i++)
              {
                beams[i_beams].k_x_sym[i_d + i] = k_x_store[i];
                beams[i_beams].k_y_sym[i_d + i] = k_y_store[i];
                beams[i_beams].k_p_sym[i_d + i] = k_p_store[i];
              }

              beams[i_beams].n_eqb_s = i_d + i_c;
              beams[i_beams].n_eqb_b = i_d;
              beams[i_beams].k_x_sym[i_d + i_c] = F_END_OF_LIST;
              beams[i_beams].k_y_sym[i_d + i_c] = F_END_OF_LIST;
              beams[i_beams].k_p_sym[i_d + i_c] = F_END_OF_LIST;
 
              /* OUTPUT of all beams and their equivalents by symmetry */
#if CONTROL_X
              fprintf(STDCTR,"********************\n");
              fprintf(STDCTR,"beam%d (%f %f) (%f %f): neqb_s %ld  neqb_b %ld \n",
                      i_beams,beams[i_beams].ind_1,beams[i_beams].ind_2,
                      beams[i_beams].k_x_sym[0],beams[i_beams].k_y_sym[0],
                      beams[i_beams].n_eqb_s,beams[i_beams].n_eqb_b);
              fprintf(STDCTR,"symmetry related beams:\n");
              for(i = 1 ;i < i_d + i_c; i++)
              {
                fprintf(STDCTR," k_x_sym %f k_y_sym %f k_p_sym %f\n",
                        beams[i_beams].k_x_sym[i] * BOHR ,
                        beams[i_beams].k_y_sym[i] * BOHR ,
                        beams[i_beams].k_p_sym[i]/DEG_TO_RAD);
              }
#endif

              /* Symmetry related information (ONLY ROTATION):
               * n_eqb_s = number of equivalent beams for overlayers
               * (number of sets times the number of beams per set).
               *
               * n_eqb_b = number of equivalent beams for bulk layers
               * (number of equivalent beams in this set).
               *
               * *eout_b/s = phase factor for outgoing beams due to rotation:
               * sqrt(n_rot) exp(+is*g')).
               *
               * *ein_b/s = phase factor for incoming beams due to rotation:
               * 1/sqrt(n_eqb) S exp(-i(m*phi + s*g))
               *
               * The information relevant for i_layers and m will be stroed in
               * ein_b/s[ nlayers*i_layers + l_max + m ]
               */

              /* k_par, k_x/y, without the parallel component of k_in */
              beams[i_beams].k_r[1] = k_x;
              beams[i_beams].k_r[2] = k_y;
              beams[i_beams].k_par  = SQUARE(k_x) + SQUARE(k_y);

              beams[i_beams].k_i[1] = 0.;
              beams[i_beams].k_i[2] = 0.;

              /* Akz_r = (area of the unit cell)^-1 */
              beams[i_beams].Akz_r = 1./b_par->area;

              beams[i_beams].eout_b_r =
                  (real*)malloc((b_par->n_layers)*sizeof(real));
              beams[i_beams].eout_b_i =
                  (real*)malloc((b_par->n_layers)*sizeof(real));
              beams[i_beams].eout_s_r =
                  (real*)malloc((o_par->n_layers)*sizeof(real));
              beams[i_beams].eout_s_i =
                  (real*)malloc((o_par->n_layers)*sizeof(real));

              beams[i_beams].ein_b_r =
                  (real *)malloc((2*l_max+1)*(b_par->n_layers)*sizeof(real));
              beams[i_beams].ein_b_i =
                  (real *)malloc((2*l_max+1)*(b_par->n_layers)*sizeof(real));
              beams[i_beams].ein_s_r =
                  (real *)malloc((2*l_max+1)*(o_par->n_layers)*sizeof(real));
              beams[i_beams].ein_s_i =
                  (real *)malloc((2*l_max+1)*(o_par->n_layers)*sizeof(real));

              /* bulk layers */
              for(i_layers = 0, iaux = 0;
                  i_layers < b_par->n_layers;
                  i_layers++)
              {
                a1_x = (b_par->layers + i_layers)->reg_shift[1];
                a1_y = (b_par->layers + i_layers)->reg_shift[2];
                pref2 = cleed_real_sqrt((real) beams[i_beams].n_eqb_b );

                faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                        + a1_y * beams[i_beams].k_y_sym[0];
                cri_expi( &faux_r, &faux_i, faux_r, 0.);

                *(beams[i_beams].eout_b_r + i_layers) = pref2 * faux_r;
                *(beams[i_beams].eout_b_i + i_layers) = pref2 * faux_i;

                pref2 = 1./cleed_real_sqrt((real) beams[i_beams].n_eqb_b );
                for(m = -l_max; m <= l_max; m ++, iaux ++)
                {
                  if( beams[i_beams].n_eqb_b == 1)
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_b_r + iaux,
                              beams[i_beams].ein_b_i + iaux,
                              -faux_r, 0.);
                  }
                  else
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_b_r + iaux,
                              beams[i_beams].ein_b_i + iaux,
                              -faux_r, 0.);

                    for(i = 1; i < beams[i_beams].n_eqb_b; i++)
                    {
                      a2_x = beams[i_beams].k_x_sym[i];
                      a2_y = beams[i_beams].k_y_sym[i];
                      pref1 = m * beams[i_beams].k_p_sym[i];

                      faux_r = a1_x * a2_x + a1_y * a2_y + pref1;
                      cri_expi(&faux_r, &faux_i, -faux_r, 0.);
                      *(beams[i_beams].ein_b_r + iaux) += faux_r;
                      *(beams[i_beams].ein_b_i + iaux) += faux_i;
                    }
                  } /* else */

                  *(beams[i_beams].ein_b_r + iaux) *= pref2;
                  *(beams[i_beams].ein_b_i + iaux) *= pref2;

                } /* for m */
              } /* for i_layers */

              /* superlattice (full rotational symmetry), use o_par */
              for(i_layers = 0, iaux = 0;
                  i_layers < o_par->n_layers;
                  i_layers++)
              {
                a1_x = (o_par->layers + i_layers)->reg_shift[1];
                a1_y = (o_par->layers + i_layers)->reg_shift[2];
                pref2 = cleed_real_sqrt((real) beams[i_beams].n_eqb_s);

                faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                        + a1_y * beams[i_beams].k_y_sym[0];
                cri_expi( &faux_r, &faux_i, faux_r, 0.);

                *(beams[i_beams].eout_s_r + i_layers) = pref2 * faux_r;
                *(beams[i_beams].eout_s_i + i_layers) = pref2 * faux_i;

                pref2 = 1./cleed_real_sqrt((real) beams[i_beams].n_eqb_s );
                for(m = -l_max; m <= l_max; m ++, iaux ++)
                {
                  if( beams[i_beams].n_eqb_s == 1)
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_s_r + iaux,
                              beams[i_beams].ein_s_i + iaux,
                              -faux_r, 0.);
                  }
                  else
                  {
                    faux_r =  a1_x * beams[i_beams].k_x_sym[0]
                            + a1_y * beams[i_beams].k_y_sym[0];
                    cri_expi( beams[i_beams].ein_s_r + iaux,
                              beams[i_beams].ein_s_i + iaux,
                              -faux_r, 0.);

                    for(i = 1; i < beams[i_beams].n_eqb_s; i++)
                    {
                      a2_x = beams[i_beams].k_x_sym[i];
                      a2_y = beams[i_beams].k_y_sym[i];
                      pref1 = m * beams[i_beams].k_p_sym[i];

                      faux_r = a1_x * a2_x + a1_y * a2_y + pref1;
                      cri_expi(&faux_r, &faux_i, -faux_r, 0.);
                      *(beams[i_beams].ein_s_r + iaux) += faux_r;
                      *(beams[i_beams].ein_s_i + iaux) += faux_i;
                    }

                    *(beams[i_beams].ein_s_r + iaux) *= pref2;
                    *(beams[i_beams].ein_s_i + iaux) *= pref2;
                  } /* else */
                } /* for m */
              } /* for i_layers */

              beams[i_beams].set = i_set;
              i_beams++;

            } /* if ctrol != 1*/
          } /* if inside k_max */
        } /* for n1/n2 */

      /* 1st pass: Sort the beams according to the parallel component
       * (i.e. smallest k_par first)
       */
      CONTROL_MSG(CONTROL, "SORTING %2d beams in set %d:\n",
                  i_beams - offset, i_set);

      for(n1 = offset; n1 < i_beams; n1 ++)
      {
        for(n2 = n1+1; n2 < i_beams; n2 ++)
        {
          if((beams + n2)->k_par < (beams + n1)->k_par )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
        } /* n2 */
      } /* n1 */

      /* 2nd pass: Sort the beams according to the 1st and 2nd index
       * (i.e. smallest indices first)
       */
      for(n1 = offset; n1 < i_beams; n1 ++)
      {
        for(n2 = n1+1;
            fabs( (beams + n2)->k_par - (beams + n1)->k_par ) < K_TOLERANCE;
            n2++)
        {
          if((beams + n2)->ind_1 < (beams + n1)->ind_1 )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
          if( IS_EQUAL_REAL((beams + n2)->ind_1, (beams + n1)->ind_1 ) &&
              ((beams + n2)->ind_2 < (beams + n1)->ind_2 )  )
          {
            memcpy( & beam_aux, beams + n2, sizeof(leed_beam) );
            memcpy( beams + n2, beams + n1, sizeof(leed_beam) );
            memcpy( beams + n1, & beam_aux, sizeof(leed_beam) );
          }
        } /* n2 */
      } /* n1 */
    } /* for i_set */

    /* Set k_par of the last element of the list to the terminating value.
     * free R_n.
     */
    beams[i_beams].k_par = F_END_OF_LIST;

    break;

    }  /* case MIRRORSYMMETRY */


    default:
    {
      ERROR_MSG("symmetry flag is false\n");
      exit(1);
      break;
    }

  } /*switch sym*/

  return(n_set);
} /* end of function leed_beam_gen_sym */
