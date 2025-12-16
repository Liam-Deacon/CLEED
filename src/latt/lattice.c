#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lattice.h"

static int lattice_resize_basis(coord_t **bas, char **bas_name, size_t n_bas)
{
  int rc = 0;

  coord_t *new_bas = (coord_t *)realloc(*bas, sizeof(coord_t) * n_bas);
  if (!new_bas)
  {
    rc = -1;
  }
  else
  {
    *bas = new_bas;
  }

  char *new_names = (char *)realloc(*bas_name, n_bas * NAMSZ * sizeof(char));
  if (!new_names)
  {
    rc = -1;
  }
  else
  {
    *bas_name = new_names;
  }

  return rc;
}

static void lattice_copy_element_name(char *dest, const char *element)
{
  if (!dest)
  {
    return;
  }

  if (!element)
  {
    dest[0] = '\0';
    return;
  }

  snprintf(dest, NAMSZ, "%s", element);
}

static bool lattice_line_startswith(const char *line, const char *prefix)
{
  return strncmp(line, prefix, strlen(prefix)) == 0;
}

static bool lattice_line_equals(const char *line, const char *value)
{
  size_t len = strlen(value);
  if (strncmp(line, value, len) != 0)
  {
    return false;
  }
  char after = line[len];
  return after == '\0' || after == '\n' || after == '\r';
}

static void lattice_log_line(const char *line)
{
  fprintf(inf_stream, "%s", line);
#if DEBUG == 1
  fprintf(ctr_stream, "%s", line);
#endif
}

static void lattice_print_help(void)
{
  printf("Valid input commands are as follows - \n"
         "'a1:' first basis vector \n"
         "'a2:' second basis vector \n"
         "'a3:' third basis vector \n"
         "'m1:' superstructure matrix row 1 \n"
         "'m2:' superstructure matrix row 2 \n"
         "'pb:' bulk atom \n"
         "'po:' overlayer atom \n"
         "'il:' image length \n"
         "'nl:' max number of layers \n"
         "'uc:' max number of unit cells \n"
         "'q'\n"
         "'help' \n");
}

static void lattice_normalize_basis(coord_t *bas, size_t n_bas)
{
  if (!bas || n_bas == 0)
  {
    return;
  }

  coord_t offset = {.x = 0., .y = 0., .z = 0.};
  for (size_t i = 1; i < n_bas; ++i)
  {
    if (bas[i].z > offset.z)
    {
      offset.x = bas[i].x;
      offset.y = bas[i].y;
      offset.z = bas[i].z;
    }
  }

  for (size_t i = 0; i < n_bas; ++i)
  {
    bas[i].x -= offset.x;
    bas[i].y -= offset.y;
    bas[i].z -= offset.z;
  }
}

static bool lattice_parse_coord_line(const char *line, const char *prefix,
                                     coord_t *value)
{
  if (!lattice_line_startswith(line, prefix))
  {
    return false;
  }

  double x = 0., y = 0., z = 0.;
  if (sscanf(line + strlen(prefix), "%lf %lf %lf", &x, &y, &z) == 3)
  {
    coord_set(value, x, y, z);
    return true;
  }

  return false;
}

static bool lattice_parse_matrix_line(const char *line, const char *prefix,
                                     double *first, double *second)
{
  if (!lattice_line_startswith(line, prefix))
  {
    return false;
  }

  return sscanf(line + strlen(prefix), "%lf %lf", first, second) == 2;
}

static bool lattice_parse_atom_line(const char *line, const char *prefix,
                                    char *dest_name, coord_t *position)
{
  if (!lattice_line_startswith(line, prefix))
  {
    return false;
  }

  return sscanf(line + strlen(prefix), " %127s %lf %lf %lf", dest_name,
                &position->x, &position->y, &position->z) == 4;
}

static bool lattice_parse_double_line(const char *line, const char *prefix,
                                     double *value)
{
  if (!lattice_line_startswith(line, prefix))
  {
    return false;
  }

  char *endptr = NULL;
  double result = strtod(line + strlen(prefix), &endptr);
  if (endptr == line + strlen(prefix))
  {
    return false;
  }

  *value = result;
  return true;
}

static bool lattice_parse_size_t_line(const char *line, const char *prefix,
                                      size_t *value)
{
  if (!lattice_line_startswith(line, prefix))
  {
    return false;
  }

  char *endptr = NULL;
  unsigned long long result = strtoull(line + strlen(prefix), &endptr, 10);
  if (endptr == line + strlen(prefix))
  {
    return false;
  }

  *value = (size_t)result;
  return true;
}

static bool lattice_is_quit_command(const char *line)
{
  return line[0] == 'q';
}

static bool lattice_is_help_command(const char *line)
{
  return lattice_line_equals(line, "help");
}

void lattice_debug(const lattice_t *lat)
{
  printf("(h, k, l) = (%7.4f, %7.4f, %7.4f)\n", 
         lat->vec_h, lat->vec_k, lat->vec_l);
  printf("a_nn = %7.4f\n", lat->a_nn);
  printf("a_latt = %7.4f\n", lat->a_latt);
  printf("c_latt = %7.4f\n", lat->c_latt);
  printf("max_disp = %7.4f\n", lat->max_disp);
  printf("max_disp_z = %7.4f\n", lat->max_disp_z);
  printf("max_layers = %zu\n", lat->max_layers);
  printf("max_cells = %zu\n", lat->max_cells);
  printf("max_atoms = %zu\n", lat->max_atoms);
  printf("image_len = %7.4f\n", lat->image_len);
}

/********************************************************
 Write list of atoms to output file
********************************************************/
void lattice_printf(FILE *output, const lattice_t *lat)
{
  size_t i_atom;
  char padding[2];
  
  /* first line: number of atoms */
  fprintf(output, "%zu\n", lat->n_atoms);

  /* second line: script */
  if(strlen(lat->script) > 0) 
  {
    if (lat->script[0] != '#') fprintf(output, "#");
	fprintf(output, "%s #\n", lat->script);
  }
  

  /****
  for( i_atom = 0; i_atom < latt->n_atoms; i_atom ++ )
  {
    fprintf(out_stream, "# %s", latt->atoms[i_atom].element);
  }
  */
  fprintf(output, "# (%.0f;%.0f;%.0f)\n", 
          lat->vec_h, lat->vec_k, lat->vec_l);
  
  /* list of atoms */
  for(i_atom = 0; i_atom < lat->n_atoms; i_atom ++)
  {
      if (strlen(lat->atoms[i_atom].element) == 1)
      {
        strcpy(padding, " ");
      }
      else 
      {
        strcpy(padding, "");
      }
      fprintf(output, "%s%s %7.3f %7.3f %7.3f\n", 
             lat->atoms[i_atom].element, padding,
             lat->atoms[i_atom].x, 
             lat->atoms[i_atom].y, 
             lat->atoms[i_atom].z);
  }

}

lattice_t *lattice_alloc(size_t n_atoms)
{
  lattice_t *lat = (lattice_t*) malloc(sizeof(lattice_t));
  if (lat == NULL) return NULL;
  lat->atoms = (atom_t*) malloc(sizeof(atom_t) * (n_atoms+1));
  lat->input_filename = (char*) malloc(sizeof(char) * PATH_MAX);
  lat->output_filename = (char*) malloc(sizeof(char) * PATH_MAX);
  lat->script = (char*) malloc(sizeof(char) * STRSZ * STRSZ);
  if (lat->atoms == NULL)
  {
    free(lat);
    return NULL;
  }
  lat->n_atoms = n_atoms;
  lat->allocated_atoms = n_atoms+1;
  strcpy(lat->input_filename, "\0");
  strcpy(lat->output_filename, "\0");
  strcpy(lat->script, "\0");
  return (lat);
}

lattice_t *lattice_init(size_t n_atoms)
{
  size_t i_atom;
  lattice_t *lat = lattice_alloc(n_atoms);
  
  if (lat == NULL) return NULL;
  
  /* initialise member variables */
  lat->a_latt = 1.;
  lat->b_latt = 0.;
  lat->c_latt = 0.;
  
  lat->max_disp = 10.;
  lat->max_disp_z = 10.;

  lat->latt_type = LAT_FCC;
  
  lat->max_layers = 20;
  lat->max_cells = 5;
  lat->image_len = IMAGE_LEN;
  lat->max_atoms = MAX_OUT_ATOMS;
  
  lat->vec_h = 0.;
  lat->vec_k = 0.;
  lat->vec_l = 1.;
  
  for (i_atom = 0; i_atom < lat->n_atoms; i_atom++)
  {
    lat->atoms[i_atom].x = 0.;
    lat->atoms[i_atom].y = 0.;
    lat->atoms[i_atom].z = 0.;
  }
  
  return (lat);
}

int lattice_realloc(lattice_t *lat, size_t size)
{
  lattice_t *result;
  atom_t *temp = lat->atoms;
  
  result = (lattice_t*) realloc(lat->atoms, sizeof(atom_t) * (size+1));
  if (!result)
  { 
    lat->atoms = temp;
    return (LATTICE_ALLOC_FAILURE);
  }

  lat->allocated_atoms = size+1; 
  
  return (LATTICE_OPERATION_SUCCESS);
}

void lattice_free(lattice_t *lat)
{
  /* clean up linked lists */
  free(lat->input_filename);
  free(lat->output_filename);
  free(lat->script);
  lattice_free_atom_list(lat);
  free(lat->atoms);
  lat->atoms = NULL;
  lat->input_filename = NULL;
  lat->output_filename = NULL;
  lat->script = NULL;

  /* clean up lattice_t struct */
  free(lat);
  lat = NULL;
}

void lattice_free_atom_list(lattice_t *lat)
{
  size_t i_atom;
  
  for (i_atom = 0; i_atom < lat->n_atoms; i_atom++) 
  {
    free(lat->atoms[i_atom].element);
  }
}

int lattice_set_atom(lattice_t *lat, const atom_t *atom, size_t index)
{
  if (index >= lat->allocated_atoms) 
  {
    return (LATTICE_ATOM_INDEX_OUT_OF_RANGE_FAILURE);
  }
 
  lat->atoms[index].element = (char *) malloc(sizeof(char) * 
                                                  strlen(atom->element));
  strcpy(lat->atoms[index].element, atom->element); 
  lat->atoms[index].x = atom->x;
  lat->atoms[index].y = atom->y;
  lat->atoms[index].z = atom->z;
  
  return (LATTICE_OPERATION_SUCCESS);
}

int lattice_set_atom_list(lattice_t *lat, const atom_t *atoms, size_t n_atoms)
{
  size_t i_atom;
  
  if (n_atoms >= lat->allocated_atoms) 
  {
    if (lattice_realloc(lat, n_atoms) == LATTICE_OPERATION_SUCCESS)
    {
      lat->n_atoms = n_atoms;
    }
    else 
    {
      return (LATTICE_ALLOC_FAILURE);
    }
  }
 
  for (i_atom = 0; i_atom < n_atoms; i_atom++)
  {
    lat->atoms[i_atom].x = atoms[i_atom].x;
    lat->atoms[i_atom].y = atoms[i_atom].y;
    lat->atoms[i_atom].z = atoms[i_atom].z;
    if (strlen(atoms[i_atom].element) > 0)
    {
      if (lat->atoms[i_atom].element == NULL)
      {
        lat->atoms[i_atom].element = (char *) malloc(sizeof(char) * 
                                            strlen(atoms[i_atom].element));
      }
      strcpy(lat->atoms[i_atom].element, atoms[i_atom].element);
    }
  }
  
  return (LATTICE_OPERATION_SUCCESS);
}

int lattice_set_input_filename(lattice_t *lat, const char *filename)
{
  if (lat->input_filename == NULL)
  {
    lat->input_filename = (char*) malloc(sizeof(char) * PATH_MAX);
    if (lat->input_filename == NULL) return (LATTICE_STRING_ALLOC_FAILURE);
  }
  if (filename != NULL) strcpy(lat->input_filename, filename);
  return (LATTICE_OPERATION_SUCCESS);
}

int lattice_set_script(lattice_t *lat, const char *script)
{
  if (lat->script == NULL)
  {
    lat->script = (char*) malloc(sizeof(char) * STRSZ * STRSZ);
    if (lat->script == NULL) return (LATTICE_STRING_ALLOC_FAILURE);
  }
  if (script != NULL) strcpy(lat->script, script);
  return (LATTICE_OPERATION_SUCCESS);
}

int lattice_set_output_filename(lattice_t *lat, const char *filename)
{
  if (lat->output_filename == NULL)
  {
    lat->output_filename = (char*) malloc(sizeof(char) * PATH_MAX);
    if (lat->output_filename == NULL) return (LATTICE_STRING_ALLOC_FAILURE);
  }
  if (filename != NULL) strcpy(lat->output_filename, filename);
  return (LATTICE_OPERATION_SUCCESS);
}

 double lattice_get_a(const lattice_t *lat)
{
  return (lat->a_latt);
}

 double lattice_get_b(const lattice_t *lat)
{
  return (lat->b_latt);
}

 double lattice_get_c(const lattice_t *lat)
{
  return (lat->c_latt);
}

 double lattice_get_max_disp(const lattice_t *lat)
{
  return (lat->max_disp);
}

 double lattice_get_max_disp_z(const lattice_t *lat)
{
  return (lat->max_disp_z);
}

 size_t lattice_get_max_layers(const lattice_t *lat)
{
  return (lat->max_layers);
}

 size_t lattice_get_max_cells(const lattice_t *lat)
{
  return (lat->max_cells);
}

 double lattice_get_h(const lattice_t *lat)
{
  return (lat->vec_h);
}

 double lattice_get_k(const lattice_t *lat)
{
  return (lat->vec_k);
}

 double lattice_get_l(const lattice_t *lat)
{
  return (lat->vec_l);
}

 const char *lattice_get_input_filename(const lattice_t *lat)
{  
  const char *filename = (const char*) lat->input_filename;
  return (filename);
}

 const char *lattice_get_output_filename(const lattice_t *lat)
{
  const char *filename = (const char*) lat->output_filename;
  return (filename);
}

 size_t lattice_get_n_atoms(const lattice_t *lat)
{
  return (lat->n_atoms);
}

atom_t *lattice_get_atom(const lattice_t *lat, size_t index)
{
  atom_t *atom = (atom_t*) malloc(sizeof(atom_t));
  
  if (index >= lat->n_atoms) return NULL;
  
  atom->x = lat->atoms[index].x;
  atom->x = lat->atoms[index].y;
  atom->x = lat->atoms[index].z;
  if (strlen(lat->atoms[index].element) > 0)
  {
    atom->element = (char*) malloc(sizeof(char) * 
                                strlen(lat->atoms[index].element));
    strcpy(atom->element, lat->atoms[index].element);
  }
  
  return (atom);
}

 const atom_t *lattice_get_atom_list(const lattice_t *lat)
{
  const atom_t *atoms = (const atom_t*) lat->atoms;
  return (atoms);
}

void lattice_atom_index_swap(const lattice_t *lat, size_t i, size_t j)
{
  atom_t *temp_atom = (atom_t*) malloc(sizeof(atom_t));
  temp_atom->element = (char*) malloc(sizeof(char) * NAMSZ);  
  
  if (i > lat->n_atoms || j > lat->n_atoms) return;
  
  temp_atom->x = lat->atoms[j].x;
  temp_atom->y = lat->atoms[j].y;
  temp_atom->z = lat->atoms[j].z;
  strncpy(temp_atom->element, lat->atoms[j].element, NAMSZ);
  
  lat->atoms[j].x = lat->atoms[i].x;
  lat->atoms[j].y = lat->atoms[i].y;
  lat->atoms[j].z = lat->atoms[i].z;
  strncpy(lat->atoms[j].element, lat->atoms[i].element, NAMSZ);
  
  lat->atoms[i].x = temp_atom->x;
  lat->atoms[i].y = temp_atom->y;
  lat->atoms[i].z = temp_atom->z;
  strncpy( lat->atoms[i].element, temp_atom->element, NAMSZ );
}

coord_t *lattice_get_surface_normal(const lattice_t *lat, const coord_t *a1,
                                    const coord_t *a2, const coord_t *a3)
{
  double h = lat->vec_h;
  double k = lat->vec_k;
  double l = lat->vec_l;


  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_get_surface_normal): malloc");
  #endif
  
  coord_t *normal = (coord_t*) malloc(sizeof(coord_t));
  
  if (normal == NULL) return NULL;

  #if DEBUG > 0
  fprintf(stderr, " - success \n");
  #endif
  
  normal = coord_init();

  /* h, k, l not all 0 */
  if( (fabs(h) > TOLERANCE) || 
      (fabs(k) > TOLERANCE) || 
      (fabs(l) > TOLERANCE) )
  { 
    normal->x = (h*(a2->y*a3->z - a2->z*a3->y) + 
                 k*(a3->y*a1->z - a3->z*a1->y) + 
                 l*(a1->y*a2->z - a1->z*a2->y) ); 
    
    normal->y = (h*(a2->z*a3->x - a2->x*a3->z) + 
                 k*(a3->z*a1->x - a3->x*a1->z) + 
                 l*(a1->z*a2->x - a1->x*a2->z) );
    
    normal->z = (h*(a2->x*a3->y - a2->y*a3->x) + 
                 k*(a3->x*a1->y - a3->y*a1->x) + 
                 l*(a1->x*a2->y - a1->y*a2->x) ); 
  }
  else /* h, k, l all 0 */
  {
    fprintf(stderr, "*** error (latt): all three Miller indices are zero.\n");
    exit(1);
  }

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_get_surface_normal): return\n");
  #endif
  
  return (normal);
}

static int lattice_setup_fcc(lattice_t *lat, coord_t *a1, coord_t *a2,
                            coord_t *a3, coord_t *nor, coord_t **bas,
                            char **bas_name, size_t *n_bas)
{
  lattice_copy_element_name(*bas_name, lat->atoms[0].element);
  *n_bas = 1;
  double a = lat->a_latt;
  coord_set(a1, 0.5 * a, 0., 0.5 * a);
  coord_set(a2, 0.5 * a, 0.5 * a, 0.);
  coord_set(a3, 0., 0.5 * a, 0.5 * a);
  lat->a_nn = a / sqrt(2.);
  coord_set(nor, lat->vec_h * a, lat->vec_k * a, lat->vec_l * a);
  return 0;
}

static int lattice_setup_hcp(lattice_t *lat, coord_t *a1, coord_t *a2,
                            coord_t *a3, coord_t *nor, coord_t **bas,
                            char **bas_name, size_t *n_bas)
{
  lattice_copy_element_name(*bas_name, lat->atoms[0].element);
  lattice_copy_element_name(*bas_name + NAMSZ, lat->atoms[0].element);
  *n_bas = 2;
  if (lattice_resize_basis(bas, bas_name, *n_bas) != 0)
  {
    return -1;
  }

  coord_set(&(*bas)[0], 0., 0., 0.);
  coord_set(&(*bas)[1], 0., 0., 0.);
  double a = lat->a_latt;
  double c = lat->c_latt;
  if (fabs(c) < TOLERANCE)
  {
    lat->c_latt = 1.59 * a;
    c = lat->c_latt;
  }

  coord_set(a1, 0.5 * a, -sqrt(0.75) * a, 0.);
  coord_set(a2, 0.5 * a, sqrt(0.75) * a, 0.);
  coord_set(a3, 0., 0., c);
  coord_set(&(*bas)[1], 0.0, sqrt(1. / 3.) * a, 0.5 * c);
  lat->a_nn = a;
  coord_set(nor, sqrt(0.75) * c * (lat->vec_k + lat->vec_h),
            0.5 * c * (lat->vec_k - lat->vec_h),
            sqrt(0.75) * a * lat->vec_l);
  return 0;
}

static int lattice_setup_bcc(lattice_t *lat, coord_t *a1, coord_t *a2,
                            coord_t *a3, coord_t *nor, coord_t **bas,
                            char **bas_name, size_t *n_bas)
{
  lattice_copy_element_name(*bas_name, lat->atoms[0].element);
  *n_bas = 1;
  double a = lat->a_latt;
  coord_set(a1, a, 0., 0.);
  coord_set(a2, 0., a, 0.);
  coord_set(a3, 0.5 * a, 0.5 * a, 0.5 * a);
  lat->a_nn = a * sqrt(0.75);
  coord_set(nor, lat->vec_h * a, lat->vec_k * a, lat->vec_l * a);
  return 0;
}

static int lattice_setup_dia(lattice_t *lat, coord_t *a1, coord_t *a2,
                            coord_t *a3, coord_t *nor, coord_t **bas,
                            char **bas_name, size_t *n_bas)
{
  lattice_copy_element_name(*bas_name, lat->atoms[0].element);
  lattice_copy_element_name(*bas_name + NAMSZ, lat->atoms[0].element);
  *n_bas = 2;
  if (lattice_resize_basis(bas, bas_name, *n_bas) != 0)
  {
    return -1;
  }

  coord_set(&(*bas)[0], 0., 0., 0.);
  coord_set(&(*bas)[1], 0.25 * lat->a_latt, 0.25 * lat->a_latt, 0.25 * lat->a_latt);
  if (fabs(lat->c_latt) < TOLERANCE)
  {
    lat->c_latt = 1.59 * lat->a_latt;
  }

  coord_set(a1, 0.5 * lat->a_latt, 0.0, 0.5 * lat->a_latt);
  coord_set(a2, 0.5 * lat->a_latt, 0.5 * lat->a_latt, 0.0);
  coord_set(a3, 0.0, 0.5 * lat->a_latt, 0.5 * lat->a_latt);
  lat->a_nn = lat->a_latt * sqrt(0.375);
  coord_set(nor, lat->vec_h * lat->a_latt, lat->vec_k * lat->a_latt,
            lat->vec_l * lat->a_latt);
  return 0;
}

static bool lattice_miller_indices_valid(const lattice_t *lat)
{
  return !(fabs(lat->vec_h) < TOLERANCE &&
           fabs(lat->vec_k) < TOLERANCE &&
           fabs(lat->vec_l) < TOLERANCE);
}

int lattice_setup(lattice_t *lat, coord_t *a1, coord_t *a2,
        coord_t *a3, coord_t *nor, coord_t **bas, char **bas_name, size_t *n_bas)
{
  int status = 0;

  switch (lat->latt_type)
  {
    case LAT_FCC:
      status = lattice_setup_fcc(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
      break;
    case LAT_HCP:
      status = lattice_setup_hcp(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
      break;
    case LAT_BCC:
      status = lattice_setup_bcc(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
      break;
    case LAT_DIA:
      status = lattice_setup_dia(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
      break;
    case LAT_INP:
      status = lattice_read(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
      break;
    default:
      fprintf(stderr, "*** error (lattice_setup): bad lattice type or input file\n");
      return -1;
  }

  if (status != 0)
  {
    return status;
  }

  if (!lattice_miller_indices_valid(lat))
  {
    fprintf(stderr, "***error (lattice_setup): all three Miller indices are zero.\n");
    return -1;
  }

  #if (DEBUG == 1)
  fprintf(stderr, "** debug (lattice_setup): finished\n");
  #endif

  return 0;
}

miller_hkl_t *lattice_get_miller_hkl(const lattice_t *lat)
{
  miller_hkl_t *hkl;
  if (! (hkl = (miller_hkl_t*) malloc(sizeof(miller_hkl_t))))
  {
    return NULL;
  }
  
  hkl->h = lat->vec_h;
  hkl->k = lat->vec_k;
  hkl->l = lat->vec_l;
  
  return (hkl);
}

int lattice_read(lattice_t *lat, coord_t *a1, coord_t *a2, coord_t *a3,
                 coord_t *nor, coord_t **bas, char **bas_name, size_t *n_bas)
{
  FILE *inp_stream = stdin;
  size_t i_bas = 0;
  coord_t b1, b2;
  double R[2][2] = {{1., 0.}, {0., 1.}};
  char line_buffer[STRSZ];
  coord_t *surface_normal = NULL;

  if (strcmp(lat->input_filename, "stdin") != 0)
  {
    if ((inp_stream = fopen(lat->input_filename, "r")) == NULL)
    {
      fprintf(stderr, "*** error (lattice_read): open failed %s\n",
              lat->input_filename);
      return -1;
    }
    fprintf(inf_stream, "Read lattice input from file \"%s\" \n",
            lat->input_filename);
    fprintf(ctr_stream, "Read lattice input from file \"%s\" \n",
            lat->input_filename);
  }

  *n_bas = (size_t)MAX_INP_ATOMS;
  if (lattice_resize_basis(bas, bas_name, *n_bas) != 0)
  {
    fprintf(stderr, "*** error (lattice_read): allocation failed\n");
    return -1;
  }

  while ((fgets(line_buffer, STRSZ, inp_stream) != NULL) &&
         (i_bas < *n_bas))
  {
    lattice_log_line(line_buffer);

    if (lattice_parse_coord_line(line_buffer, "a1:", a1))
    {
      continue;
    }
    if (lattice_parse_coord_line(line_buffer, "a2:", a2))
    {
      continue;
    }
    if (lattice_parse_coord_line(line_buffer, "a3:", a3))
    {
      continue;
    }
    if (lattice_parse_matrix_line(line_buffer, "m1:", &R[0][0], &R[0][1]))
    {
      continue;
    }
    if (lattice_parse_matrix_line(line_buffer, "m2:", &R[1][0], &R[1][1]))
    {
      continue;
    }
    if (lattice_parse_atom_line(line_buffer, "pb:",
                                (*bas_name) + (i_bas * NAMSZ), &(*bas)[i_bas]))
    {
      i_bas++;
      continue;
    }
    if (lattice_parse_atom_line(line_buffer, "po:",
                                (*bas_name) + (i_bas * NAMSZ), &(*bas)[i_bas]))
    {
      if (lat->max_layers == 0)
      {
        lat->max_layers = 1;
      }
      i_bas++;
      continue;
    }
    if (lattice_parse_double_line(line_buffer, "il:", &lat->image_len))
    {
      continue;
    }
    if (lattice_parse_size_t_line(line_buffer, "nl:", &lat->max_layers))
    {
      continue;
    }
    if (lattice_parse_size_t_line(line_buffer, "uc:", &lat->max_cells))
    {
      continue;
    }
    if (lattice_is_quit_command(line_buffer))
    {
      break;
    }
    if (lattice_is_help_command(line_buffer) &&
        strcmp(lat->input_filename, "stdin") == 0)
    {
      lattice_print_help();
    }
  }

  *n_bas = i_bas;

  b1.x = R[0][0] * a1->x + R[0][1] * a2->x;
  b1.y = R[0][0] * a1->y + R[0][1] * a2->y;

  b2.x = R[1][0] * a1->x + R[1][1] * a2->x;
  b2.y = R[1][0] * a1->y + R[1][1] * a2->y;

  a1->x = b1.x;
  a1->y = b1.y;

  a2->x = b2.x;
  a2->y = b2.y;

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): update basis vectors\n");
  fprintf(stderr, "a1 = ");
  coord_printf(stderr, a1);
  fprintf(stderr, "a2 = ");
  coord_printf(stderr, a2);
  fprintf(stderr, "b1 = ");
  coord_printf(stderr, &b1);
  fprintf(stderr, "b2 = ");
  coord_printf(stderr, &b2);
  #endif

  lat->a_latt = COORD_MAGNITUDE(a1);
  lat->b_latt = COORD_MAGNITUDE(a2);
  lat->c_latt = COORD_MAGNITUDE(a3);

  lat->a_nn = lat->a_latt;
  if (lat->b_latt < lat->a_nn)
  {
    lat->a_nn = lat->b_latt;
  }
  if (lat->c_latt < lat->a_nn)
  {
    lat->a_nn = lat->c_latt;
  }

  lattice_normalize_basis(*bas, *n_bas);

  #if DEBUG > 0
  for (size_t index = 0; index < *n_bas; ++index)
  {
    fprintf(stderr, "  bas[%zu] = ", index);
    coord_printf(stderr, &(*bas)[index]);
  }
  fprintf(stderr, "** debug (lattice_read): end basis assignment\n");
  #endif

  surface_normal = lattice_get_surface_normal(lat, a1, a2, a3);
  if (surface_normal)
  {
    coord_set(nor, surface_normal->x, surface_normal->y, surface_normal->z);
    coord_free(surface_normal);
  }

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): n_bas = %zu\n", *n_bas);
  #endif

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): end of lattice_read\n");
  #endif

  return 0;
}
