/*********************************************************************
 *                    SRRDINP.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/21.08.95 - Creation
 *   GH/27.09.95 - Change switch/case into if - else if chains,
 *                 Read R factor parameters
 *   GH/28.09.95 - Read a1/2, b1/2, m1/2 either from inp_file or from
 *                <sr_project>.bul.
 *   GH/29.12.95 - read reference list directly without specifying any symmetry.
 *   WB/29.09.98 - change BUFSZ and some CONTROLS ...
 *               - find a fault: buf size STRSZ ...
 *               - set for allocation sr_atoms : n_atoms+2
 *   GH/29.09.00 - calculate dr2 for dmt input in function leed_inp_debye_temp
 *   GH/31.03.03 - Added theta_0 and phi_0 for the angle search,
 *                 use i_par_theta/phi
 ***********************************************************************/

/*!
 * \file
 * \author Georg Held <g.held@reading.ac.uk>
 * \brief Read input atom positions and convert them to parameter strings.
 *
 */

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#include "csearch.h"

static const double GEOM_TOLERANCE = 0.0001;
enum { BUFSZ = 1024 };                   /*!< max. length of input line */

extern search_atom *sr_atoms;
extern search *sr_search;
extern char *sr_project;

/* input buffers */
char buf[BUFSZ]; 
char atom_name[STRSZ];
char whatnext[STRSZ];

/*!
 * Read input for search from input file (and *.bul)
 *
 * \param[in] inp_file input file containing start geometry and control
 * parameters for the \c search program.
 *
 * Unlike in the CLEED program this input routine does not convert the
 * parameters into atomic units (BOHR and HARTREE).
 *
 * When reading the input file the currently recognized identifiers are:
 * \c 'a1:','a2:' bulk unit cell parameters.
 * \c 'b1:','b2:' overlayer unit cell parameters.
 * \c 'm1:','m2:' superstr. matrix.
 * \c 'it:','ip:' angles of incidence.
 *
 * \warning The above parameters \c a1, \c a2, \c b1, \c b2, \c m1, \c m2,
 * and \c ip, \c it will be searched for in \p inp_file as well as in
 * \c <sr_project>.bul. If they are found in both files \p inp_file
 * is given priority.
 *
 * \c 'po:' position and type of bulk atoms.
 *
 * \c 'rm:' minimum radius.   ( name r_m : type name, min. radius)
 * \c 'zr:' range of z values.   ( z_min z_max : min./max. z value)
 *
 * \c 'sr:' rotation symmetry ( n x_a y_a : degree, x/y coord of axis).
 * \c 'sm:' mirror symmetry
 *        ( x_d y_d x_p y_p : direction, x/y coord of point in mp.).
 * \c 'sz:' 1 = optimize only z coordinates; 0 = optimize all coordinates.
 * \c 'sa:' 1 = angle search is on; 0 = angle search is off.
 *
 * \c 'spn:' number of parameters (n_par)
 * \c 'spp:' parameter reference list (n_atom x/y/z par_ref_list)
 *
 *         An atomic coordinate x is calculated as x_0 plus the sum over
 *         x_par[i] * par[i], where x_par is the parameter reference list
 *         for this particular atom and par the list of (all) parameters.
 *
 * \c 'rft:' R factor type (input for R factor program)
 * \c 'rfr:' shift range (input for R factor program)
 *
 * Each entry of atoms will contain:
 *  - type: reference to list types.
 *  - x,y,z: atom positions
 *  - dr: vibrational amplitude ( \f$ <dr^2> \f$ )
 *
 * \note #sr_atoms and #sr_search will be allocated in this function.
 * \return number of parameters.
 * \retval #SR_FAILURE if failed and \c EXIT_ON_ERROR is not defined.
 *
 * \note the exit code #search_error will be be given if the function
 * fails and \c EXIT_ON_ERROR is defined.
 *
 */
int sr_rdinp(const char *inp_file)
{

  FILE *inp_stream;

  int iaux;                     /* counter, dummy  variables */
  int i_c, i_str, i_dummy;

  size_t i_atoms, n_atoms;
  size_t i_types, n_types;
  size_t i_par, n_par;

  real temp;

  real a1[4], a2[4];
  real m_super[5];

  real faux;                   /* dummy variable */
  real vaux[4];                /* dummy vector */
  real *paux;                  /* dummy pointer */

  struct type_str
  {
    char name[STRSZ];          /* atomic symbol */
    real r_min;                /* minimum radius (used in geometry assessment) */
  };

  struct type_str *types;      /* this vector of structure type_str is
                                  used to read and treat the input atomic
                                  properties */

  char fmt_buffer[STRSZ];

/********************************************************************
 * First: find project name
 ********************************************************************/

  CONTROL_MSG(CONTROL_X, "start of function\n");

  for(i_str = 0; ( inp_file[i_str] != '\0' ) && ( inp_file[i_str] != '.' )  &&
                 ( i_str < STRSZ ); i_str ++)
  {
    sr_project[i_str] = inp_file[i_str];
  }
  sr_project[i_str] = '\0';

  CONTROL_MSG(CONTROL, "project name = \"%s\"\n", sr_project);

  /*
   * Preset parameters
   * - allocate atoms (1 unit) and type (2 units)
   */

  sr_atoms = (search_atom *) malloc(sizeof(search_atom));
  if( sr_atoms == NULL)
  {
    ERROR_MSG("allocation error (sr_atoms)\n");
    ERROR_EXIT_RETURN(SR_ALLOC_ERROR, -1);
  }
  n_atoms = 0;

  types = (struct type_str *) malloc( sizeof(struct type_str) );
  if( types == NULL)
  {
    ERROR_MSG("allocation error (types)\n");
    ERROR_EXIT_RETURN(SR_ALLOC_ERROR, -1);
  }
  types->r_min = F_END_OF_LIST;
  n_types = 0;

  sr_search = (search *) malloc(sizeof(search));
  if( sr_search == NULL)
  {
    ERROR_MSG("allocation error (sr_search)\n");
    ERROR_EXIT_RETURN(SR_ALLOC_ERROR, -1);
  }

  for(iaux = 0; iaux <= 5; iaux ++) sr_search->b_lat[iaux] = 0.;

  sr_search->sr_angle = 0; /* added for the angle search */
  sr_search->z_only = 0;
  sr_search->rot_deg = 1;
  sr_search->rot_axis[1] = sr_search->rot_axis[2] = 0.;

  sr_search->mir_point[1] = sr_search->mir_point[2] = 0.;
  sr_search->mir_dir[1] = sr_search->mir_dir[2] = 0.;

  strncpy(sr_search->rfac_type, RFAC_TYP, 16);
  sr_search->rfac_range = RFAC_SHIFT_RANGE;

  temp = DEF_TEMP;
  for(iaux = 0; iaux <= 5; iaux ++) m_super[iaux] = 0.;
  for(iaux = 0; iaux <= 4; iaux ++) a1[iaux] = a2[iaux] = 0.;

  /* n_par != 0 indicates manual entry of parameter reference list */
  n_par = 0;

  /*
   * START INPUT (*.bul)
   * First try to read a1/a2, b1/b2, m1/m2 from <sr_project>.bul
   */
  sprintf(buf, "%s.bul", sr_project);

  CONTROL_MSG(CONTROL_X, "open file \"%s\" \n", buf);

  if( (inp_stream = fopen(buf, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", buf);
    ERROR_EXIT_RETURN(SR_FILE_IO_ERROR, -1);
  }

  CONTROL_MSG(CONTROL_X, "Search file \"%s\" for lattice parameters\n", buf);

  while ( fgets(buf, BUFSZ, inp_stream) != NULL)
  {
    CONTROL_MSG(CONTROL_X, "%s", buf);

    /* find first non blank character */
    for( i_str = 0;  *(buf+i_str) == ' '; i_str ++) { ; }

    /*
     * a1 a2:  (*.bul)
     * input of bulk unit cell parameters
     */
    if( !strncasecmp(buf+i_str, "a1:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf %%%sf",
              CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer, a1+1, a1+2, a1+3) < 2)
      {
        ERROR_MSG("need at least x/y coordinates of a1\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }
    }  /* a1 */
   
    else if( !strncasecmp(buf+i_str, "a2:" ,3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf %%%sf",
                 CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer, a2+1, a2+2, a2+3) < 2)
      {
        ERROR_MSG("need at least x/y coordinates of a2\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }

    }  /* a2 */

    /*
     * b1 b2:  (*.bul)
     * input of super structure unit cell parameters
     * */
    else if( !strncasecmp(buf+i_str,"b1:",3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer,
         (sr_search->b_lat)+1, (sr_search->b_lat)+3 ) < 2)
      {
        ERROR_MSG("need x/y coordinates of b1\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }

    } /* b1 */

    else if( !strncasecmp(buf+i_str, "b2:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer,
           (sr_search->b_lat)+2, (sr_search->b_lat)+4 ) < 2)
      {
        ERROR_MSG("need x/y coordinates of b2\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }

    } /* b2 */

    /*
     * m1 m2:  (*.bul)
     * input of super structure matrix
     * (use m_super as temporary storage)
     */
    else if( !strncasecmp(buf+i_str, "m1:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      sscanf(buf+i_str+3, fmt_buffer, m_super+1, m_super+2);
    } /* m1 */

    else if( !strncasecmp(buf+i_str, "m2:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      sscanf(buf+i_str+3, fmt_buffer, m_super+3, m_super+4);
    } /* m2 */

    /* Added for the angle search (SRP, GH/02.04.03) */

    /*
     * ip:  (*.bul)
     * read phi_0 for the angle search
     */
    else if( !strncasecmp(buf+i_str, "ip:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer, &(sr_search->phi_0) );
    } /* case ip */

    /*
     * it:  (*.bul)
     * read theta_0 for the angle search
     */
    else if( !strncasecmp(buf+i_str, "it:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer, &(sr_search->theta_0) );
    } /* case it */

  } /* while: read *.bul file for lattice parameters */

  fprintf(STDOUT, "\n");
  fclose(inp_stream);

  /*
   * CONTINUE INPUT (*.inp file)
   * Open and Read input file
   */
  if( (inp_stream = fopen(inp_file, "r")) == NULL)
  {
    ERROR_MSG("could not open file \"%s\"\n", inp_file);
    ERROR_EXIT_RETURN(SR_FILE_IO_ERROR, -1);
  }

  CONTROL_MSG(CONTROL, "Search file \"%s\" for other parameters\n", inp_file);

  while ( fgets(buf, BUFSZ, inp_stream) != NULL)
  {
    CONTROL_MSG(CONTROL_X, "%s", buf);

    /* find first non blank character */
    for( i_str = 0;  *(buf+i_str) == ' '; i_str ++) { ; }

    /*
     * a1 a2:
     *   input of bulk unit cell parameters
     */
    if( !strncasecmp(buf+i_str,"a1:",3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf %%%sf",
              CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer, a1+1, a1+2, a1+3) < 2)
      {
        ERROR_MSG("need at least x/y coordinates of a1\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }
    }  /* a1 */
   
    else if( !strncasecmp(buf+i_str,"a2:",3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf %%%sf",
                    CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer, a2+1, a2+2, a2+3) < 2)
      {
        ERROR_MSG("need at least x/y coordinates of a2\n");
        ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
      }
    }  /* a2 */

    /*
     * b1 b2:
     *   input of super structure unit cell parameters
     */
    else if( !strncasecmp(buf+i_str, "b1:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer,
           (sr_search->b_lat)+1, (sr_search->b_lat)+3 ) < 2)
      {
        ERROR_MSG("need x/y coordinates of b1\n");
        ERROR_RETURN(-1);
      }
    } /* b1 */

    else if( !strncasecmp(buf+i_str, "b2:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      if( sscanf(buf+i_str+3, fmt_buffer,
           (sr_search->b_lat)+2, (sr_search->b_lat)+4 ) < 2)
      {
        ERROR_MSG("need x/y coordinates of b2\n");
        ERROR_RETURN(-1);
      }
    } /* b2 */

    /*
     * m1 m2:
     *   input of super structure matrix (use m_super as temporary storage)
     */
    else if( !strncasecmp(buf+i_str, "m1:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      sscanf(buf+i_str+3, fmt_buffer, m_super+1, m_super+2);
    } /* m1 */

    else if( !strncasecmp(buf+i_str, "m2:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      sscanf(buf+i_str+3, fmt_buffer, m_super+3, m_super+4);
    } /* m2 */

    /*
     * po:
     *   input of atom positions and types for variable atoms through 'po':
     */
    else if( !strncasecmp(buf+i_str, "po:", 3) )
    {
      sr_atoms = (search_atom *) realloc(
                 sr_atoms, (n_atoms+2) * sizeof(search_atom) );

      /* preset xyz_par with NULL ref and n_ref with I_END_OF_LIST */
      (sr_atoms+n_atoms)->x_par = NULL;
      (sr_atoms+n_atoms)->y_par = NULL;
      (sr_atoms+n_atoms)->z_par = NULL;

      (sr_atoms+n_atoms)->ref = U_END_OF_LIST;
      (sr_atoms+n_atoms)->n_ref = U_END_OF_LIST;

      sprintf(fmt_buffer, " %%S %%%sf %%%sf %%%sf %%%sf %%%sf %%%sf",
              CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT,
              CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer,
                    (sr_atoms+n_atoms)->name,
                    &((sr_atoms+n_atoms)->x),
                    &((sr_atoms+n_atoms)->y),
                    &((sr_atoms+n_atoms)->z),
                    whatnext,
                    vaux+1, vaux+2, vaux+3);
      /*
       * Input of phaseshifts and displacements due to thermal vibrations:
       * Eventually, the vector vaux will contain
       *   vaux[0] = <dr^2> = <dx^2> + <dy^2> + <dz^2>;
       *   vaux[1] = sqrt(<dx^2>),
       *   vaux[2] = sqrt(<dy^2>),
       *   vaux[3] = sqrt(<dz^2>),
       *
       * In the case of isotropic vibrations
       *   sqrt(<dx^2>) = sqrt(<dy^2>) = sqrt(<dz^2>) = sqrt(<dr^2>/3)
       */
      if (iaux <= 5) (sr_atoms+n_atoms)->dr = 0.;
      else
      {
        /* input of the isotropic root mean square displacement */
        if( ( !strncmp(whatnext, "dr1", 3) ) && (iaux >= 6) )
        {
          (sr_atoms+n_atoms)->dr = vaux[1];
        }

        /* input of root mean square displacements for each direction */
        else if( ( !strncmp(whatnext, "dr3", 3) ) && (iaux >= 8) )
        {
          (sr_atoms+n_atoms)->dr =
              cleed_real_sqrt( SQUARE(vaux[1]) + SQUARE(vaux[2]) + SQUARE(vaux[3]) );
        }

        /* Input of Debye temperature, atomic mass and temperature:
         *   vaux[1] = Debye temperature
         *   vaux[2] = atomic mass
         *   vaux[3] = temperature (has to be specified only for the first atom;
         *             if not specified, 300 K is used)
         *
         *   <r^2> (in atomic units ) is calculated in leed_inp_debye_temp
         */
        else if( ( !strncmp(whatnext, "dmt", 3) ) && (iaux >= 7) )
        {
          if(iaux >= 8) temp = vaux[3];
          else if( IS_EQUAL_REAL(temp, DEF_TEMP))
          {
            WARNING_MSG("temperature input is missing %.1f K used instead.\n",
                     temp);
          }

          vaux[0] = leed_inp_debye_temp(vaux[1], vaux[2], temp);
          (sr_atoms+n_atoms)->dr = cleed_real_sqrt(vaux[0]) * BOHR;
         
          CONTROL_MSG(CONTROL, "temp = %.1f dr = %.3f\n",
                      temp, (sr_atoms+n_atoms)->dr);
        }
        else
        {
#         if WARNING_LOG
          WARNING_MSG("Could not interpret input: %s", whatnext);
          for(i_str=1; i_str<=iaux-5; i_str++)
          {
            fprintf(STDWAR, " %.3f", vaux[i_str]);
          }
          fprintf(STDWAR, "\n");
#         endif
          (sr_atoms+n_atoms)->dr = 0.;
        }
      }

      /* Check type list for current atom name  */
      for(i_types = 0; i_types < n_types; i_types ++)
      {
        if(strcmp( (types+i_types)->name, sr_atoms[n_atoms].name ) == 0)
        {
          (sr_atoms+n_atoms)->type = (int)i_types;
          break;
        }
      }

      /* Include current atom name in type list if it was not in the old list */
      if(i_types == n_types)
      {
        types = ( struct type_str *) realloc(
               types, (n_types+1) * sizeof(struct type_str) );

        strcpy( (types+i_types)->name, sr_atoms[n_atoms].name);
        (types+i_types)->r_min = F_END_OF_LIST;

        sr_atoms[n_atoms].type = (int)i_types;
        n_types ++;
      }

      fprintf(STDOUT, "po(%d)\n", n_atoms);
      n_atoms ++;

    } /* po */

    /* Added for the angle search (SRP/25.03.03) */

    /*
     * ip:
     *   read phi_0 for the angle search
     *   (Added on 25th March -03 by SRP)
     */
    else if( !strncasecmp(buf+i_str, "ip:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer, &(sr_search->phi_0) );
    } /* case ip */

    /*
     * it:
     *   read theta_0 for the angle search
     *   (Added on 25th March -03 by SRP)
     */
    else if( !strncasecmp(buf+i_str, "it:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer, &(sr_search->theta_0) );
    } /* case it */

    /* End of the bit for the angle search added by SRP on 25th March -03. */

    /*
     * rfr:
     *   input of shift range for R factor program
     */
    else if( !strncasecmp(buf+i_str, "rfr:", 4) )
    {
      sprintf(fmt_buffer, "%%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+4, fmt_buffer, &(sr_search->rfac_range) );
    }

    /*
     * rft:
     *   input of R factor type for R factor program
     */
    else if( !strncasecmp(buf+i_str, "rft:", 4) )
    {
      /* read into 'whatnext' to make sure, there is no writing over
       * the array boundaries (16)
       */
      iaux = sscanf(buf+i_str+4, "%s", whatnext );
      strncpy(sr_search->rfac_type, whatnext, 16);
    }

    /*
     * rm:
     *   input of min. radius for certain type.
     */
    else if( !strncasecmp(buf+i_str, "rm:", 3) )
    {
      sprintf(fmt_buffer, " %%s %%%sf", CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3 , fmt_buffer, atom_name, &faux);

      /* Write the current r_min parameter to existing
       * structure element in types */
      for(i_types = 0; i_types < n_types; i_types ++)
      {
        if(strcmp( (types+i_types)->name, atom_name ) == 0)
        {
          (types+i_types)->r_min = faux;
          break;
        }
      }

      /* Include current type name in types if it was not in the old list */
      if(i_types == n_types)
      {
        types = ( struct type_str *) realloc(
                 types, (n_types+1) * sizeof(struct type_str) );
        strcpy( (types+i_types)->name, atom_name);
        (types+i_types)->r_min = faux;
        n_types ++;
      }

    } /* rm */

    /*
     * sm:
     *   input of mirror plane
     */
    else if( !strncasecmp(buf+i_str, "sm:", 3) )
    {
      if(n_par != 0)
      {
        WARNING_MSG("ignore ''sm''-entry with n_par != 0 (spn)\n");
      }
      else
      {
        sprintf(fmt_buffer, " %%%sf %%%sf %%%sf %%%sf",
                CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT, CLEED_REAL_FMT);
        iaux = sscanf(buf+i_str+3, fmt_buffer,
                (sr_search->mir_point)+1, (sr_search->mir_point)+2,
                (sr_search->mir_dir)+1,   (sr_search->mir_dir)+2   );
      }

    } /* sm */

    /*
     * sr:
     *   input of rotation axis
     */
    else if( !strncasecmp(buf+i_str, "sr:", 3) )
    {
      if(n_par != 0)
      {
        WARNING_MSG("ignore ''sr''-entry with n_par != 0 (spn)\n");
      }
      else
      {
        sprintf(fmt_buffer, " %%d %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
        iaux = sscanf(buf+i_str+3, fmt_buffer,
                      &(sr_search->rot_deg),
                      (sr_search->rot_axis)+1,
                      (sr_search->rot_axis)+2);
      }

    } /* sr */

    /*
     * sz:
     *   z search only, xyz search
     *   set sr_search->z_only to 1 if no argument,
     *   to the argument value otherwise
     */
    else if( !strncasecmp(buf+i_str, "sz:", 3) )
    {
      if(n_par != 0)
      {
        WARNING_MSG("ignore ''sz''-entry with n_par != 0 (spn)\n");
      }
      else
      {
        iaux = sscanf(buf+i_str+3, " %d", &i_dummy);
        sr_search->z_only = (i_dummy != 0 && iaux > 0) ? (true) : (false);
        if (sr_search->z_only)
        {
          CONTROL_MSG(CONTROL, "Only variation of vertical parameters\n");
        }
      }

    } /* sz */

    /*
     * sa:
     *   1 = angle search on,
     *   0 = angle search is off (default)
     */
    else if( !strncasecmp(buf+i_str, "sa:", 3) )
    {
      iaux = sscanf(buf+i_str+3, " %d", &i_dummy);
      sr_search->sr_angle = (i_dummy != 0 && iaux > 0) ? (true) : (false);
      if (sr_search->sr_angle)
      {
        CONTROL_MSG(CONTROL, "Angle search is on\n");
      }

    } /* sa */

    /*
     * spn:
     *   number of parameters
     *   - set z_only to zero
     */
    else if( !strncasecmp(buf+i_str, "spn:", 4) )
    {
      sscanf(buf+i_str+4, " %d", &n_par );
      sr_search->n_par = n_par;
      sr_search->z_only = 0;

      CONTROL_MSG(CONTROL_X, "%d parameters specified\n", sr_search->n_par);
      fprintf(STDOUT, "spn(%d) ", sr_search->n_par);

    } /* spn */

    /*
     * spp:
     *   reference list for parameters
     */
    else if( !strncasecmp(buf+i_str, "spp:", 4) )
    {
      /* check if number of parameters is defined */
      if (n_par == 0)
      {
        WARNING_MSG("no manual entry of reference list with n_par = 0 (spn)\n");
      }
      else
      {
        paux = (real *)calloc( (n_par +1), sizeof(real) );

        /* read first item: number of atom ('-' means last atom) */
        i_str += 4;
        while(buf[i_str] == ' ') i_str++;
        if( (buf[i_str] == '-') && (n_atoms > 0) ) i_atoms = n_atoms - 1;
        else if( (buf[i_str] >= '0') && (buf[i_str] <= '9') )
        {
          sscanf(buf+i_str, "%d", &i_atoms );
        }
        else
        {
          ERROR_MSG("'%s'", buf);
          ERROR_MSG("number or '-' expected as first parameter in reference list\n");
          exit(SR_INVALID_INPUT_FILE);
        }

        /* read and store 2nd item: xyz */
        while(buf[i_str] != ' ') i_str ++;
        while(buf[i_str] == ' ') i_str ++;
        if( (buf[i_str] == 'x') || (buf[i_str] == 'y') || (buf[i_str] == 'z') )
        {
          i_c =  buf[i_str];
        }
        else
        {
          ERROR_MSG("'%s'", buf);
          ERROR_MSG("x, y, or z expected as second parameter in reference list\n");
          exit(SR_INVALID_INPUT_FILE);
        }

        /* read parameter reference list */
        while(buf[i_str] != ' ') i_str ++;
        while(buf[i_str] == ' ') i_str ++;

        iaux = (int)strlen(buf);
        sprintf(fmt_buffer, "%%%se", CLEED_REAL_FMT);
        for( i_par = 1; (i_par <= n_par) && (i_str < iaux) ; i_par ++)
        {

          sscanf(buf+i_str, fmt_buffer, paux + i_par);

          while( (i_str < iaux) && (buf[i_str] != ' ') ) i_str ++;
          while( (i_str < iaux) && (buf[i_str] == ' ') ) i_str ++;
        }

        /* copy paux into respective parameter list */
        if(i_c == 'x')
        {
          if((sr_atoms+i_atoms)->x_par != NULL)
          {
            free((sr_atoms+i_atoms)->x_par);
          }
          (sr_atoms+i_atoms)->x_par = paux;

          fprintf(STDOUT, "spp(%dx) ", i_atoms);
        }

        if(i_c == 'y')
        {
          if((sr_atoms+i_atoms)->y_par != NULL)
          {
            free((sr_atoms+i_atoms)->y_par);
          }
         (sr_atoms+i_atoms)->y_par = paux;

         fprintf(STDOUT, "spp(%dy) ", i_atoms);
        }

        if(i_c == 'z')
        {
          if((sr_atoms+i_atoms)->z_par != NULL)
          {
            free((sr_atoms+i_atoms)->z_par);
          }
          (sr_atoms+i_atoms)->z_par = paux;

          fprintf(STDOUT,"spp(%dz) ", i_atoms);
        }

      } /* else n_par != 0 */

    } /* spp */

    /*
     * zr:
     *   Input of min. and max z value
     */
    else if( !strncasecmp(buf+i_str, "zr:", 3) )
    {
      sprintf(fmt_buffer, " %%%sf %%%sf", CLEED_REAL_FMT, CLEED_REAL_FMT);
      iaux = sscanf(buf+i_str+3, fmt_buffer,
            &(sr_search->z_min), &(sr_search->z_max) );
    } /* case zr */

    /*
     * The following characters indicate Comments not to be stored and
     * new line characters.
     * print warning for keywords that are not recognized
     */
    else if( (*(buf+i_str) != '#' ) &&
             (*(buf+i_str) != '\r') &&
             (*(buf+i_str) != '\n')  )
    {
      WARNING_MSG("could not interpret line \n\t%s\t(in file \"%s\")\n",
              buf, inp_file);
    }

  }   /* while: read input file */

  fprintf(STDOUT, "\n");

  /* END OF INPUT
   *   Close input file.
   *   Terminate lists.
   **/
  fclose(inp_stream);

  (types+n_types)->name[0] = '\0';
  (sr_atoms+n_atoms)->type   = I_END_OF_LIST;

  /* Start processing input data if there were any atoms (n_atoms > 0) */

  #ifdef CONTROL
  fprintf(STDCTR, "(sr_rdinp): start processing input data:\n");
  fprintf(STDCTR, "\tNo of atoms = %d\n", n_atoms);
  fprintf(STDCTR, "\tz_min = %.3f, z_max = %.3f\n",
          sr_search->z_min, sr_search->z_max);
  fprintf(STDCTR, "\trfac_type = \"%s\", rfac_range = %.2f\n",
          sr_search->rfac_type, sr_search->rfac_range);
  #endif

  if(n_atoms == 0)
  {
    ERROR_MSG("no atoms found\n");
    ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
  }

  /* Copy r_min for all atoms from type list */
  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    i_types = (size_t)(sr_atoms+i_atoms)->type;
    if( IS_EQUAL_REAL((types+i_types)->r_min, F_END_OF_LIST) )
    {
      WARNING_MSG("no min. r available for atom type %s "
                  "(zero will be used instead)\n", (types+i_types)->name);
      (types+i_types)->r_min = 0.;
    }
    else
    {
      (sr_atoms+i_atoms)->r_min = (types+i_types)->r_min;
    }
   
  } /* for i_atoms */

  /* The input of the superstructure unit cell parameters can either be
   * done through 'a1:'/'a2:' and the superstructuer matrix 'm1:'/'m2:' or
   * directly through 'b1:'/'b2:'. (The latter overwrites the first).
   *
   * Calculate b_lat if the first input type was used.
   * Determine max. x/y search range from b_lat.
   */
  if( cleed_real_hypot(sr_search->b_lat[1], sr_search->b_lat[3]) < GEOM_TOLERANCE)
  {
    /* There was no input of superstructure lattice vectors
     * => use matrix to calculate them. b_t = m*a_t */
    if( (cleed_real_hypot(a1[1], a1[2]) < GEOM_TOLERANCE) ||
        (cleed_real_hypot(a2[1], a2[2]) < GEOM_TOLERANCE)  )
    {
      ERROR_MSG("Cannot determine lattice parameters\n");
      ERROR_EXIT_RETURN(SR_INVALID_INPUT_FILE, -1);
    }
  
    sr_search->b_lat[1] = m_super[1] * a1[1] + m_super[2] * a2[1];
    sr_search->b_lat[3] = m_super[1] * a1[2] + m_super[2] * a2[2];
    sr_search->b_lat[2] = m_super[3] * a1[1] + m_super[4] * a2[1];
    sr_search->b_lat[4] = m_super[3] * a1[2] + m_super[4] * a2[2];

  }  /* no b defined */

  CONTROL_MSG(CONTROL, "b1 = (%6.3f,%6.3f)\nb2 = (%6.3f,%6.3f)\n",
                       sr_search->b_lat[1], sr_search->b_lat[3],
                       sr_search->b_lat[2], sr_search->b_lat[4]);

  sr_search->x_max = MAX(fabs(sr_search->b_lat[1]), fabs(sr_search->b_lat[2]) );
  sr_search->x_min = - sr_search->x_max;

  sr_search->y_max = MAX(fabs(sr_search->b_lat[3]), fabs(sr_search->b_lat[4]) );
  sr_search->y_min = - sr_search->y_max;

  CONTROL_MSG(CONTROL, "x_min/max = %6.3f/%6.3f\ny_min/max = %6.3f/%6.3f\n",
              sr_search->x_min, sr_search->x_max,
              sr_search->y_min, sr_search->y_max);

  /* Symmetry:
   *   - if n_par == 0: create parameter reference list according to the
   *     symmetry parameters given in the input file.
   *   - if n_par != 0: go through the x/y/z_par vectors and reset them to
   *     zero if they have not been specified during read in.
   */

  if(n_par == 0)
  {
    CONTROL_MSG(CONTROL, "rot_deg = %d, axis = (%6.3f,%6.3f)\n",
            sr_search->rot_deg, sr_search->rot_axis[1], sr_search->rot_axis[2]);
    n_par = sr_ckrot(sr_atoms, sr_search);
  }
  else
  {
    CONTROL_MSG(CONTROL_X, "n_par = %d, n_atoms = %d\n", n_par, n_atoms);

    for(i_atoms = 0; i_atoms <= n_atoms; i_atoms ++)
    {
      CONTROL_MSG(CONTROL_X, "i_atoms = %d", i_atoms);

      fprintf(STDCTR, "x\n");
      if( (sr_atoms+i_atoms)->x_par == NULL )
      {
        (sr_atoms+i_atoms)->x_par = (real *)calloc( (n_par +1), sizeof(real) );
      }
      fprintf(STDCTR, "y\n");
      if( (sr_atoms+i_atoms)->y_par == NULL )
      {
        (sr_atoms+i_atoms)->y_par = (real *)calloc( (n_par +1), sizeof(real) );
      }
      fprintf(STDCTR, "z\n");
      if( (sr_atoms+i_atoms)->z_par == NULL )
      {
        (sr_atoms+i_atoms)->z_par = (real *)calloc( (n_par +1), sizeof(real) );
      }

    } /* for i_atoms */

  }

# if CONTROL
  fprintf(STDCTR, "\n(sr_rdinp): %d parameters, atoms:\n", n_par);
  for(i_atoms = 0; i_atoms < n_atoms; i_atoms ++)
  {
    fprintf(STDCTR, "\n%d \"%s\" (%6.3f, %6.3f, %6.3f) ref: %d n_ref: %d",
            i_atoms, (sr_atoms + i_atoms)->name,
            (sr_atoms + i_atoms)->x, (sr_atoms + i_atoms)->y,
            (sr_atoms + i_atoms)->z,
            (sr_atoms + i_atoms)->ref, (sr_atoms + i_atoms)->n_ref);
    fprintf(STDCTR, "\n\tr_min: %6.3f", (sr_atoms + i_atoms)->r_min);

    if(!sr_search->z_only)
    {
      fprintf(STDCTR, "\n\tx_par: ");
      for(i_par = 1; i_par <= n_par; i_par ++)
      {
        fprintf(STDCTR, "%6.3f, ", (sr_atoms+i_atoms)->x_par[i_par]);
      }
      fprintf(STDCTR, "\n\ty_par: ");
      for(i_par = 1; i_par <= n_par; i_par ++)
      {
        fprintf(STDCTR, "%6.3f, ", (sr_atoms+i_atoms)->y_par[i_par]);
      }
    }
    fprintf(STDCTR, "\n\tz_par: ");
    for(i_par = 1; i_par <= n_par; i_par ++)
    {
      fprintf(STDCTR, "%6.3f, ", (sr_atoms+i_atoms)->z_par[i_par]);
    }
    fprintf(STDCTR, "\n");
  }
  fprintf(STDCTR, "\n(sr_rdinp): n_par = %d", n_par);
  fprintf(STDCTR, "\n(sr_rdinp): return\n");
# endif

  /* increment n_par by 2 for angle search */
  if(sr_search->sr_angle)
  {
    sr_search->n_par_geo   = sr_search->n_par;
    sr_search->i_par_theta = sr_search->n_par_geo+1;
    sr_search->i_par_phi   = sr_search->n_par_geo+2;
    sr_search->n_par += 2;
    n_par += 2;
  }
  else
  {
    sr_search->n_par_geo = sr_search->n_par;
    sr_search->i_par_phi   = 0;
    sr_search->i_par_theta = 0;
  }

  return((int)n_par);
}
