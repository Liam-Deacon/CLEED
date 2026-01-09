/*********************************************************************
 *                    RFAC_COMPAT.C
 *
 * Minimal compatibility helpers for cleed++.
 * These implementations are intentionally small and focus on enabling
 * the C++ bindings to build. They should be replaced with the full
 * rfac API from develop in a follow-up.
 *********************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "rfac.h"

static int rfac_iv_compare_energy(const void *lhs, const void *rhs)
{
  const rfac_iv_data *a = (const rfac_iv_data *)lhs;
  const rfac_iv_data *b = (const rfac_iv_data *)rhs;

  if (a->energy < b->energy) return -1;
  if (a->energy > b->energy) return 1;
  return 0;
}

rfac_iv *rfac_iv_alloc(size_t n_eng)
{
  if (n_eng == 0) return NULL;

  rfac_iv *iv = (rfac_iv *)calloc(1, sizeof(rfac_iv));
  if (!iv) return NULL;

  iv->data = (rfac_iv_data *)calloc(n_eng, sizeof(rfac_iv_data));
  if (!iv->data) {
    free(iv);
    return NULL;
  }

  iv->n_eng = n_eng;
  iv->equidist = false;
  iv->sort = false;
  iv->smooth = false;
  iv->spline = false;

  return iv;
}

void rfac_iv_free(rfac_iv *iv)
{
  if (!iv) return;
  free(iv->data);
  free(iv);
}

void rfac_iv_copy(rfac_iv *dest, const rfac_iv *src)
{
  if (!dest || !src) return;

  free(dest->data);
  dest->data = NULL;
  *dest = *src;

  if (src->n_eng == 0 || !src->data) return;

  dest->data = (rfac_iv_data *)calloc(src->n_eng, sizeof(rfac_iv_data));
  if (!dest->data) {
    dest->n_eng = 0;
    return;
  }

  memcpy(dest->data, src->data, src->n_eng * sizeof(rfac_iv_data));
}

rfac_iv *rfac_iv_read(const char *filename)
{
  if (!filename) return NULL;

  FILE *file = fopen(filename, "r");
  if (!file) return NULL;

  size_t cap = 0;
  size_t count = 0;
  rfac_iv_data *data = NULL;
  char line[256];

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
      continue;
    }

    double energy = 0.0;
    double intensity = 0.0;
    double deriv2 = 0.0;
    int parsed = sscanf(line, "%lf %lf %lf", &energy, &intensity, &deriv2);
    if (parsed < 2) {
      continue;
    }

    if (count == cap) {
      size_t next = cap ? cap * 2 : 64;
      rfac_iv_data *next_data =
          (rfac_iv_data *)realloc(data, next * sizeof(rfac_iv_data));
      if (!next_data) {
        free(data);
        fclose(file);
        return NULL;
      }
      data = next_data;
      cap = next;
    }

    data[count].energy = (real)energy;
    data[count].intens = (real)intensity;
    data[count].deriv2 = (parsed == 3) ? (real)deriv2 : 0.0;
    count++;
  }

  fclose(file);

  if (count == 0) {
    free(data);
    return NULL;
  }

  rfac_iv *iv = rfac_iv_alloc(count);
  if (!iv) {
    free(data);
    return NULL;
  }

  memcpy(iv->data, data, count * sizeof(rfac_iv_data));
  free(data);

  iv->n_eng = count;
  iv->first_eng = iv->data[0].energy;
  iv->last_eng = iv->data[count - 1].energy;
  iv->max_int = iv->data[0].intens;
  for (size_t i = 1; i < count; i++) {
    if (iv->data[i].intens > iv->max_int) {
      iv->max_int = iv->data[i].intens;
    }
  }

  return iv;
}

rfac_iv *rfac_iv_read_cleed(rfac_ivcur *iv_cur, char *buffer, char *indices)
{
  (void)iv_cur;
  (void)buffer;
  (void)indices;
  errno = ENOSYS;
  return NULL;
}

int rfac_iv_sort(rfac_iv *iv)
{
  if (!iv || !iv->data || iv->n_eng == 0) return -1;
  qsort(iv->data, iv->n_eng, sizeof(rfac_iv_data), rfac_iv_compare_energy);
  iv->sort = true;
  iv->first_eng = iv->data[0].energy;
  iv->last_eng = iv->data[iv->n_eng - 1].energy;
  return 0;
}

int rfac_iv_spline(rfac_iv *iv)
{
  if (!iv) return -1;
  iv->spline = true;
  return 0;
}

int rfac_iv_lorentz_smooth(rfac_iv *iv, real vi)
{
  (void)vi;
  if (!iv) return -1;
  iv->smooth = true;
  return 0;
}

rfac_ivcur *rfac_ivcur_read(const char *control_file, const char *theory_file)
{
  (void)control_file;
  (void)theory_file;

  rfac_ivcur *ivcur = (rfac_ivcur *)calloc(1, sizeof(rfac_ivcur));
  if (!ivcur) return NULL;

  ivcur->group_id = END_OF_GROUP_ID;
  return ivcur;
}

void rfac_ivcur_free_all(rfac_ivcur *ivcur)
{
  if (!ivcur) return;

  size_t idx = 0;
  while (ivcur[idx].group_id != END_OF_GROUP_ID) {
    rfac_iv_free(ivcur[idx].experimental);
    rfac_iv_free(ivcur[idx].theory);
    idx++;
  }
  free(ivcur);
}

rfac_args *rfac_rdargs(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  rfac_args *args = (rfac_args *)calloc(1, sizeof(rfac_args));
  if (!args) return NULL;

  args->s_ini = -10.0;
  args->s_fin = 10.0;
  args->s_step = 0.5;
  args->r_type = RP_FACTOR;
  args->vi = 4.0;
  args->all_groups = false;
  return args;
}

real rfac_rmin(rfac_ivcur *iv_cur, rfac_args *args,
               real *r_min_ptr, real *rr_ptr, real *shift_ptr)
{
  (void)iv_cur;
  (void)args;
  if (r_min_ptr) *r_min_ptr = 0.0;
  if (rr_ptr) *rr_ptr = 0.0;
  if (shift_ptr) *shift_ptr = 0.0;
  errno = ENOSYS;
  return 0.0;
}
