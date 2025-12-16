#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <stdbool.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>
#include "lattice.h"

/**
 * @brief Scratch context for parsing a lattice input stream.
 *
 * The parser reads a line-oriented command format (e.g. "a1:", "pb:", "il:")
 * and updates the provided lattice and output buffers. This struct keeps the
 * function signatures small and makes the dispatch table-driven to keep
 * cyclomatic complexity low (Codacy thresholds).
 */
typedef struct lattice_read_ctx
{
  lattice_t *lat;
  coord_t *a1;
  coord_t *a2;
  coord_t *a3;
  coord_t *nor;
  coord_t **bas;
  char **bas_name;
  size_t *n_bas;
  size_t *i_bas;
  double (*R)[2];
} lattice_read_ctx_t;

typedef bool (*lattice_line_handler_t)(lattice_read_ctx_t *ctx, const char *payload);

static size_t lattice_line_length(const char *line);
static int lattice_open_input_stream(const lattice_t *lat, FILE **stream,
                                     bool *should_close);
static int lattice_prepare_basis_buffers(coord_t **bas, char **bas_name,
                                         size_t *n_bas);
static void lattice_apply_superstructure(coord_t *a1, coord_t *a2,
                                         const double R[2][2]);
static void lattice_update_constants(lattice_t *lat, const coord_t *a1,
                                     const coord_t *a2, const coord_t *a3);
static void lattice_update_nearest_neighbour(lattice_t *lat);
static void lattice_update_surface_normal(lattice_t *lat, const coord_t *a1,
                                          const coord_t *a2, const coord_t *a3,
                                          coord_t *nor);

static bool lattice_handle_a1(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_a2(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_a3(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_m1(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_m2(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_pb(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_po(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_il(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_nl(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_handle_uc(lattice_read_ctx_t *ctx, const char *payload);
static bool lattice_dispatch_prefixed(lattice_read_ctx_t *ctx,
                                      const char *line,
                                      size_t line_len);

typedef enum lattice_line_result
{
  LATTICE_LINE_CONTINUE = 0,
  LATTICE_LINE_STOP = 1
} lattice_line_result_t;

static lattice_line_result_t lattice_process_input_line(lattice_read_ctx_t *ctx,
                                                        const char *line);
static void lattice_read_input_stream(lattice_read_ctx_t *ctx, FILE *stream,
                                      char *line_buffer);

/**
 * @brief Resize the basis buffers used by @ref lattice_read.
 *
 * @param bas Pointer to the basis coordinate array.
 * @param bas_name Pointer to the basis element-name buffer (NAMSZ bytes per entry).
 * @param n_bas New number of basis entries.
 * @return 0 on success, non-zero on allocation failure.
 */
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

/**
 * @brief Copy an element name into a fixed-size destination buffer.
 *
 * The destination buffer is always NUL-terminated (when non-NULL).
 *
 * @param dest Destination buffer (at least @ref NAMSZ bytes).
 * @param element Source string (may be NULL).
 */
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

/**
 * @brief Return the bounded length of an input line buffer.
 *
 * Uses @ref STRSZ as the maximum scan length so callers never rely on
 * unbounded traversal of potentially malformed buffers.
 */
static size_t lattice_line_length(const char *line)
{
  if (!line)
  {
    return 0;
  }
  return strnlen(line, STRSZ);
}

static int lattice_open_input_stream(const lattice_t *lat, FILE **stream,
                                     bool *should_close)
{
  if (!lat || !stream || !should_close)
  {
    return -1;
  }

  *stream = stdin;
  *should_close = false;

  if (strcmp(lat->input_filename, "stdin") == 0)
  {
    return 0;
  }

  FILE *handle = fopen(lat->input_filename, "r");
  if (!handle)
  {
    fprintf(stderr, "*** error (lattice_read): open failed %s\n",
            lat->input_filename);
    return -1;
  }

  *stream = handle;
  *should_close = true;
  fprintf(inf_stream, "Read lattice input from file \"%s\" \n",
          lat->input_filename);
  fprintf(ctr_stream, "Read lattice input from file \"%s\" \n",
          lat->input_filename);
  return 0;
}

static int lattice_prepare_basis_buffers(coord_t **bas, char **bas_name,
                                         size_t *n_bas)
{
  if (!bas || !bas_name || !n_bas)
  {
    return -1;
  }

  *n_bas = (size_t)MAX_INP_ATOMS;
  if (lattice_resize_basis(bas, bas_name, *n_bas) != 0)
  {
    fprintf(stderr, "*** error (lattice_read): allocation failed\n");
    return -1;
  }
  return 0;
}

static void lattice_apply_superstructure(coord_t *a1, coord_t *a2,
                                         const double R[2][2])
{
  coord_t b1, b2;
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
}

static void lattice_update_constants(lattice_t *lat, const coord_t *a1,
                                     const coord_t *a2, const coord_t *a3)
{
  lat->a_latt = COORD_MAGNITUDE(a1);
  lat->b_latt = COORD_MAGNITUDE(a2);
  lat->c_latt = COORD_MAGNITUDE(a3);
}

static void lattice_update_nearest_neighbour(lattice_t *lat)
{
  lat->a_nn = lat->a_latt;
  if (lat->b_latt < lat->a_nn)
  {
    lat->a_nn = lat->b_latt;
  }
  if (lat->c_latt < lat->a_nn)
  {
    lat->a_nn = lat->c_latt;
  }
}

static void lattice_update_surface_normal(lattice_t *lat, const coord_t *a1,
                                          const coord_t *a2, const coord_t *a3,
                                          coord_t *nor)
{
  coord_t *surface_normal = lattice_get_surface_normal(lat, a1, a2, a3);
  if (surface_normal)
  {
    coord_set(nor, surface_normal->x, surface_normal->y, surface_normal->z);
    coord_free(surface_normal);
  }

  #if DEBUG > 0
  fprintf(stderr, "** debug (lattice_read): end of lattice_read\n");
  #endif
}

/**
 * @brief Compare a line to a command token, allowing trailing newlines.
 *
 * @param line Line buffer as read by fgets.
 * @param value Token to compare against (e.g. "help").
 * @return true when the line matches exactly (ignoring newline), false otherwise.
 */
static bool lattice_line_equals(const char *line, const char *value)
{
  if (!line || !value)
  {
    return false;
  }

  size_t line_len = lattice_line_length(line);
  if (line_len == STRSZ)
  {
    return false;
  }

  size_t len = strnlen(value, STRSZ);
  if (len == STRSZ)
  {
    return false;
  }
  if (line_len < len)
  {
    return false;
  }
  if (memcmp(line, value, len) != 0)
  {
    return false;
  }

  char after = line[len];
  return after == '\0' || strchr("\r\n", after) != NULL;
}

/**
 * @brief Forward input lines to the diagnostic streams.
 *
 * This preserves historical behaviour where lattice input is echoed to
 * info/trace streams during parsing.
 */
static void lattice_log_line(const char *line)
{
  fprintf(inf_stream, "%s", line);
#if DEBUG == 1
  fprintf(ctr_stream, "%s", line);
#endif
}

/**
 * @brief Print interactive help for stdin-driven lattice input.
 */
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

/**
 * @brief Translate basis coordinates so the maximum z becomes 0.
 *
 * The lattice input format expects that subsequent operations work with a
 * basis that has been shifted such that all z coordinates are <= 0.
 */
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

/**
 * @brief True when a line begins with the quit command token.
 */
static bool lattice_is_quit_command(const char *line)
{
  return line[0] == 'q';
}

/**
 * @brief True when a line equals the help command token.
 */
static bool lattice_is_help_command(const char *line)
{
  return lattice_line_equals(line, "help");
}

static lattice_line_result_t lattice_process_input_line(lattice_read_ctx_t *ctx,
                                                        const char *line)
{
  size_t line_len = lattice_line_length(line);
  if (line_len == STRSZ || line_len == 0)
  {
    return LATTICE_LINE_CONTINUE;
  }

  if (lattice_is_quit_command(line))
  {
    return LATTICE_LINE_STOP;
  }

  if (lattice_is_help_command(line) &&
      strcmp(ctx->lat->input_filename, "stdin") == 0)
  {
    lattice_print_help();
    return LATTICE_LINE_CONTINUE;
  }

  (void) lattice_dispatch_prefixed(ctx, line, line_len);
  return LATTICE_LINE_CONTINUE;
}

static void lattice_read_input_stream(lattice_read_ctx_t *ctx, FILE *stream,
                                      char *line_buffer)
{
  while ((fgets(line_buffer, STRSZ, stream) != NULL) &&
         (*ctx->i_bas < *ctx->n_bas))
  {
    lattice_log_line(line_buffer);

    if (lattice_process_input_line(ctx, line_buffer) == LATTICE_LINE_STOP)
    {
      break;
    }
  }
}

/**
 * @brief Parse an "a1:" line payload into the first lattice vector.
 */
static bool lattice_handle_a1(lattice_read_ctx_t *ctx, const char *payload)
{
  double x = 0., y = 0., z = 0.;
  if (sscanf(payload, "%lf %lf %lf", &x, &y, &z) != 3)
  {
    return false;
  }
  coord_set(ctx->a1, x, y, z);
  return true;
}

/**
 * @brief Parse an "a2:" line payload into the second lattice vector.
 */
static bool lattice_handle_a2(lattice_read_ctx_t *ctx, const char *payload)
{
  double x = 0., y = 0., z = 0.;
  if (sscanf(payload, "%lf %lf %lf", &x, &y, &z) != 3)
  {
    return false;
  }
  coord_set(ctx->a2, x, y, z);
  return true;
}

/**
 * @brief Parse an "a3:" line payload into the third lattice vector.
 */
static bool lattice_handle_a3(lattice_read_ctx_t *ctx, const char *payload)
{
  double x = 0., y = 0., z = 0.;
  if (sscanf(payload, "%lf %lf %lf", &x, &y, &z) != 3)
  {
    return false;
  }
  coord_set(ctx->a3, x, y, z);
  return true;
}

/**
 * @brief Parse an "m1:" superstructure matrix row.
 */
static bool lattice_handle_m1(lattice_read_ctx_t *ctx, const char *payload)
{
  return sscanf(payload, "%lf %lf", &ctx->R[0][0], &ctx->R[0][1]) == 2;
}

/**
 * @brief Parse an "m2:" superstructure matrix row.
 */
static bool lattice_handle_m2(lattice_read_ctx_t *ctx, const char *payload)
{
  return sscanf(payload, "%lf %lf", &ctx->R[1][0], &ctx->R[1][1]) == 2;
}

/**
 * @brief Parse a "pb:"/"po:" payload into an element name and position.
 */
static bool lattice_parse_atom_payload(const char *payload, char *name, coord_t *position)
{
  if (!payload || !name || !position)
  {
    return false;
  }
  return sscanf(payload, " %255s %lf %lf %lf", name,
                &position->x, &position->y, &position->z) == 4;
}

/**
 * @brief Parse a "pb:" basis line and append it to the basis arrays.
 */
static bool lattice_handle_pb(lattice_read_ctx_t *ctx, const char *payload)
{
  if (!ctx || !ctx->bas || !ctx->bas_name || !ctx->n_bas || !ctx->i_bas)
  {
    return false;
  }
  if (*ctx->i_bas >= *ctx->n_bas)
  {
    return false;
  }

  coord_t position = {.x = 0., .y = 0., .z = 0.};
  char element[STRSZ];
  if (!lattice_parse_atom_payload(payload, element, &position))
  {
    return false;
  }

  (*ctx->bas)[*ctx->i_bas] = position;
  lattice_copy_element_name((*ctx->bas_name) + (*ctx->i_bas * NAMSZ), element);
  (*ctx->i_bas)++;
  return true;
}

/**
 * @brief Parse a "po:" overlayer line and append it to the basis arrays.
 *
 * Historically, reading an overlayer atom implies at least one layer.
 */
static bool lattice_handle_po(lattice_read_ctx_t *ctx, const char *payload)
{
  if (!ctx || !ctx->lat)
  {
    return false;
  }
  if (ctx->lat->max_layers == 0)
  {
    ctx->lat->max_layers = 1;
  }
  return lattice_handle_pb(ctx, payload);
}

/**
 * @brief Parse an "il:" image length line.
 */
static bool lattice_handle_il(lattice_read_ctx_t *ctx, const char *payload)
{
  char *endptr = NULL;
  double result = strtod(payload, &endptr);
  if (endptr == payload)
  {
    return false;
  }
  ctx->lat->image_len = result;
  return true;
}

/**
 * @brief Parse an "nl:" max layers line.
 */
static bool lattice_handle_nl(lattice_read_ctx_t *ctx, const char *payload)
{
  char *endptr = NULL;
  unsigned long long result = strtoull(payload, &endptr, 10);
  if (endptr == payload)
  {
    return false;
  }
  ctx->lat->max_layers = (size_t)result;
  return true;
}

/**
 * @brief Parse a "uc:" max cells line.
 */
static bool lattice_handle_uc(lattice_read_ctx_t *ctx, const char *payload)
{
  char *endptr = NULL;
  unsigned long long result = strtoull(payload, &endptr, 10);
  if (endptr == payload)
  {
    return false;
  }
  ctx->lat->max_cells = (size_t)result;
  return true;
}

/**
 * @brief Dispatch prefixed lattice commands to small handler functions.
 *
 * The dispatch is table-driven to keep parsing logic readable and maintainable
 * while meeting static analysis complexity thresholds.
 */
static bool lattice_dispatch_prefixed(lattice_read_ctx_t *ctx,
                                      const char *line,
                                      size_t line_len)
{
  if (!ctx || !line)
  {
    return false;
  }
  if (line_len == STRSZ || line_len < 3)
  {
    return false;
  }

  typedef struct lattice_command_spec
  {
    char token[4];
    lattice_line_handler_t handler;
  } lattice_command_spec_t;

  static const lattice_command_spec_t commands[] = {
      {"a1:", lattice_handle_a1},
      {"a2:", lattice_handle_a2},
      {"a3:", lattice_handle_a3},
      {"m1:", lattice_handle_m1},
      {"m2:", lattice_handle_m2},
      {"pb:", lattice_handle_pb},
      {"po:", lattice_handle_po},
      {"il:", lattice_handle_il},
      {"nl:", lattice_handle_nl},
      {"uc:", lattice_handle_uc},
  };

  for (size_t index = 0; index < (sizeof(commands) / sizeof(commands[0])); ++index)
  {
    if (memcmp(line, commands[index].token, 3) == 0)
    {
      return commands[index].handler(ctx, line + 3);
    }
  }

  return false;
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
    lat->atoms[i_atom].element = NULL;
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

/**
 * @brief Initialize lattice vectors, basis, and surface normal from lattice type.
 *
 * For built-in lattice types (FCC/HCP/BCC/DIA) this function computes the
 * primitive vectors and nearest-neighbour distance. For @ref LAT_INP it reads
 * an external lattice definition via @ref lattice_read.
 *
 * @param lat Lattice configuration and output values.
 * @param a1 Output first lattice vector.
 * @param a2 Output second lattice vector.
 * @param a3 Output third lattice vector.
 * @param nor Output surface normal.
 * @param bas Output basis coordinate array (reallocated as required).
 * @param bas_name Output basis name buffer (reallocated as required).
 * @param n_bas Output number of basis atoms.
 * @return 0 on success, non-zero on error.
 */
int lattice_setup(lattice_t *lat, coord_t *a1, coord_t *a2,
        coord_t *a3, coord_t *nor, coord_t **bas, char **bas_name, size_t *n_bas)
{
  typedef int (*lattice_setup_impl_t)(lattice_t *lat, coord_t *a1, coord_t *a2,
                                      coord_t *a3, coord_t *nor, coord_t **bas,
                                      char **bas_name, size_t *n_bas);
  static const lattice_setup_impl_t impls[] = {
      NULL,
      lattice_setup_fcc,
      lattice_setup_hcp,
      lattice_setup_bcc,
      lattice_setup_dia,
  };
  lattice_setup_impl_t impl = NULL;
  int status = 0;

  if (lat->latt_type >= LAT_FCC && lat->latt_type <= LAT_DIA)
  {
    impl = impls[lat->latt_type];
  }
  else if (lat->latt_type == LAT_INP)
  {
    return lattice_read(lat, a1, a2, a3, nor, bas, bas_name, n_bas);
  }
  else
  {
    fprintf(stderr, "*** error (lattice_setup): bad lattice type or input file\n");
    return -1;
  }

  if (!impl)
  {
    fprintf(stderr, "*** error (lattice_setup): unsupported lattice type\n");
    return -1;
  }

  status = impl(lat, a1, a2, a3, nor, bas, bas_name, n_bas);

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

/**
 * @brief Read a lattice definition from a file or stdin.
 *
 * The expected format is line-oriented with commands:
 * - Lattice vectors: "a1:", "a2:", "a3:"
 * - Superstructure matrix: "m1:", "m2:"
 * - Basis entries: "pb:" (bulk) and "po:" (overlayer)
 * - Scalars: "il:" (image length), "nl:" (max layers), "uc:" (max cells)
 * - Control: "q" to stop reading, "help" for interactive help (stdin only)
 *
 * After parsing, the function applies the superstructure matrix to a1/a2,
 * updates lattice constants, normalizes the basis (max z -> 0), and computes
 * the surface normal from the Miller indices and lattice vectors.
 *
 * @return 0 on success, non-zero on error.
 */
int lattice_read(lattice_t *lat, coord_t *a1, coord_t *a2, coord_t *a3,
                 coord_t *nor, coord_t **bas, char **bas_name, size_t *n_bas)
{
  FILE *inp_stream = NULL;
  bool should_close = false;
  size_t i_bas = 0;
  double R[2][2] = {{1., 0.}, {0., 1.}};
  char line_buffer[STRSZ];

  if (lattice_open_input_stream(lat, &inp_stream, &should_close) != 0)
  {
    return -1;
  }

  if (lattice_prepare_basis_buffers(bas, bas_name, n_bas) != 0)
  {
    if (should_close) fclose(inp_stream);
    return -1;
  }

  lattice_read_ctx_t ctx = {.lat = lat,
                            .a1 = a1,
                            .a2 = a2,
                            .a3 = a3,
                            .nor = nor,
                            .bas = bas,
                            .bas_name = bas_name,
                            .n_bas = n_bas,
                            .i_bas = &i_bas,
                            .R = R};

  lattice_read_input_stream(&ctx, inp_stream, line_buffer);
  if (should_close) fclose(inp_stream);

  *n_bas = i_bas;
  lattice_apply_superstructure(a1, a2, R);
  lattice_update_constants(lat, a1, a2, a3);
  lattice_update_nearest_neighbour(lat);
  lattice_normalize_basis(*bas, *n_bas);
  lattice_update_surface_normal(lat, a1, a2, a3, nor);
  return 0;
}
