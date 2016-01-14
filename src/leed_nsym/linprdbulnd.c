/************************************************************************
 *                      LINPRDBULND.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/17.08.94 - insert CLEED_REAL_IS_DOUBLE and CLEED_REAL_IS_FLOAT for sscanf.
 *   GH/20.04.95 - read superstructure matrix
 *   GH/19.07.95 - read vibrational amplitudes (root mean square)
 *   GH/25.07.95 - introduce structure element "layers" in bulk_par
 *                 and eliminate "atoms"
 *   GH/04.09.97 - include symmetry flags.
 *   GH/03.05.00 - read parameters for non-diagonal t matrix
 *   WB/05.05.98 - read rotaxis
 *   WB/17.08.98 - include mirror planes
 *   WB/19.08.98 - realloc atoms_rd iatoms+2
 *   WB/26.08.98 - read mirror planes,also more lines
 *   WB/27.08.98 - switch symmetry flags
 *   WB/14.09.98 - read only one mirror plane in one line
 *   GH/29.09.00 - calculate dr2 for dmt input in function leed_inp_debye_temp
 *********************************************************************/

/*! \file
 *
 * Reads all the bulk parameters (ones that do not change during a search).
 */

#include <math.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "leed.h"
#include "leed_def.h"

static const double SQRT3 = 1.73205080756887729352; /*!< \f$ \sqrt{3} \f$ */

/*!
 * Reads all the bulk parameters that do not change during a search.
 *
 * The letters 'e' - 'k' are reserved as identifiers for parameter input
 * through function leed_inp_leed_read_par
 *
 * currently recognized identifier:
 *
 * 'a1','a2','a3': lattice vectors of bulk unit cell
 * 'b1','b2': superstructure unit cell.
 * 'c': comment
 *
 * 'm1','m2': superstructure matrix.
 * 'pb': postion and type of bulk atoms.
 * 'vi','vr': imaginary and real part of optical potential.
 *
 * \param p_bulk_par
 * \param p_phs_shifts
 * \param filename Name of input file to read from.
 * \return Error code indicating function success.
 * \retval 0 if successful.
 * \retval -1 if failed and #EXIT_ON_ERROR not defined.
 *
 * \warning Function will exit program with code \c errno upon
 * memory (re)allocation failure.
 * 
 * \see leed_inp_phase_nd() and leed_inp_bul_layer()
 */
int leed_inp_read_bul_nd(leed_crystal **p_bulk_par, leed_phase **p_phs_shifts,
                         const char *filename)
{
  FILE *inp_stream = NULL;

  /* input buffers */
  char linebuffer[STRSZ] = "";
  char phaseinp[STRSZ] = "";
  char whatnext[STRSZ] = "";
  size_t i, j;
  int iaux;                     /* used as a counter or for dummy variables */
  size_t i_c, i_str;
  size_t i_com = 0;             /* number of comments */
  size_t i_atoms = 0;           /* counter for number of atoms */
  size_t i_layer = 0;           /* counter for number of layers */
  size_t i_plane = 0;           /* only used for mirror planes */

  real faux;                    /* dummy variable */
  real a1[4], a2[4], a3[4];     /* vectors: 1=x, 2=y, 3=z, 0 is not used */
  real vaux[4];                 /* dummy vector */

  leed_crystal *bulk_par = NULL;/* use instead of pointer p_bulk_par */

  leed_atom atom_aux;           /* used for sorting atoms */
  leed_atom *atoms_rd = NULL;   /* this vector of structure atom_str is
                                   used to read and treat the input atomic
                                   properties and will be copied into bulk_par
                                   afterwards */

  /* If bulk_par or phs_shifts is NULL: allocate memory and copy back */
  bulk_par = *p_bulk_par; /* initialise */
  if (*p_bulk_par == NULL)
  {
    bulk_par = *p_bulk_par = (leed_crystal *) calloc(1, sizeof(leed_crystal));
    CLEED_ALLOC_CHECK(bulk_par);
  }

  /* Preset parameters
   * - allocate atoms_rd (1 unit)
   * - allocate bulk_par->comments (1 unit)
   * - set m_trans, m_super, m_recip to identity
   * - set ai[j] and bulk_par->b[i] to 0.
   * - set n_types to zero.
   * - set temperature (bulk_par->temp) to room temperature (DEF_TEMP = 300K);
   * - set symmetry flags to no symmetry.
   */

  atoms_rd = (leed_atom *) malloc(2*sizeof(leed_atom));
  for (i_c = 0; i_c < 4; i_c++) a1[i_c] = a2[i_c] = a3[i_c] = 0.;

  /* set initial bulk parameters */
  CLEED_ALLOC_CHECK(bulk_par->m_plane = (real *) calloc(1, sizeof(real)));
  CLEED_ALLOC_CHECK(bulk_par->alpha   = (real *) calloc(1, sizeof(real)));

  CLEED_ALLOC_CHECK(bulk_par->comments = (char **) calloc(1, sizeof(char *)));
  *(bulk_par->comments) = NULL;

  bulk_par->m_trans[1] = bulk_par->m_trans[4] = 1.;
  bulk_par->m_trans[2] = bulk_par->m_trans[3] = 0.;

  bulk_par->m_super[1] = bulk_par->m_super[4] = 1.;
  bulk_par->m_super[2] = bulk_par->m_super[3] = 0.;

  bulk_par->m_recip[1] = bulk_par->m_recip[4] = 1.;
  bulk_par->m_recip[2] = bulk_par->m_recip[3] = 0.;
  for (i_c = 0; i_c < 5; i_c++) bulk_par->b[i_c] = 0.;

  bulk_par->temp = DEF_TEMP;
  bulk_par->n_types = 0;
  bulk_par->n_rot = 1;
  bulk_par->rot_axis[1] = bulk_par->rot_axis[2] = 0.;
  bulk_par->n_mir = 0;
  bulk_par->symmetry = NOSYM;

  /* Open and Read input file */
  if ((inp_stream = fopen(filename, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", filename);
    ERROR_RETURN(-1);
  }

  CONTROL_MSG(CONTROL, "Reading file \"%s\"\n", filename);

  while (fgets(linebuffer, STRSZ, inp_stream) != NULL)
  {
    CONTROL_MSG(CONTROL_X, "%s", linebuffer);

    /* find first non blank character */
    for (i_str = 0; *(linebuffer + i_str) == ' '; i_str++)
    {;}
    switch (*(linebuffer + i_str))
    {
      case ('a'): case ('A'): /* input of bulk unit cell parameters */
      {
        switch (*(linebuffer + i_str + 1))
        {
          case ('1'): /* a1 */
          {
            #ifdef CLEED_REAL_IS_DOUBLE
            if (sscanf(linebuffer+i_str+3, " %lf %lf %lf",
                       &a1[1], &a1[2], &a1[3]) < 2)
            #else
            if(sscanf(linebuffer+i_str+3, " %f %f %f",
                      &a1[1], &a1[2], &a1[3]) < 2)
            #endif
            {
              ERROR_MSG("need at least x/y coordinates of a1\n");
              ERROR_RETURN(-1);
            }

            a1[1] /= BOHR; a1[2] /= BOHR; a1[3] /= BOHR;
            break;
          } /* a1 */

          case ('2'): /* a2 */
          {
            #ifdef CLEED_REAL_IS_DOUBLE
            if (sscanf(linebuffer + i_str + 3, " %lf %lf %lf",
                       &a2[1], &a2[2], &a2[3]) < 2)
            #else
            if(sscanf(linebuffer + i_str + 3, " %f %f %f",
                      &a2[1], &a2[2], &a2[3]) < 2)
            #endif
            {
              ERROR_MSG("need at least x/y coordinates of a2\n");
              ERROR_RETURN(-1);
            }

            a2[1] /= BOHR; a2[2] /= BOHR; a2[3] /= BOHR;
            break;
          } /* a2 */

          case ('3'): /* a3 */
          {
            if (
            #ifdef CLEED_REAL_IS_DOUBLE
              sscanf(linebuffer + i_str + 3, " %lf %lf %lf",
                   &a3[1], &a3[2], &a3[3])
            #else
              sscanf(linebuffer + i_str + 3, " %f %f %f", &a3[1], &a3[2], &a3[3])
            #endif
              < 3) 
            {
              ERROR_MSG("could not read vector a3\n");
              ERROR_RETURN(-1);
            }
            a3[1] /= BOHR; a3[2] /= BOHR; a3[3] /= BOHR;
            break;
          } /* a3 */
        }
        break;
      } /* case 'a' */

      case ('b'): case ('B'): /* input of super structure unit cell parameters */
      {
        switch (*(linebuffer + i_str + 1))
        {
          case ('1'): /* b1 */
          {
            #ifdef CLEED_REAL_IS_DOUBLE
            if (sscanf(linebuffer + i_str + 3, " %lf %lf", (bulk_par->b) + 1,
                (bulk_par->b) + 3) < 2)
            #else
            if(sscanf(linebuffer + i_str + 3, " %f %f",
                    (bulk_par->b)+1, (bulk_par->b)+3 ) < 2)
            #endif
            {
              ERROR_MSG("need x/y coordinates of b1\n");
              ERROR_RETURN(-1);
            }
            bulk_par->b[1] /= BOHR;
            bulk_par->b[3] /= BOHR;
            break;
          } /* b1 */
          case ('2'): /* b2 */
          {
            #ifdef CLEED_REAL_IS_DOUBLE
            if (sscanf(linebuffer + i_str + 3, " %lf %lf", (bulk_par->b) + 2,
                (bulk_par->b) + 4) < 2)
            #else
            if(sscanf(linebuffer + i_str + 3, " %f %f",
                    (bulk_par->b)+2, (bulk_par->b)+4 ) < 2)
            #endif
            {
              ERROR_MSG("need x/y coordinates of b2\n");
              ERROR_RETURN(-1);
            }
            bulk_par->b[2] /= BOHR;
            bulk_par->b[4] /= BOHR;
            break;
          } /* b2 */
        }
        break;
      } /* case 'b' */

      case ('c'): case ('C'): /* input of comments to be stored */
      {
        char **tmp_comments =  
          (char **) realloc(bulk_par->comments, (i_com+2) * sizeof(char *));

        if (tmp_comments == NULL)
        {
          ERROR_MSG("could not allocate memory for comments\n");
          exit(ENOMEM);
        }
        else
        {
          bulk_par->comments = tmp_comments;
        }
        *(bulk_par->comments + i_com) = (char *) calloc(
            strlen(filename) + strlen(linebuffer) + 2 - i_str, sizeof(char));
        *(bulk_par->comments + i_com + 1) = NULL;

        CLEED_ALLOC_CHECK(bulk_par->comments[i_com]);
        sprintf(*(bulk_par->comments + i_com), "(%s): %s",
                filename, linebuffer + i_str + 2);

        i_com++;
        break;
      } /* case 'c' */

      case ('m'): case ('M'): /* input of super structure matrix */
      /* (use m_recip as temporary storage) */
      {
        switch (*(linebuffer + i_str + 1))
        {
          case ('1'): /* m11 and m21 superstructure matrix elements */
          {
            if (
            #ifdef CLEED_REAL_IS_DOUBLE
              sscanf(linebuffer + i_str + 3, " %lf %lf",
                     bulk_par->m_recip + 1, bulk_par->m_recip + 2)
            #else
              sscanf(linebuffer+i_str+3, " %f %f",
                    bulk_par->m_recip+1, bulk_par->m_recip+2)
            #endif
              < 2) 
            {
              ERROR_MSG("could not read first row of superstructure matrix\n");
              ERROR_RETURN(-1);
            }
            break;
          }
          case ('2'): /* m21 and m22 superstructure matrix elements */
          {
            if (
            #ifdef CLEED_REAL_IS_DOUBLE
              sscanf(linebuffer + i_str + 3, " %lf %lf",
                   bulk_par->m_recip + 3, bulk_par->m_recip + 4)
            #else
              sscanf(linebuffer + i_str + 3, " %f %f",
                bulk_par->m_recip+3, bulk_par->m_recip+4)
            #endif
              < 2)
            {
              ERROR_MSG("could not read second row of superstructure matrix\n");
              ERROR_RETURN(-1);
            }
            break;
          }
        }
        break;
      } /* case 'm' */

      case ('p'): case ('P'):
      /* input of atom positions and types for bulk through 'pb': */
      {
        /* go on if 2nd letter is different from 'b' */
        if ((*(linebuffer + i_str + 1) != 'b')
            && (*(linebuffer + i_str + 1) != 'B'))
        {
          /* "po" is known to function leed_read_overlayer,
           * all others cause warning */
          if ((*(linebuffer + i_str + 1) != 'o')
              && (*(linebuffer + i_str + 1) != 'O'))
          {
            WARNING_MSG("could not interpret line \n\t%s\t(in file \"%s\")\n",
                        linebuffer, filename);
          }
          break;
        }
        leed_atom *tmp_atoms_rd = (leed_atom *) realloc(atoms_rd,
                                    (i_atoms + 2) * sizeof(leed_atom));
        if (tmp_atoms_rd != NULL)
        {
          atoms_rd = tmp_atoms_rd;
        }
        else
        {
          ERROR_MSG("could not reallocate %u blocks of memory to 'atoms_rd'"
                    " at address %p (%s)\n", (i_atoms + 2) * sizeof(leed_atom),
                    (void *)atoms_rd, strerror(errno));
          exit(errno);
        }

        #ifdef CLEED_REAL_IS_DOUBLE
        iaux = sscanf(linebuffer + i_str + 3, " %s %lf %lf %lf %s %lf %lf %lf",
        #else
        iaux = sscanf(linebuffer + i_str + 3, " %s %f %f %f %s %f %f %f",
        #endif
            phaseinp, atoms_rd[i_atoms].pos + 1, atoms_rd[i_atoms].pos + 2,
            atoms_rd[i_atoms].pos + 3, whatnext, vaux + 1, vaux + 2, vaux + 3);

        for (i = 1; i <= 3; i++) atoms_rd[i_atoms].pos[i] /= BOHR;

        /* Input of phaseshifts and displacements due to thermal vibrations:
         *
         * Eventually, the vector vaux will contain
         *
         * vaux[0] = <dr^2> = <dx^2> + <dy^2> + <dz^2>;
         * vaux[1] = sqrt(<dx^2>),
         * vaux[2] = sqrt(<dy^2>),
         * vaux[3] = sqrt(<dz^2>),
         *
         * In the case of isotropic vibrations
         *
         * sqrt(<dx^2>) = sqrt(<dy^2>) = sqrt(<dz^2>) = sqrt(<dr^2>/3)
         */
        if (iaux <= 5)
        {
          for (i = 0; i <= 3; i++) vaux[i] = 0.;
        }
        else
        {
          /* Input of the isotropic root mean square displacement */
          if ((!strncmp(whatnext, "dr1", 3)) && (iaux >= 6))
          {
            atoms_rd[i_atoms].t_type = T_DIAG;

            vaux[0] = vaux[1] / BOHR;
            vaux[1] = vaux[2] = vaux[3] = vaux[0] / SQRT3;
            vaux[0] *= vaux[0];
          }

          /* Input of root mean square displacements for each direction:
           * use isotropic average for diagonal t matrix
           */
          else if ((!strncmp(whatnext, "dr3", 3)) && (iaux >= 8))
          {
            atoms_rd[i_atoms].t_type = T_DIAG;

            for (i = 1; i <= 3; i++) vaux[i] /= BOHR;
            vaux[0] = SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]);
          }

          /* Input of root mean square displacements for each direction:
           * no average for non-diagonal t matrix.
           */
          else if ((!strncmp(whatnext, "nd3", 3)) && (iaux >= 8))
          {
            atoms_rd[i_atoms].t_type = T_NOND;

            for (i = 1; i <= 3; i++) vaux[i] /= BOHR;
            vaux[0] = SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]);
          }

          /* Input of Debye temperature, atomic mass and temperature:
           * vaux[1] = Debye temperature
           * vaux[2] = atomic mass
           * vaux[3] = temperature (has to be specified only for the first atom;
           * if not specified, DEF_TEMP K is used)
           *
           * <r^2> is calculated in leed_inp_debye_temp
           */
          else if ((!strncmp(whatnext, "dmt", 3)) && (iaux >= 7))
          {
            atoms_rd[i_atoms].t_type = T_DIAG;

            if (iaux >= 8) bulk_par->temp = vaux[3];

            vaux[0] = leed_inp_debye_temp(vaux[1], vaux[2], bulk_par->temp);
            vaux[1] = vaux[2] = vaux[3] = cleed_real_sqrt(vaux[0]) / SQRT3;

            CONTROL_MSG(CONTROL_X, "temp = %.1f dr = %.3f\n",
                        bulk_par->temp, vaux[1] * SQRT3*BOHR );
          }
          else
          {
#if WARNING_LOG
            WARNING_MSG("Could not interpret input: %s", whatnext);
            for (i = 1; i <= (size_t)abs(iaux) - 5; i++)
            {
              fprintf(STDWAR, " %.3f", vaux[i]);
            }
            fprintf(STDWAR, "\n");
#endif

            for (i = 0; i <= 3; i++) vaux[i] = 0.;
          }
        }

        /* input of atomic phase shifts */
        atoms_rd[i_atoms].type = leed_inp_phase_nd(phaseinp, vaux,
                                    atoms_rd[i_atoms].t_type, p_phs_shifts);
        bulk_par->n_types = MAX(atoms_rd[i_atoms].type + 1, bulk_par->n_types);

        i_atoms++;
        break;
      } /* case 'p' */

      case ('v'): case ('V'):
      /* input of optical potentials */
      {
        switch (*(linebuffer + i_str + 1))
        {
          case ('i'): case ('I'):
          {
            if (
            #ifdef CLEED_REAL_IS_DOUBLE
              sscanf(linebuffer + i_str + 3, " %lf", &(bulk_par->vi))
            #else
              sscanf(linebuffer + i_str + 3, " %f", &(bulk_par->vi))
            #endif
                  < 1)
            {
              WARNING_MSG("could not read in 'vi' optical potential value "
                          "(using vi=%g)\n", bulk_par->vi);
            }
            (bulk_par->vi) /= HART;
            /* make sure, vi is > 0. */
            if (bulk_par->vi < 0.)
            {
              bulk_par->vi = -bulk_par->vi;

              WARNING_MSG("Vi must be positive, use the negative value "
                          "of input %.1f\n", bulk_par->vi * HART);
            }
            break;
          }

          case ('r'): case ('R'):
          {
            if (
            #ifdef CLEED_REAL_IS_DOUBLE
              sscanf(linebuffer + i_str + 3, " %lf", &(bulk_par->vr))
            #else
              sscanf(linebuffer + i_str + 3, " %f", &(bulk_par->vr))
            #endif
                  < 1)
            {
              WARNING_MSG("could not read in 'vr' optical potential value "
                "(using vr=%g)\n", bulk_par->vr);
            }
            (bulk_par->vr) /= HART;


            /* make sure, vr is < 0. */
            if (bulk_par->vr > 0.)
            {
              bulk_par->vr = -bulk_par->vr;

              WARNING_MSG("Vr must be negative, use the negative value "
                          "of input %.1f\n", bulk_par->vr * HART);
            }
            break;
          }
        }
        break;
      } /* case 'v' */

      case ('s'): case ('S'): /* symmetry */
      {
        switch( *(linebuffer+i_str+1 ))
        {
          case('r'): case('R'): /* rotational symmetry */
          {
            #ifdef CLEED_REAL_IS_DOUBLE
            if( sscanf(linebuffer+i_str+3, "%d %lf %lf",
                       &(bulk_par->n_rot), (bulk_par->rot_axis)+1,
                       (bulk_par->rot_axis)+2) < 3)
            #else
            if( sscanf(linebuffer+i_str+3,"%d %f %f",
                       &(bulk_par->n_rot), (bulk_par->rot_axis)+1,
                       (bulk_par->rot_axis)+2) < 3)
            #endif
            {
              ERROR_MSG("need x/y coordinates of rotaxis\n");
              ERROR_RETURN(-1);
            }
            bulk_par->rot_axis[1] /= BOHR;
            bulk_par->rot_axis[2] /= BOHR;
            break;
          } /* case r */

          case('m'): case('M'): /* mirror plane */
          {
            /* allocate */
            i_plane++;
            real *tmp_m_plane = (real *)
              realloc(bulk_par->m_plane, (2*i_plane + 1) * sizeof(real));
            if (tmp_m_plane != NULL)
            {
              bulk_par->m_plane = tmp_m_plane;
            }
            else
            {
              ERROR_MSG("failed to reallocate %u blocks of memory for "
                        "'bulk_par->m_plane' at address %p (%s)\n",
                        (2 * i_plane + 1) * sizeof(real), 
                        (void*)bulk_par->m_plane, strerror(errno));
              exit(errno);
            }

            real *tmp_alpha = (real *)realloc(bulk_par->alpha,
                                              (i_plane + 1) * sizeof(real));
            if (tmp_alpha != NULL)
            {
              bulk_par->alpha = tmp_alpha;
            }
            else 
            {
              ERROR_MSG("failed to reallocate %u blocks of memory for "
                        "'bulk_par->alpha' at address %p (%s)\n",
                        (i_plane + 1) * sizeof(real),
                        (void*)bulk_par->alpha, strerror(errno));
              exit(errno);
            }
            i = 2 *i_plane -1;
            j = 2 *i_plane -2;

            #ifdef CLEED_REAL_IS_DOUBLE
            if(sscanf(linebuffer+i_str+3, " %lf %lf ",
                      (bulk_par->m_plane)+j, (bulk_par->m_plane)+i) < 2)
            #else
            if(sscanf(linebuffer+i_str+i, " %f %f ",
                      (bulk_par->m_plane)+j, (bulk_par->m_plane)+i) < 2)
            #endif
            {
              ERROR_MSG("need x/y coordinates of mirrorplane\n");
              ERROR_RETURN(-1);
            }
            bulk_par->m_plane[j] /= BOHR;
            bulk_par->m_plane[i] /= BOHR;
            bulk_par->m_plane[i+1] = F_END_OF_LIST;
            bulk_par->n_mir = i_plane;

            break;

          } /* case m */

          default:
          {
            break;
          }

        }

        break;
      } /* case 's' */

      /* comments not to be stored and new line characters */
      case ('#'): case ('\n'): case ('\r'):
      /* identifiers used in leed_inp_leed_read_par() */
      case ('e'): case ('E'):
      case ('f'): case ('F'):
      case ('i'): case ('I'):
      case ('l'): case ('L'):
      {
        break;
      }

      default:
      /* default: print warning for unrecognized key words */
      {
        WARNING_MSG("could not interpret line \n\t%s\t(in file \"%s\")\n",
                    linebuffer, filename);
        break;
      }
    } /* switch linebuffer */
  } /* while: read input file */

  fclose(inp_stream);

  /* END OF INPUT
   * - Check the number of bulk atoms. Exit if zero
   * - Sort the mirror planes according to the angle
   */
  if (i_atoms < 1)
  {
    ERROR_MSG("could not find any bulk atoms (i_atoms = %d)\n", i_atoms);
    ERROR_RETURN(-1);
  }

  for(i=0; i < bulk_par->n_mir; i++)
  {
    for(j=i+1; j< bulk_par->n_mir; j++)
    {
      if(bulk_par->alpha[j] < bulk_par->alpha[i])
      {
        faux = bulk_par->alpha[j];
        bulk_par->alpha[j] = bulk_par->alpha[i];
        bulk_par->alpha[i] = faux;
        faux = bulk_par->m_plane[2* j];
        bulk_par->m_plane[2*j] = bulk_par->m_plane[2* i];
        bulk_par->m_plane[2*i] = faux;
        faux = bulk_par->m_plane[2* j+1];
        bulk_par->m_plane[2* j+1] = bulk_par->m_plane[2* i+1];
        bulk_par->m_plane[2* i+1] = faux;
      }
    }
    bulk_par->alpha[bulk_par->n_mir]= F_END_OF_LIST;
  }

  #ifdef CONTROL
  for(i = 0; i < bulk_par->n_mir; i++)
  {
    fprintf(STDOUT, "alpha%d = %f\n", i,bulk_par->alpha[i]/ DEG_TO_RAD);
    fprintf(STDOUT, "mirrorplane (%f %f)\n\n", bulk_par->m_plane[2*i]*BOHR,
                                               bulk_par->m_plane[2*i+1]*BOHR);
  }
  #endif

  /* Ensure that the basis vectors for the bulk unit cell specified
   * through a1, a2, a3 form a right-handed system with a3 pointing into
   * the crystal (i.e. a3[3] < 0).
   * If not: change the directions and the order of a1, a2, a3.
   *
   * Store a1 and a2 and their inverse values in bul_par.a / bul_par.a_1
   * 2x2 - matrices are stored as:  m[1]  m[2]
   *                                m[3]  m[4]
   */

  /* check if the z-components of a1 and a2 are 0. */
  if (!IS_EQUAL_REAL(a1[3], 0.0) || !IS_EQUAL_REAL(a2[3], 0.0))
  {
    ERROR_MSG("Vectors a1 and a2 are not parallel to the surface (xy plane)\n");
    ERROR_RETURN(-1);
  }

  /* check the direction of a3 */
  if (a3[3] > 0.0) a3[3] = -a3[3];

  /* Check the angle between a1 and a2 (1: must be >= pi/2, i.e. a1*a2 < 0).
   * if not: take a1 and -a2 as basic vectors and modify transformation matrix.
   */
  if ((a1[1] * a2[1] + a1[2] * a2[2]) > 0.0)
  {
    for (i = 0; i < 3; i++) a2[i] = -a2[i];
    bulk_par->m_trans[2] = -bulk_par->m_trans[2];
    bulk_par->m_trans[4] = -bulk_par->m_trans[4];
  }
  /* Check the angle between a1 and a2 (2: must be < pi, i.e. a1*a2 > 0)
   * if not: exchange a1 and a2 and modify transformation matrix.
   */
  if ((a1[1] * a2[2] - a1[2] * a2[1]) < 0.0)
  {
    faux = a2[1];
    a2[1] = a1[1];
    a1[1] = faux;
    faux = a2[2];
    a2[2] = a1[2];
    a1[2] = faux;

    faux = bulk_par->m_trans[1];
    bulk_par->m_trans[1] = bulk_par->m_trans[2];
    bulk_par->m_trans[2] = faux;

    faux = bulk_par->m_trans[3];
    bulk_par->m_trans[3] = bulk_par->m_trans[4];
    bulk_par->m_trans[4] = faux;
  }

  /* Store a1 and a2 and their inverse values in bul_par.a / bul_par.a_1 */
  bulk_par->a[1] = a1[1];
  bulk_par->a[2] = a2[1];
  bulk_par->a[3] = a1[2];
  bulk_par->a[4] = a2[2];

  faux = a1[1] * a2[2] - a1[2] * a2[1];
  bulk_par->area = cleed_real_fabs(faux);

  faux = 2. * PI / faux;
  bulk_par->a_1[1] = faux * a2[2];
  bulk_par->a_1[2] = -faux * a2[1];
  bulk_par->a_1[3] = -faux * a1[2];
  bulk_par->a_1[4] = faux * a1[1];

  /* Superstructure:
   * - The superstructure matrix in the input files is defined with respect to
   *   the unmodified unit cell vectors a:
   * b = m_super * a => b = m_super * m_trans * a_prg
   * (m_super was stored in m_recip)
   *
   * - Calculate inverse transposed of m_super = m_recip.
   */
  if ( cleed_real_hypot(bulk_par->b[1], bulk_par->b[3]) < GEO_TOLERANCE)
  /* There was no input of superstructure lattice vectors
   * => use matrix to calculate them.
   */
  {
    bulk_par->m_super[1] = bulk_par->m_recip[1] * bulk_par->m_trans[1] + 
                           bulk_par->m_recip[2] * bulk_par->m_trans[3];
    bulk_par->m_super[2] = bulk_par->m_recip[1] * bulk_par->m_trans[2] + 
                           bulk_par->m_recip[2] * bulk_par->m_trans[4];
    bulk_par->m_super[3] = bulk_par->m_recip[3] * bulk_par->m_trans[1] + 
                           bulk_par->m_recip[4] * bulk_par->m_trans[3];
    bulk_par->m_super[4] = bulk_par->m_recip[3] * bulk_par->m_trans[2] + 
                           bulk_par->m_recip[4] * bulk_par->m_trans[4];
    /*
     * Calculate superstructure lattice vectors b_t = m*a_t
     * b1x = b[1], b2x = b[2]
     * b1y = b[3], b2y = b[4]
     */

    bulk_par->b[1] = bulk_par->m_super[1] * bulk_par->a[1] + 
                     bulk_par->m_super[2] * bulk_par->a[2];
    bulk_par->b[3] = bulk_par->m_super[1] * bulk_par->a[3] + 
                     bulk_par->m_super[2] * bulk_par->a[4];
    bulk_par->b[2] = bulk_par->m_super[3] * bulk_par->a[1] + 
                     bulk_par->m_super[4] * bulk_par->a[2];
    bulk_par->b[4] = bulk_par->m_super[3] * bulk_par->a[3] + 
                     bulk_par->m_super[4] * bulk_par->a[4];
  } /* b was not defined */

  else
  /* Superstructure lattice vectors have been supplied
   * => use them to determine the superstructure matrix m_super.
   *
   * This overwrites even an existing superstructure matrix !!!
   */
  {
    faux = 0.5 / PI;
    bulk_par->m_super[1] = (bulk_par->a_1[1] * bulk_par->b[1] + 
                            bulk_par->a_1[2] * bulk_par->b[3]) * faux;
    bulk_par->m_super[3] = (bulk_par->a_1[1] * bulk_par->b[2] + 
                            bulk_par->a_1[2] * bulk_par->b[4]) * faux;
    bulk_par->m_super[2] = (bulk_par->a_1[3] * bulk_par->b[1] + 
                            bulk_par->a_1[4] * bulk_par->b[3]) * faux;
    bulk_par->m_super[4] = (bulk_par->a_1[3] * bulk_par->b[2] + 
                            bulk_par->a_1[4] * bulk_par->b[4]) * faux;
  } /* b was defined */

  /* Check the matrix for non-integer elements */
  if ((cleed_real_fabs(bulk_par->m_super[1] - cleed_real_nint(bulk_par->m_super[1]))
        > GEO_TOLERANCE) || 
      (cleed_real_fabs(bulk_par->m_super[2] - cleed_real_nint(bulk_par->m_super[2]))
        > GEO_TOLERANCE) || 
      (cleed_real_fabs(bulk_par->m_super[3] - cleed_real_nint(bulk_par->m_super[3]))
        > GEO_TOLERANCE) || 
      (cleed_real_fabs(bulk_par->m_super[4] - cleed_real_nint(bulk_par->m_super[4]))
          > GEO_TOLERANCE))
  {
    ERROR_MSG("superstructure is not commensurate \n");
    ERROR_RETURN(-1);
  }

  /* Check the angle between b1 and b2 (2: must be < pi, i.e. b1*b2 > 0)
   * if not: exchange b1 and b2 and modify transformation matrix.
   */
  if ((bulk_par->b[1] * bulk_par->b[4] - bulk_par->b[3] * bulk_par->b[2]) < 0.0)
  {
    faux = bulk_par->b[2];
    bulk_par->b[2] = bulk_par->b[1];
    bulk_par->b[1] = faux;
    
    faux = bulk_par->b[4];
    bulk_par->b[4] = bulk_par->b[3];
    bulk_par->b[3] = faux;

    faux = bulk_par->m_super[1];
    bulk_par->m_super[1] = bulk_par->m_super[3];
    bulk_par->m_super[3] = faux;

    faux = bulk_par->m_super[2];
    bulk_par->m_super[2] = bulk_par->m_super[4];
    bulk_par->m_super[4] = faux;
  }

  #ifdef CONTROL_X
  fprintf(STDCTR, "M_super: %5.2f %5.2f\n",
      bulk_par->m_super[1], bulk_par->m_super[2]);
  fprintf(STDCTR, "         %5.2f %5.2f\n",
      bulk_par->m_super[3], bulk_par->m_super[4]);
  fprintf(STDCTR, "b1: %5.2f %5.2f\n", bulk_par->b[1]*BOHR,bulk_par->b[3]*BOHR);
  fprintf(STDCTR, "b2: %5.2f %5.2f\n", bulk_par->b[2]*BOHR,bulk_par->b[4]*BOHR);
  #endif

  /*
   * area of superstructure unit cell in multiples of the (1x1) unit cell
   * = det(m_super)
   */
  faux = bulk_par->m_super[1] * bulk_par->m_super[4]
       - bulk_par->m_super[2] * bulk_par->m_super[3];
  bulk_par->rel_area_sup = cleed_real_fabs(faux);

  /* m_recip = m* = (m_super)t^-1 */
  faux = 1. / faux;

  bulk_par->m_recip[1] = +faux * bulk_par->m_super[4];
  bulk_par->m_recip[2] = -faux * bulk_par->m_super[3];
  bulk_par->m_recip[3] = -faux * bulk_par->m_super[2];
  bulk_par->m_recip[4] = +faux * bulk_par->m_super[1];

  #ifdef CONTROL_X
  fprintf(STDCTR, "M_recip: %5.2f %5.2f\n",
      bulk_par->m_recip[1], bulk_par->m_recip[2]);
  fprintf(STDCTR, "         %5.2f %5.2f\n",
      bulk_par->m_recip[3], bulk_par->m_recip[4]);
  fprintf(STDCTR, "area_sup: %5.2f\n", bulk_par->rel_area_sup);
  #endif

  /* Calculate reciprocal superstructure vectors: b_1 = 2PI * b^-1
   * b*1x = b_1[1], b*1y = b_1[2]
   * b*2x = b_1[3], b*2y = b_1[4]
   */
  faux = 2 * PI /
      (bulk_par->b[1] * bulk_par->b[4] - bulk_par->b[2] * bulk_par->b[3]);

  bulk_par->b_1[1] = +faux * bulk_par->b[4];
  bulk_par->b_1[2] = -faux * bulk_par->b[2];
  bulk_par->b_1[3] = -faux * bulk_par->b[3];
  bulk_par->b_1[4] = +faux * bulk_par->b[1];

  /* Subtract the coordinates of the rotation-axis (i.e. mirrorplanes)
   * from the atomic positions
   *
   * Move all atomic positions specified in atoms.pos into the 2-dim bulk unit
   * cell specified through a1 and a2:
   *
   * The vector x = A_1*pos must only have components between 0 and 1.
   * => subtract the integer surplus from pos.
   */
  for (i = 0; i < i_atoms; i++)
  {
    atoms_rd[i].pos[1] -= bulk_par->rot_axis[1];
    atoms_rd[i].pos[2] -= bulk_par->rot_axis[2];
  
    vaux[1] = (atoms_rd[i].pos[1] * bulk_par->a_1[1] +
               atoms_rd[i].pos[2] * bulk_par->a_1[2]) / (2. * PI);
    vaux[2] = (atoms_rd[i].pos[1] * bulk_par->a_1[3] +
               atoms_rd[i].pos[2] * bulk_par->a_1[4]) / (2. * PI);

    if (vaux[1] < 0.) iaux = (int) vaux[1] - 1;
    else iaux = (int) vaux[1];
    {
      atoms_rd[i].pos[1] -= iaux * bulk_par->a[1];
      atoms_rd[i].pos[2] -= iaux * bulk_par->a[3];
    }

    if (vaux[2] < 0.) iaux = (int) vaux[2] - 1;
    else iaux = (int) vaux[2];
    {
      atoms_rd[i].pos[1] -= iaux * bulk_par->a[2];
      atoms_rd[i].pos[2] -= iaux * bulk_par->a[4];
    }
  }

  /* Sort the atoms specified through atoms.pos according to their z
   * coordinates (largest z first).
   */
  for (i = 0; i < i_atoms; i++)
    for (j = i + 1; j < i_atoms; j++)
    {
      if (atoms_rd[i].pos[3] < atoms_rd[j].pos[3])
      {
        memcpy(&atom_aux, atoms_rd + j, sizeof(leed_atom));
        memcpy(atoms_rd + j, atoms_rd + i, sizeof(leed_atom));
        memcpy(atoms_rd + i, &atom_aux, sizeof(leed_atom));
      }
    }

  /* Check if the z-coordinates of all atoms are within the unit
   * cell. Those which are not, will be neglected.
   */
  for (i = 0; i < i_atoms; i++)
  {
    if (atoms_rd[i].pos[3] - atoms_rd[0].pos[3] < a3[3])
    {

      #ifdef WARNING_LOG
      fprintf(STDWAR, "* warning (leed_inp_read_bul): Some coordinates of "
                      "bulk atoms exceed the\n                       "
                      "bulk unit cell and will not be considered:\n");
      for (j = i; j < i_atoms; j++)
      {
        fprintf(STDWAR, " %s \t %7.4f  %7.4f  %7.4f\n", phaseinp,
            atoms_rd[j].pos[1] * BOHR, atoms_rd[j].pos[2] * BOHR,
            atoms_rd[j].pos[3] * BOHR);
      }
      #endif

      break;
    }
  }
  i_atoms = i;
  atoms_rd[i_atoms].type = I_END_OF_LIST;
  bulk_par->n_atoms = i_atoms;

  /* ATTENTION:
   * - Distribute the atoms to layers.
   * - Find the minimum interlayer distance.
   * - Test the symmetry, so if the test is negative the program will break
   */
  if(bulk_par->n_rot >= 1 || bulk_par->n_mir > 0)
  {
    /*!TODO:
    i_layer = leed_inp_bul_layer_sym(bulk_par, atoms_rd, a3);
    */
  }
  else
  {
    iaux = leed_inp_bul_layer(bulk_par, atoms_rd, a3);
    i_layer = (iaux >= 0) ? (size_t)iaux : 0;
  }
  
  free(atoms_rd);

  bulk_par->dmin = cleed_real_fabs(bulk_par->layers[0].vec_from_last[3]);
  for (i = 0; i < bulk_par->n_layers - 1 /* origin is not relevant */; i++)
  {
    bulk_par->dmin = MIN(bulk_par->dmin,
        cleed_real_fabs(bulk_par->layers[i].vec_to_next[3]));
  }

  /* Set the symmetry flags
   * first number shows the geometry: 
   *    (1=monocline)(2=rec)(3=hexagonal)(4=quadrat)
   * second number shows rotation axis (2-fold,3-fold,4-fold)
   * third number shows mirror planes (1,2,3,4)
   */
  vaux[1] = cleed_real_atan2(bulk_par->b[3],bulk_par->b[1]);
  vaux[2] = cleed_real_atan2(bulk_par->b[4],bulk_par->b[2]);
  faux = cleed_real_fabs(vaux[1] - vaux[2]);

  vaux[0] = bulk_par->b[1]*bulk_par->b[2] + bulk_par->b[3]*bulk_par->b[4];
  vaux[1] = cleed_real_hypot(bulk_par->b[1],bulk_par->b[3]);
  vaux[2] = cleed_real_hypot(bulk_par->b[2],bulk_par->b[4]);

  if(bulk_par->n_mir > 0 || bulk_par->n_rot > 1)
  {
    if( (cleed_real_fabs(faux - PI/3) < GEO_TOLERANCE ||
         cleed_real_fabs(faux - 2*PI/3) < GEO_TOLERANCE) &&
        (cleed_real_fabs(vaux[1] - vaux[2]) < GEO_TOLERANCE)                                      )
    {
      bulk_par->symmetry = 300 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
    }
    else if(cleed_real_fabs(vaux[0]) < GEO_TOLERANCE &&
            cleed_real_fabs(vaux[1] - vaux[2]) < GEO_TOLERANCE)
    {
      bulk_par->symmetry = 400 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
    }
    else if(cleed_real_fabs(vaux[0]) < GEO_TOLERANCE &&
            cleed_real_fabs(vaux[1] - vaux[2]) > GEO_TOLERANCE)
    {
      bulk_par->symmetry = 200 + 10 * bulk_par->n_mir +  bulk_par->n_rot;
    }
    else
    {
      bulk_par->symmetry = 100 + 10 * bulk_par->n_mir + bulk_par->n_rot;
    }
  }
  else
  {
    bulk_par->symmetry = NOSYM;
  }
  
  #ifdef CONTROL_X
  printf("***********************(leed_inp_read_bul)***********************\n");
  printf("potentials:\n");
  printf("\tvr: %7.4f eV  vi: %7.4f eV\n",
      (bulk_par->vr)*HART, (bulk_par->vi)*HART);

  printf("\nbulk unit cell:\n");
  printf("\ta1:  (%7.4f  %7.4f  %7.4f) A\n",
      a1[1]*BOHR, a1[2]*BOHR, a1[3]*BOHR);
  printf("\ta2:  (%7.4f  %7.4f  %7.4f) A\n",
      a2[1]*BOHR, a2[2]*BOHR, a2[3]*BOHR);
  printf("\ta3:  (%7.4f  %7.4f  %7.4f) A\n",
      a3[1]*BOHR, a3[2]*BOHR, a3[3]*BOHR);
  printf("\n     reciprocal lattice: \n");
  printf("\ta1*: (%7.4f  %7.4f) A^-1\n",
      bulk_par->a_1[1]/BOHR, bulk_par->a_1[2]/BOHR);
  printf("\ta2*: (%7.4f  %7.4f) A^-1\n",
      bulk_par->a_1[3]/BOHR, bulk_par->a_1[4]/BOHR);
  printf("\nsuperstructure unit cell:\n");
  printf("\t(%5.2f %5.2f)\tb1:  (%7.4f  %7.4f) A\n",
      bulk_par->m_super[1], bulk_par->m_super[2],
      bulk_par->b[1]*BOHR, bulk_par->b[3]*BOHR);
  printf("\t(%5.2f %5.2f)\tb2:  (%7.4f  %7.4f) A\n",
      bulk_par->m_super[3], bulk_par->m_super[4],
      bulk_par->b[2]*BOHR, bulk_par->b[4]*BOHR);

  printf("\n     reciprocal lattice: \n");
  printf("\t(%5.2f %5.2f)\tb1*: (%7.4f  %7.4f) A^-1\n",
      bulk_par->m_recip[1], bulk_par->m_recip[2],
      bulk_par->b_1[1]/BOHR, bulk_par->b_1[2]/BOHR);
  printf("\t(%5.2f %5.2f)\tb2*: (%7.4f  %7.4f) A^-1\n",
      bulk_par->m_recip[3], bulk_par->m_recip[4],
      bulk_par->b_1[3]/BOHR, bulk_par->b_1[4]/BOHR);

  printf("\npositions(bulk):\n");

  for(i=0; i < bulk_par->n_layers; i++)
  {
    printf("\n->\tvec: (%7.4f  %7.4f  %7.4f) A\n\n",
        bulk_par->layers[i].vec_from_last[1]*BOHR,
        bulk_par->layers[i].vec_from_last[2]*BOHR,
        bulk_par->layers[i].vec_from_last[3]*BOHR );

    if( bulk_par->layers[i].periodic == 0 ) printf("np:");
    else printf("p: ");

    for( j = 0; j < bulk_par->layers[i].n_atoms; j ++)
    {
      printf("\tpos: (%7.4f  %7.4f  %7.4f) A\tlayer: %d type: %d atom: %d\n",
          bulk_par->layers[i].atoms[j].pos[1]*BOHR,
          bulk_par->layers[i].atoms[j].pos[2]*BOHR,
          bulk_par->layers[i].atoms[j].pos[3]*BOHR,
          bulk_par->layers[i].atoms[j].layer,
          bulk_par->layers[i].atoms[j].type, j);
    }
  }

  printf("\n->\tvec: (%7.4f  %7.4f  %7.4f) A\n\n",
      bulk_par->layers[bulk_par->n_layers-1].vec_to_next[1]*BOHR,
      bulk_par->layers[bulk_par->n_layers-1].vec_to_next[2]*BOHR,
      bulk_par->layers[bulk_par->n_layers-1].vec_to_next[3]*BOHR );

  printf("M_trans:\n");
  printf("\t%7.4f  %7.4f\n", bulk_par->m_trans[1], bulk_par->m_trans[2]);
  printf("\t%7.4f  %7.4f\n", bulk_par->m_trans[3], bulk_par->m_trans[4]);
  printf("comments:\n");

  printf("Symmetry: ");
  switch(bulk_par->symmetry)
  {
     case NOSYM:
     {
       printf("none\n");
       break;
     }
     case MONO_2ROT:
     {
       printf("monocline structure with 2-fold rotational symmetry\n");
       break;
     }
     case MONO_1MIR:
     {
       printf("monocline structure with 1 mirror plane\n");
       break;
     }
     case REC_2ROT:
     {
       printf("rectangular structure with 2-fold rotational symmetry\n");
       break;
     }
     case REC_1MIR:
     {
       printf("rectangular structure with 1 mirror plane\n");
       break;
     }
     case REC_2MIR:
     {
       printf("rectangular structure with 2 mirror planes\n");
       break;
     }
     case HEX_3ROT:
     {
       printf("hexagonal structure with 3-fold rotational symmetry\n");
       break;
     }
     case HEX_1MIR:
     {
       printf("hexagonal structure with 1 mirror plane\n");
       break;
     }
     case HEX_3MIR:
     {
       printf("hexagonal structure with 3 mirror planes\n");
       break;
     }
     case SQ_2ROT:
     {
      printf("square structure with 2-fold rotational symmetry\n");
      break;
     }
     case SQ_4ROT:
     {
       printf("square structure with 4-fold rotational symmetry\n");
       break;
     }
     case SQ_1MIR:
     {
       printf("square structure with 1 mirror plane\n");
       break;
     }
     case SQ_2MIR:
     {
       printf("square structure with 2 mirror planes\n");
       break;
     }
     case SQ_4MIR:
     {
       printf("square structure with 4 mirror planes\n");
       break;
     }
     default:
     {
       ERROR_MSG("symmetry invalid\n");
       ERROR_RETURN(-1);
       break;
     }

  } /* switch */
  
  for( i=0; i<i_com; i++)
  {
    printf("\t%s", *(bulk_par->comments + i));
  }

  fprintf(STDCTR, "phase shifts:\n");
  fprintf(STDCTR, "\t%d different sets of phase shifts used:\n",
      bulk_par->n_types);
  for(i_c = 0; i_c < bulk_par->n_types; i_c ++)
  {
    fprintf(STDCTR,"\t(%d) %s (%d energies, lmax = %d)\tV<dr^2>_T = %.3f A^2\n",
            i_c,
            p_phs_shifts[i_c]->input_file,
            p_phs_shifts[i_c]->n_eng,
            p_phs_shifts[i_c]->lmax,
            cleed_real_sqrt(p_phs_shifts[i_c]->dr[0]) * BOHR);
  }
  printf("***********************(leed_inp_read_bul)***********************\n");
  #endif

  /* write the structures phs_shifts and bulk_par back. */
  *p_bulk_par = bulk_par;

  return(0);

  i_layer = i_layer * 1; /* force use of i_layer */
}
