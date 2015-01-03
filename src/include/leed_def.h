/*********************************************************************
 *                           LEED_DEF.H
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
 *   GH/1995.09.20 - creation
 *   ST/1997.07.16 -
 *   GH/1997.09.02 - change beam_str and cryst_str (add. elements),
 *                   define S0, SX, SY, SXY.
 *   WB/1998.08.07 - add k_rsym (symmetry related beams) to beam_str.
 *   WB/1998.08.27 - include symmetry flags in crystal struct
 *   GH/2000.05.03 - include t_type in atom_str.
 *                 - include t_type in phs_str.
 *   LD/2014.07.28 - added struct typedefs, C linkage from C++ and
 *                   Doxygen compatible comments.
 *   LD/2014.10.08 - added leed_matrix_diag enum for T_DIAG & T_NOND
 *                 - added leed_structure enum for BULK and OVER
 *                 - added leed_mirror_sym enum for mirror symmetry axes.
 *                 - changed int declarations to size_t where appropriate.
 *
 *  version SYM 1.1 + TEMP 0.5
 *  GH/27.09.00 - same include file for version SYM 1.1 + TEMP 0.5
 *
 *********************************************************************/

/*!
 * \file
 * \brief Header file for data structures, type definitions and
 * constant values used in the LEED programs \c leed_sym and \c leed_nsym .
 */

#ifndef LEED_DEF_H
#define LEED_DEF_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include "leed_ver.h"
#include "real.h"
#include "mat_def.h"

/*********************************************************************
 * Fundamental constants/conversion factors
 * (Source: CRC Handbook, 73rd Edition)
 *********************************************************************/

static const double HART = 27.2113962;  /*!< Hartree in eV */
static const double BOHR = 0.529177249; /*!< Bohr radius in Angstroms */
static const double MEL_U = 5.48579903e-4; /*!< electron mass in amu */
static const double U_MEL = 1822.88850636; /*!< 1 amu in multiples of electron mass */
static const double KB = 3.16682941e-6;  /*!< Boltzmann constant in Hartree/K */

/*********************************************************************
 * Program parameters
 * - tolerance
 * - threshold values etc.
 *********************************************************************/

/* Defaults */

/*!
 * Default path name for phase shift input relative to HOME directory
 */
//static const char *PHASE_PATH = "phase";
                               
/*!
 * Default temperature for calculating thermal vibrations
 */
static const double DEF_TEMP = 300.;

/*!
 * Default muffin-tin-radius (in BOHR) used to calculate l_max (if not given)
 */
static const double R_FOR_LMAX = 2.0;

/* Threshold values */
/*!
 * Minimum distance for Layer Doubling in BOHR (1.0 A)
 */
static const double MIN_DIST = 1.5;

/*!
 * Start value for variable imag. opt. potential (100. eV/HART)
 */
static const double VI_START = 3.67493;

/* Tolerances */
static const double E_TOLERANCE = 1.e-3;    /*!< tolerance for comparing two energies */
static const double GEO_TOLERANCE = 1.e-3;  /*!< tolerance for geometrical parameters in BOHR */
static const double INT_TOLERANCE = 1.e-10; /*!< min. intensity > 0. */
static const double K_TOLERANCE = 1.e-4;    /*!< tolerance for k_par in (BOHR)^-1 */
static const double LD_TOLERANCE = 1.e-4;   /*!< convergence criterion for layer doubling */
static const double WAVE_TOLERANCE = 1.e-4; /*!< tolerance for wave amplitudes */

/* Flags for mirror planes etc. */

/*! \typedef leed_structure
 *  \brief Indicates whether structure is the bulk or an overlayer.
 */
typedef enum {
  BULK=0,       /*!< Bulk structure */
  OVER          /*!< Overlayer structure */
} leed_structure;

/*! \typedef leed_matrix_diag
 *  \brief indicates whether a diagonal T-matrix or not.
 */
typedef enum {
  T_DIAG=0,     /*!< Diagonal matrix */
  T_NOND        /*!< Non-diagonal matrix */
} leed_matrix_diag;

/*! \typedef leed_mirror_sym
 *  \brief indicates mirror symmetry type.
 */
typedef enum {
  S0=0,       /*!< no mirror symmetry */
  SX,         /*!< mirror symmetry along x-axis direction */
  SY,         /*!< mirror symmetry along y-axis direction */
  SXY         /*!< mirror symmetry along xy direction */
} leed_mirror_sym;

/*!
 * Return codes for LEED program.
 */
typedef enum {
  LEED_ERROR = -1,        /*!< general failure */
  LEED_SUCCESS,           /*!< success */
  LEED_ALLOCATION_ERROR,  /*!< memory allocation failure */
  LEED_FILE_IO_ERROR,     /*!< file read/write failure */
  LEED_INVALID_ENERGY     /*!< error when initial, final or energy step
                           *   is incorrect */
} leed_error;


typedef enum {
  NOSYM     = 101,  /*!< Indicates no symmetry for structure */
  MONO_2ROT = 102,  /*!< Monoclinic structure with 2-fold rot. symmetry */
  MONO_1MIR = 111,  /*!< Monoclinic structure with 1 mirror plane */

  REC_2ROT  = 202,  /*!< Rectangular structure with 2-fold rot. symmetry */
  REC_1MIR  = 211,  /*!< Rectangular structure with 1 mirror plane */
  REC_2MIR  = 221,  /*!< Rectangular structure with 2 mirror planes */

  HEX_1MIR  = 311,  /*!< Hexagonal structure with 1 mirror plane */
  HEX_3ROT  = 303,  /*!< Hexagonal structure with 3-fold rot. symmetry */
  HEX_3MIR  = 331,  /*!< Hexagonal structure with 3 mirror planes */

  SQ_2ROT   = 402,  /*!< Square structure with 2-fold rotational symmetry */
  SQ_4ROT   = 404,  /*!< Square structure with 4-fold rotational symmetry */
  SQ_1MIR   = 411,  /*!< Square structure with 1 mirror planes */
  SQ_2MIR   = 421,  /*!< Square structure with 2 mirror planes */
  SQ_4MIR   = 441   /*!< Square structure with 4 mirror planes */
} leed_symmetry;

/* current version No. now in file leed_ver.h */

/*********************************************************************
 *structures and types
 *********************************************************************/

/*! \typedef leed_atom
 *  \brief Contains all properties of a single atom in the model */
typedef struct leed_atom /*! Holds properties of a single atom */
{
  size_t layer;    /*!< number of layer where the atom belongs to */
  leed_structure type;
                   /*!< type of atom (i. e. set of phase shifts to be used) */
  leed_matrix_diag t_type;
                   /*!< type of t matrix (T_DIAG or T_NOND) */
  real pos[4];     /*!< relative position inside the unit cell */
  real dwf;        /*!< Debye-Waller factor */
} leed_atom;

/*! \typedef leed_layer
 *  \brief contains all properties of a single Bravais layer. */
typedef struct leed_layer /*! Holds properties of a single Bravais layer */
{
  size_t no_of_layer;       /*!< number of layer in array */
  leed_structure bulk_over; /*!< BULK (0) or OVER */
  int  periodic;            /*!< 1: layer is part of the periodically
                             *      repeated bulk unit cell.
                             *   0: layer is only used once.
                             */
  size_t n_atoms;         /*!< number of atoms in the layer */
  real a_lat[5];          /*!< basis vectors of the real 2-dim unit cell stored
                           * as standard matrix (a1,a2):
                           *    a1x = a_lat[1], a2x = a_lat[2]
                           *    a1y = a_lat[3], a2y = a_lat[4]
                           */
  real rel_area;          /*!< area of the unit cell relative to 1x1 */
  real reg_shift[4];      /*!< 2-dim vector pointing to the axis
                           *   of rot. symmetry/mirror plane
                           */
  real vec_from_last[4];  /*!< vector from the origin of layer (n-1) */
  real vec_to_next[4];    /*!< vector to the origin of layer (n+1) */
  leed_atom *atoms;       /*!< properties of the atoms within the composite
                           * (or Bravais) layers */
} leed_layer;

/*! \typedef leed_crystal
 *
 * Contains all the crystallographic parameters e.g. for the bulk model or
 * the overlayer structure.
 */
typedef struct leed_crystal /*! Holds all crystallographic parameters */
{

  /* general parameters */
  real vr;         /*!< real part of the optical potential */
  real vi;         /*!< imaginary part optical potential */
  real temp;       /*!< crystal temperature */

  /* symmetries */
  size_t n_rot;    /*!< degree of rotational symmetry */
  real rot_axis[4];/*!< axis of rotational symmetry */
  size_t n_mir;    /*!< number of mirror plane */
  real *m_plane;   /*!< points define mirror plane */
  real *alpha;     /*!< angle in degree */
  leed_mirror_sym symmetry;  /*!< NOSYM(0) ROTSYM(1) MIRRORSYM(2) RMSYM(3)*/

  /* 1x1 unit cell */
  real a[5];       /*!< basis vectors of the real 2-dim unit cell stored as
                    *   standard matrix (a1,a2):
                    *     a1x = a[1], a2x = a[2]
                    *     a1y = a[3], a2y = a[4]
                    */
  real a_1[5];     /*!< inverse of a (= 2PI * (a^-1))
                    *   => the rows are the basis vectors of the reciprocal
                    *   2-dim unit cell:
                    *     a*1x = a_1[1], a*1y = a_1[2]
                    *     a*2x = a_1[3], a*2y = a_1[4]
                    */
  real area;       /*! Area of the real 2-dim (1x1) unit cell */

  real m_trans[5]; /*!< 2x2 transformation matrix: unit cell used in the
                    * programs with respect to the input:
                    * a_inp = m_trans * a_prg
                    */

  /* superstructure */
  real m_super[5]; /*!< 2x2 superstructure matrix: b = m_super * a */
  real m_recip[5]; /*!< reciprocal 2x2 superstructure matrix: Mt^-1 */

  real b[5];       /*!< basis vectors of the real 2-dim superstructure unit cell
                    *   stored as standard matrix (b1,b2):
                    *     b1x = b[1], a2x = b[2]
                    *     b1y = b[3], a2y = b[4]
                    */
  real b_1[5];     /*!< inverse of b (= 2PI * (b^-1))
                    *   => the rows are the basis vectors of the reciprocal
                    *   2-dim super structure unit cell:
                    *     b*1x = b_1[1], b*1y = b_1[2]
                    *     b*2x = b_1[3], b*2y = b_1[4]
                    */
  real rel_area_sup; /*!< Area of the real 2-dim superstructure unit cell
                      *   relative to (1x1)
                      */
                     
  /* stacking of layers */
  size_t n_layers; /*!< number of layers in array layers */
  leed_layer *layers; /*!< information concerning the atomic layers */
  real dmin;       /*!< minimum interlayer distance */

  size_t n_atoms;  /*!< total number of atoms */
  size_t n_types;  /*!< total number atom types */

  char **comments; /*!< comments */
} leed_crystal;


/*! \typedef leed_phase
 *  \brief Contains all phase shift parameters. */
typedef struct leed_phase  /*! Holds parameters for a single phase shift */
{
  int lmax;           /*!< maximum angular momentum quantum number */
  size_t n_eng;       /*!< number of energies in phase shift file */
  leed_matrix_diag t_type; /*!< type of scattering matrix: T_DIAG or T_NOND */
  real eng_max;        /*!< maximum energy */
  real eng_min;        /*!< minimum energy */
  real *energy;        /*!< array of energy values */
  real *pshift;        /*!< array of phase shift values */
 
  real dr[4];
  char input_file[FILENAME_MAX];    /*!< name of input file */
} leed_phase;

/*! \typedef leed_beam
 *  \brief Contains all parameters of a specific beam in k-space. */
typedef struct leed_beam /*! Holds parameters of a beam in k-space */
{
  real ind_1;     /*!< h beam index in (1x1) basis A (real) */
  real ind_2;     /*!< k beam index in (1x1) basis A (real) */
  int  b_ind_1;   /*!< h beam index in super-structure basis B (integer) */
  int  b_ind_2;   /*!< k beam index in super-structure basis B (integer) */
  real k_par;     /*!< k_vector length of the parallel components */
  real k_r[4];    /*!< k_r[0] = |k_r|, k_r[i] = kx,ky,kz */
  real k_i[4];    /*!< imagary part of k vector */
  real cth_r;     /*!< real part of cos (theta(k)) */
  real cth_i;     /*!< imaginary part of cos (theta(k)) */
  real phi;       /*!< phi(k) */
  real Akz_r;     /*!< factor 1/(Akz), needed to calculate the scattering matrix */
  real Akz_i;     /*!< factor 1/(Akz), needed to calculate the scattering matrix */

  /* symmetry related phase factors */
  real k_p_sym[12]; /*!< angular difference (phi) between all symmetry-related
                     *    beams and the representant beam (12 is max.
                     *    number of beams)
                     */
  real k_x_sym[12]; /*!< x component of all symmetry-related beams represented
                     *   by this beam (0) (12 is max. number of beams)
                     */
  real k_y_sym[12]; /*!< y component of all symmetry-related beams represented
                     *   by this beam (0) (12 is max. number of beams)
                     */
  size_t n_eqb_b;   /*!< number of equivalent beams represented by this beam
                     * (bulk layers)
                     */
  size_t n_eqb_s;   /*!< number of equivalent beams represented by this beam
                     * (superstructure)
                     */
  real *eout_b_r;   /*!< phase factor for the outgoing beam:
                     *   sqrt(n_rot) S exp(+is*g')), */
  real *eout_b_i;   /*!< needed to calculate the scattering matrix,
                     *   length = n_layer*/
  real *eout_s_r;   /*!< b: bulk layers, s: superstructure */
  real *eout_s_i;
  real *ein_b_r;    /*!< phase factor for the incoming beam:
                     *   1/sqrt(n_rot) S exp(-i(m*phi + s*g)), */
  real *ein_b_i;    /*!< needed to calculate the scattering matrix,
                     *   length = (2*l_max + 1) * n_layer*/
  real *ein_s_r;    /*!< b: bulk layers, s: superstructure */
  real *ein_s_i;

  size_t set;       /*!< beam set, where the beam belongs to */
} leed_beam;


/*! \typedef leed_var
 *  
 * Contains all parameters that change during the energy
 * loop and the parameters controlling them. */
typedef struct leed_var /*! Holds parameters that change during energy loop */
{
  real eng_r;    /*!< current energy in crystal (real  part) */
  real eng_i;    /*!< current energy in crystal (imag. part) */
  real eng_v;    /*!< current vacuum energy */
  real vr;       /*!< real part of the optical potential */
  real vi_pre;   /*!< prefactor: imag. part of the optical potential */
  real vi_exp;   /*!< exponent: imag. part of the optical potential */

  real theta;    /*!< polar angle of incidence */
  real phi;      /*!< azimuth angle of incidence */
  real k_in[4];  /*!< incident beam:
                  *   k[0] = |k_par| k[1/2] = k_par_x/y k[3] = k_z
                  */

  real epsilon;  /*!< decimal tolerance when comparing floating point */
  size_t l_max;  /*!< max. l quantum number used in the calculation */
  mat *p_tl;     /*!< array of diagonal atomic scattering matrices
                  *   (1st dim = lmax, 2nd dim = 1) */
} leed_var;


/*! \typedef leed_energy
 *  \brief Contains the parameters that control the energy loop. */
typedef struct leed_energy /*! Contains all parameters that change during
                            *  the energy loop and the parameters
                            *  controlling them */
{
  real initial;      /*!< initial energy */
  real final;        /*!< final energy */
  real step;         /*!< energy step */
} leed_energy;

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */

#endif

#endif /* LEED_DEF_H */


