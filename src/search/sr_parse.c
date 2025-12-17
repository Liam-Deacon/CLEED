/*********************************************************************
 *                        SR_PARSE.C
 *
 *  GPL-3.0-or-later
 *********************************************************************/

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

int sr_parse_two_reals(const char *line, real *out_a, real *out_b)
{
  if (line == NULL || out_a == NULL || out_b == NULL) return -1;

  const char *p = sr_skip_ws(line);
  if (p == NULL || *p == '\0' || *p == '#') return -1;

  char *end = NULL;
  errno = 0;
  double a = strtod(p, &end);
  if (end == p || errno != 0) return -1;

  p = sr_skip_ws(end);
  if (p == NULL || *p == '\0') return -1;

  errno = 0;
  double b = strtod(p, &end);
  if (end == p || errno != 0) return -1;

  *out_a = (real)a;
  *out_b = (real)b;
  return 0;
}

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
