/*********************************************************************
 *                        SR_PARSE.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

/**
 * @file sr_parse.c
 * @brief Implementation of small parsing helpers shared by SEARCH utilities.
 *
 * These helpers centralise a few parsing idioms used by legacy SEARCH tools:
 * - skip whitespace
 * - ignore comment lines beginning with `#`
 * - parse numeric tokens robustly
 */

// cppcheck-suppress missingIncludeSystem
#include <errno.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "gh_stddef.h"
#include "sr_parse.h"

static const char *sr_skip_ws(const char *p)
{
  while (p && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
    p++;
  }
  return p;
}

static int sr_parse_double_token(const char *p, double *out, const char **out_end)
{
  if (p == NULL || out == NULL || out_end == NULL) return -1;

  char *end = NULL;
  errno = 0;
  double v = strtod(p, &end);
  if (end == p || errno != 0) return -1;

  *out = v;
  *out_end = end;
  return 0;
}

static int sr_parse_next_double(const char **p, double *out)
{
  if (p == NULL || out == NULL) return -1;
  const char *s = sr_skip_ws(*p);
  if (s == NULL) return -1;
  if (*s == '\0') return -1;
  if (*s == '#') return -1;

  const char *end = NULL;
  if (sr_parse_double_token(s, out, &end) != 0) return -1;
  *p = end;
  return 0;
}

/**
 * @brief Parse two numeric values from a line into @ref real outputs.
 *
 * See @ref sr_parse_two_reals for full semantics.
 */
int sr_parse_two_reals(const char *line, real *out_a, real *out_b)
{
  if (line == NULL || out_a == NULL || out_b == NULL) return -1;

  const char *p = line;
  double a = 0.0;
  double b = 0.0;
  if (sr_parse_next_double(&p, &a) != 0) return -1;
  if (sr_parse_next_double(&p, &b) != 0) return -1;

  *out_a = (real)a;
  *out_b = (real)b;
  return 0;
}

/**
 * @brief Read the first line containing two @ref real values from a file.
 *
 * See @ref sr_read_two_reals_from_file for full semantics.
 */
int sr_read_two_reals_from_file(const char *path, real *out_a, real *out_b)
{
  if (path == NULL || out_a == NULL || out_b == NULL) return -1;

  FILE *fp = fopen(path, "r");
  if (fp == NULL) return -1;

  char buf[STRSZ];
  int rc = -1;
  while (fgets(buf, (int)sizeof(buf), fp) != NULL) {
    if (sr_parse_two_reals(buf, out_a, out_b) == 0) {
      rc = 0;
      break;
    }
  }

  fclose(fp);
  return rc;
}
