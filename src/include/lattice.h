/************************************************************************
 *                           LATTICE.H
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *
 * Description: Header for struct lattice and contains prototypes for
 *              associated functions.
 *
 * Changes:
 *   LD/2014.08.28 - Creation
 ************************************************************************/

#ifndef LATTICE_H
#define LATTICE_H

#include "atom.h"
#include "coord.h"
#include "miller_index.h"
#include "basis.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* define constants */
#define  STRSZ 256
#define  NAMSZ 128

#define TOLERANCE (1E-2)
#define IMAGE_LEN 30.

#ifndef MAX_INP_ATOMS
#define MAX_INP_ATOMS 256
#endif

#ifndef MAX_OUT_ATOMS
#define MAX_OUT_ATOMS 4096
#endif

#define LAT_INP 100

/*! \typedef \enum lattice_error_code_t
 *  \brief return codes from functions operating on lattice struct
 */
typedef enum {
  LATTICE_OPERATION_SUCCESS=0,
  LATTICE_ATOM_INDEX_OUT_OF_RANGE,
  LATTICE_ALLOC_FAILURE,
  LATTICE_STRING_ALLOC_FAILURE
} lattice_error_code;

/*! \enum latt_type
 *  \brief indicates lattice type
 */
typedef enum {
  LAT_FCC=1,
  LAT_HCP,
  LAT_BCC,
  LAT_DIA,
  LAT_UNKNOWN
} latt_type;

/*! \typedef lattice
 *  \brief lattice struct.
 *
 * Contains all the information for generating a lattice of different 
 * atoms.
 */
typedef struct {
  latt_type latt_type;  /*!< Type of lattice e.g. HCP or FCC. */
  double a_nn;          /*!< nearest neighbour distance. */
  double a_latt;        /*!< a lattice constant. */
  double b_latt;        /*!< b lattice constant. */
  double c_latt;        /*!< c lattice constant. */
  double max_disp;      /*!< Maximum abs(x) displacement. */
  double max_disp_z;    /*!< Maximum abs(z) displacement. */
  size_t max_layers;    /*!< Maximum number of layers. */
  size_t max_cells;     /*!< Maximum number of in-plane cell displacements. */
  size_t max_atoms;     /*!< Maximum number of atoms. */
  double image_len;
  double vec_h;         /*!< h Miller index. */
  double vec_k;         /*!< k Miller index. */
  double vec_l;         /*!< l Miller index. */
  char *input_filename; 
  char *output_filename;
  char *script;         /*!< do jmol script to write to xyz output */
  atom *atoms;          /*!< Array of atoms. */
  size_t n_atoms;
  size_t allocated_atoms;
} lattice;

extern FILE *inf_stream;
extern FILE *ctr_stream;

/* function prototypes */
void latt_usage(FILE *output);
void latt_parse_args(int argc, char *argv[], lattice *latt);
void latt_info();

void lattice_generate_atoms(lattice *latt);
lattice *lattice_read_input(const char *filename);
lattice *lattice_alloc(size_t n_atoms);
lattice *lattice_init(size_t n_atoms);
int lattice_realloc(lattice *lat, size_t size);
int lattice_set_input_filename(lattice *lat, const char *filename);
int lattice_set_output_filename(lattice *lat, const char *filename);
int lattice_set_script(lattice *lat, const char *script);
void lattice_free(lattice *lat);
void lattice_free_atom_list(lattice *lat);
void lattice_read(lattice *lat, coord *a1, coord *a2, coord *a3, 
                  coord *nor, coord *bas, char *bas_name, size_t *n_bas);
int lattice_set_atom(lattice *lat, const atom *atom, size_t index);
int lattice_set_atom_list(lattice *lat, const atom *atoms, size_t n_atoms);
void lattice_printf(FILE *output, const lattice *lat);
void lattice_debug(const lattice *lat);

double lattice_get_a(const lattice *lat);
double lattice_get_b(const lattice *lat);
double lattice_get_c(const lattice *lat);
double lattice_get_max_disp(const lattice *lat);
double lattice_get_max_disp_z(const lattice *lat);
size_t lattice_get_max_layers(const lattice *lat);
size_t lattice_get_max_cells(const lattice *lat);
double lattice_get_h(const lattice *lat);
double lattice_get_k(const lattice *lat);
double lattice_get_l(const lattice *lat);
const char *lattice_get_input_filename(const lattice *lat);
const char *lattice_get_output_filename(const lattice *lat);
size_t lattice_get_n_atoms(const lattice *lat);
const atom *lattice_get_atom_list(const lattice *lat);
atom *lattice_get_atom(const lattice *lat, size_t index);
void lattice_atom_index_swap(const lattice *lat, size_t i, size_t j);

coord *lattice_get_surface_normal(const lattice *lat, const coord *a1,
                                    const coord *a2, const coord *a3);
double **lattice_normal_get_rotation_matrix(const coord *normal);
void lattice_setup(lattice *lat, coord *a1, coord *a2, coord *a3, coord *nor,
                   coord *bas, char *bas_name, size_t *n_bas);
         
miller_hkl *lattice_get_miller_hkl(const lattice *lat);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

class Lattice {

  public:
    Lattice();
    ~Lattice();

  protected:
    lattice *lat;
}


} /* namespace cleed */

#endif /* ifdef */
                                        
#endif /* LATTICE_H */
