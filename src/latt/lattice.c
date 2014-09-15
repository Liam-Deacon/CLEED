#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "lattice.h"

void lattice_debug(const lattice_t *lat)
{
  printf("(h, k, l) = (%7.4f, %7.4f, %7.4f)\n", 
         lat->vec_h, lat->vec_k, lat->vec_l);
  printf("a_nn = %7.4f\n", lat->a_nn);
  printf("a_latt = %7.4f\n", lat->a_latt);
  printf("c_latt = %7.4f\n", lat->c_latt);
  printf("max_disp = %7.4f\n", lat->max_disp);
  printf("max_disp_z = %7.4f\n", lat->max_disp_z);
  printf("max_layers = %u\n", lat->max_layers);
  printf("max_cells = %u\n", lat->max_cells);
  printf("max_atoms = %u\n", lat->max_atoms);
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
  fprintf(output, "%d\n", lat->n_atoms);

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

void lattice_setup(lattice_t *lat, coord_t *a1, coord_t *a2, 
        coord_t *a3, coord_t *nor, coord_t *bas, char *bas_name, int *n_bas)
{
  
  *n_bas = 1;
  double a = lat->a_latt;
  double c = lat->c_latt;
  double h = lat->vec_h;
  double k = lat->vec_k;
  double l = lat->vec_l;
  
  if(lat->latt_type == LAT_FCC)
  {

    /********************************************************
                            LAT_FCC 
    ********************************************************/

	strncpy(bas_name, lat->atoms[0].element, NAMSZ);

    coord_set(a1, 0.5 * a, 0.0, 0.5 * a);
    coord_set(a2, 0.5 * a,0.5 * a, 0.0);
    coord_set(a3, 0.0, 0.5 * a, 0.5 * a);  
    
    lat->a_nn = a / sqrt(2.);

    /*** Find surface normal ***/
    coord_set(nor, h * a, k * a, l * a);

  } /* if fcc */

  else if(lat->latt_type == LAT_HCP)
  {
    /********************************************************
                        LAT_HCP 
    ********************************************************/

    *n_bas = 2;
    realloc((coord_t*) bas, sizeof(coord_t) * (*n_bas));
    realloc((char *) bas_name, *n_bas * NAMSZ * sizeof(char));
    
    strncpy(bas_name, lat->atoms[0].element, NAMSZ);
    strncpy(bas_name + NAMSZ, lat->atoms[0].element, NAMSZ);
    
    coord_set(&bas[0], 0., 0., 0.);
    coord_set(&bas[1], 0., 0., 0.);
    
    /* value for Ru */
    if (fabs(c) < TOLERANCE) 
    {
      lat->c_latt = 1.59 * a; 
      c = lat->c_latt;
    }

    coord_set(a1, 0.5 * a, -sqrt(0.75) * a, 0.);
    coord_set(a2, 0.5 * a,  sqrt(0.75) * a, 0.);
    coord_set(a3, 0., 0., c);

    coord_set(bas+sizeof(coord_t), 0.0, sqrt(1./3.) * a, 0.5 * c);

    lat->a_nn = a;

    /*** Find surface normal ***/
    coord_set(nor, sqrt(0.75) * c * (k + h), 0.5 * c * (k - h), sqrt(0.75) * a * l);
 
  } /* if hcp */

  else if(lat->latt_type == LAT_BCC)
  {
    /********************************************************
        LAT_BCC
    ********************************************************/
    strncpy (bas_name, lat->atoms[0].element, NAMSZ);

    coord_set(a1, a, 0., 0.);
    coord_set(a2, 0., a, 0.);
    coord_set(a3, 0.5 * a, 0.5 * a, 0.5 * a);

    lat->a_nn = a * sqrt(0.75);

    /*** Find surface normal ***/
    coord_set(nor, (h * a), (k * a), (l * a));

  } /* if bcc */

  else if(lat->latt_type == LAT_DIA)
  {
    /********************************************************
                        LAT_DIA
    ********************************************************/

    *n_bas = 2;
    realloc((coord_t*) bas, sizeof(coord_t) * (*n_bas));
    realloc((char *) bas_name, *n_bas * NAMSZ * sizeof(char));
    
    strncpy (bas_name, lat->atoms[0].element, NAMSZ);
    strncpy (bas_name + NAMSZ, lat->atoms[0].element, NAMSZ);
    
    /* value for Ru */
    if (fabs(lat->c_latt) < TOLERANCE) 
    {
      lat->c_latt = 1.59 * a;
      c = lat->c_latt;
    }
    
    coord_set(a1, 0.5 * a, 0.0, 0.5 * a);
    coord_set(a2, 0.5 * a, 0.5 * a, 0.0);
    coord_set(a3, 0.0, 0.5 * a, 0.5 * a);
    
    coord_set(&bas[0], 0., 0., 0.);
    coord_set(&bas[1], 0.25 * a, 0.25 * a, 0.25 * a);

    lat->a_nn = a * sqrt(0.375);

  } /* if dia */

  else if(lat->latt_type == LAT_INP)
  {
    /********************************************************
               LAT_INP: external lattice input
    ********************************************************/

    lattice_read(lat, a1, a2, a3, nor, bas, bas_name, &*n_bas);
    
    #if DEBUG > 0
    fprintf(stderr, "** debug (lattice_read): return\n");
    #endif
    
  } /* if LAT_INP */

/********************************************************
 ELSE
********************************************************/
  else
  {
    fprintf(stderr, "*** error (lattice_setup): "
            "bad lattice type or input file\n");
  }
  
  /* h, k, l all 0 */
  if( ( fabs(lat->vec_h - 0.) < (TOLERANCE * fabs(lat->vec_h)) ) && 
      ( fabs(lat->vec_k - 0.) < (TOLERANCE * fabs(lat->vec_k)) ) &&
      ( fabs(lat->vec_l - 0.) < (TOLERANCE * fabs(lat->vec_l)) ) )
  {
    fprintf(stderr, "***error (lattice_setup): all three Miller indices are zero.\n");
    exit(1);
  }
  
  #if (DEBUG == 1)
  fprintf(stderr, "** debug (lattice_setup): finished\n");
  #endif

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

void lattice_read(lattice_t *lat, coord_t *a1, coord_t *a2, coord_t *a3, 
                  coord_t *nor, coord_t *bas, char *bas_name, int *n_bas)
{

  FILE *inp_stream = stdin;
  coord_t faux = {.x=0., .y=0., .z=0.};
  size_t i_bas = 0;
  coord_t b1, b2;
  double R[2][2];
  char line_buffer[STRSZ];
  char faux_name[NAMSZ];
  
  /* Translate basis vectors such that all z are < 0. */
  faux.x = bas[0].x;
  faux.y = bas[0].y;
  faux.z = bas[0].z;
  if (bas_name != NULL)
  {
	strncpy(faux_name, bas_name, NAMSZ);
  }

  fprintf(stderr, "%s -> %s\n", bas_name, faux_name);

  if (strcmp(lat->input_filename, "stdin") != 0) 
  {
    if ((inp_stream = fopen(lat->input_filename, "r")) == NULL)
    {
      fprintf(stderr, "*** error (lattice_read): open failed %s\n", 
              lat->input_filename);
      exit(1);
    }
    fprintf(inf_stream, "Read lattice input from file \"%s\" \n",
            lat->input_filename);
    fprintf(ctr_stream, "Read lattice input from file \"%s\" \n", 
            lat->input_filename);
  }
  
  *n_bas = MAX_INP_ATOMS;
  
  realloc((coord_t*) bas, sizeof(coord_t) * (*n_bas));
  realloc((char *) bas_name, *n_bas * NAMSZ * sizeof(char));
  
  R[0][0] = R[1][1] = 1.;
  R[0][1] = R[1][0] = 0.;

  while((fgets(line_buffer, STRSZ, inp_stream) != NULL) && (i_bas < *n_bas))
  {
    
    fprintf(inf_stream, "%s", line_buffer);
    #if DEBUG == 1
    fprintf(ctr_stream, "%s", line_buffer);
    #endif 
    
    /* read lattice vectors and superstructure matrix */
    if( strncmp(line_buffer, "a1:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf %lf %lf", &a1->x, &a1->y, &a1->z);
    }
    if( strncmp(line_buffer, "a2:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf %lf %lf", &a2->x, &a2->y, &a2->z);
    }
    if( strncmp(line_buffer, "a3:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf %lf %lf", &a3->x, &a3->y, &a3->z);
    }
    if( strncmp(line_buffer, "m1:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf %lf", &R[0][0], &R[0][1]);
    }
    if( strncmp(line_buffer, "m2:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf %lf", &R[1][0], &R[1][1]);
     }

    /* read basis vectors */
    if( strncmp(line_buffer, "pb:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%s %lf %lf %lf", bas_name+(i_bas * NAMSZ),
    		  &bas[i_bas].x, &bas[i_bas].y, &bas[i_bas].z);
      i_bas ++;
    }

    if( strncmp(line_buffer, "po:", 3) == 0  )
    {
      if (lat->max_layers == 0) lat->max_layers = 1;
      sscanf(line_buffer+3, "%s %lf %lf %lf", bas_name+(i_bas * NAMSZ),
             &bas[i_bas].x, &bas[i_bas].y, &bas[i_bas].z);
      i_bas ++;
    }

    /* image dimensions */
    if( strncmp(line_buffer, "il:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%lf", &lat->image_len);
    }

    if( strncmp(line_buffer, "nl:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%u", &lat->max_layers);
    }

    if( strncmp(line_buffer, "uc:", 3) == 0 )
    {
      sscanf(line_buffer+3, "%u", &lat->max_cells);
    }
     
    if( strncmp(line_buffer, "q", 1) == 0 || line_buffer == NULL) break;
    
    if( strcmp(line_buffer, "help") == 0 && 
        strcmp(lat->input_filename, "stdin") == 0)
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

  } /* for  fgets */

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): end read\n");
  #endif
  
  *n_bas = i_bas;

  /* update lattice vectors with superstructure matrix */

  b1.x = R[0][0]*a1->x + R[0][1]*a2->x;
  b1.y = R[0][0]*a1->y + R[0][1]*a2->y;

  b2.x = R[1][0]*a1->x + R[1][1]*a2->x;
  b2.y = R[1][0]*a1->y + R[1][1]*a2->y;

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
  if(lat->b_latt < lat->a_nn) lat->a_nn = lat->b_latt;
  if(lat->c_latt < lat->a_nn) lat->a_nn = lat->c_latt;

  /*!TODO: fix initial assignment of faux */
  faux.x = 0.;
  faux.y = 0.;
  faux.z = 0.;

  for(i_bas = 1; i_bas < *n_bas; i_bas ++)
  {
    if(bas[i_bas].z > faux.z)
    {
      faux.x = bas[i_bas].x;
      faux.y = bas[i_bas].y;
      faux.z = bas[i_bas].z;
    }
  }

  for(i_bas = 0; i_bas < *n_bas; i_bas ++)
  {  
    bas[i_bas].z -= faux.x;
    bas[i_bas].y -= faux.y;
    bas[i_bas].z -= faux.z;
  }

  #if DEBUG > 0
  for(i_bas = 0; i_bas < *n_bas; i_bas ++)
  {
	  fprintf(stderr, "  bas[%u] = ", i_bas);
	  coord_printf(stderr, &bas[i_bas]);
  }
  fprintf(stderr, "** debug (lattice_read): end basis assignment\n");
  #endif
  
  /* Find surface normal */
  nor = lattice_get_surface_normal(lat, a1, a2, a3);

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): n_bas = %u\n", *n_bas);
  #endif
  
  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): end of lattice_read\n");
  #endif

}
