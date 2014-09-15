/****************************************************************************
                        LATT.H (Georg Held 01.05.06)
                        
Changes:
 LD/29.04.14 - Added header for latt.c                 
         
****************************************************************************/
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

#define LAT_INP  100

#define LATTICE_OPERATION_SUCCESS 0
#define LATTICE_ATOM_INDEX_OUT_OF_RANGE_FAILURE 10
#define LATTICE_ALLOC_FAILURE 1
#define LATTICE_STRING_ALLOC_FAILURE 2

/*! \enum latt_type_t
 *  \brief lattice types
 */
typedef enum {
  LAT_FCC=1,
  LAT_HCP,
  LAT_BCC,
  LAT_DIA,
  LAT_UNKNOWN
} latt_type_t;

/*! \typedef lattice_t
 *  \brief lattice struct.
 *
 * Contains all the information for generating a lattice of different 
 * atoms.
 */
typedef struct {
  int latt_type;        /*!< Type of lattice e.g. HCP or FCC. */
  double a_nn;          /*!< nearest neighbour distance. */
  double a_latt;        /*!< a lattice constant. */
  double b_latt;        /*!< b lattice constant. */
  double c_latt;        /*!< c lattice constant. */
  double max_disp;         /*!< Maximum abs(x) displacement. */
  double max_disp_z;         /*!< Maximum abs(z) displacement. */
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
  atom_t *atoms;        /*!< Array of atoms. */
  size_t n_atoms;
  size_t allocated_atoms;
} lattice_t;

extern FILE *inf_stream;
extern FILE *ctr_stream;

/* function prototypes */
void latt_usage(FILE *output);
void latt_parse_args(int argc, char *argv[], lattice_t *latt);
void latt_info();

void lattice_generate_atoms(lattice_t *latt);
lattice_t *lattice_read_input(const char *filename);
lattice_t *lattice_alloc(size_t n_atoms);
lattice_t *lattice_init(size_t n_atoms);
int lattice_realloc(lattice_t *lat, size_t size);
int lattice_set_input_filename(lattice_t *lat, const char *filename);
int lattice_set_output_filename(lattice_t *lat, const char *filename);
int lattice_set_script(lattice_t *lat, const char *script);
void lattice_free(lattice_t *lat);
void lattice_free_atom_list(lattice_t *lat);
void lattice_read(lattice_t *lat, coord_t *a1, coord_t *a2, coord_t *a3, 
                  coord_t *nor, coord_t *bas, char *bas_name, int *n_bas);
int lattice_set_atom(lattice_t *lat, const atom_t *atom, size_t index);
int lattice_set_atom_list(lattice_t *lat, const atom_t *atoms, size_t n_atoms);
void lattice_printf(FILE *output, const lattice_t *lat);
void lattice_debug(const lattice_t *lat);

double lattice_get_a(const lattice_t *lat);
double lattice_get_b(const lattice_t *lat);
double lattice_get_c(const lattice_t *lat);
double lattice_get_max_disp(const lattice_t *lat);
double lattice_get_max_disp_z(const lattice_t *lat);
size_t lattice_get_max_layers(const lattice_t *lat);
size_t lattice_get_max_cells(const lattice_t *lat);
double lattice_get_h(const lattice_t *lat);
double lattice_get_k(const lattice_t *lat);
double lattice_get_l(const lattice_t *lat);
const char *lattice_get_input_filename(const lattice_t *lat);
const char *lattice_get_output_filename(const lattice_t *lat);
size_t lattice_get_n_atoms(const lattice_t *lat);
const atom_t *lattice_get_atom_list(const lattice_t *lat);
atom_t *lattice_get_atom(const lattice_t *lat, size_t index);
void lattice_atom_index_swap(const lattice_t *lat, size_t i, size_t j);

coord_t *lattice_get_surface_normal(const lattice_t *lat, const coord_t *a1,
                                    const coord_t *a2, const coord_t *a3);
double **lattice_normal_get_rotation_matrix(const coord_t *normal);
void lattice_setup(lattice_t *lat, coord_t *a1, coord_t *a2, 
         coord_t *a3, coord_t *nor, coord_t *bas, char *bas_name, int *n_bas);
         
miller_hkl_t *lattice_get_miller_hkl(const lattice_t *lat);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
} /* extern "C" */

namespace cleed {

class Lattice {

  public:
    Lattice();
    ~Lattice();

  protected:
    lattice_t *lat;
}


} /* namespace cleed */

#endif /* ifdef */
                                        
#endif /* LATTICE_H */
