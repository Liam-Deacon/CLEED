/*********************************************************************
  Helpers for selecting CLEED symmetry mode.
*********************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "cleed_mode.h"

static int cleed_match_value(const char *value, const char *token)
{
  size_t i;

  for (i = 0; value[i] != '\0' && token[i] != '\0'; i++)
  {
    if (tolower((unsigned char)value[i]) != tolower((unsigned char)token[i]))
    {
      return 0;
    }
  }

  return value[i] == '\0' && token[i] == '\0';
}

cleed_mode_t cleed_parse_mode_value(const char *value)
{
  if (value == NULL)
  {
    return CLEED_MODE_INVALID;
  }

  if (cleed_match_value(value, "auto"))
  {
    return CLEED_MODE_AUTO;
  }
  if (cleed_match_value(value, "yes") ||
      cleed_match_value(value, "true") ||
      cleed_match_value(value, "1"))
  {
    return CLEED_MODE_SYM;
  }
  if (cleed_match_value(value, "no") ||
      cleed_match_value(value, "false") ||
      cleed_match_value(value, "0"))
  {
    return CLEED_MODE_NSYM;
  }

  return CLEED_MODE_INVALID;
}

static int cleed_line_has_symmetry(const char *line)
{
  const unsigned char *p = (const unsigned char *)line;

  while (*p != '\0' && isspace(*p))
  {
    p++;
  }

  if (*p == '\0' || *p == '#' || *p == '!' || *p == ';')
  {
    return 0;
  }

  if ((p[0] == 's' || p[0] == 'S') &&
      (p[1] == 'r' || p[1] == 'R' || p[1] == 'm' || p[1] == 'M'))
  {
    if (p[2] == '\0' || p[2] == ':' || isspace(p[2]))
    {
      return 1;
    }
  }

  return 0;
}

int cleed_detect_symmetry_file(const char *path, int *has_symmetry)
{
  FILE *stream;
  char line[512];
  int found = 0;

  if (has_symmetry != NULL)
  {
    *has_symmetry = 0;
  }

  if (path == NULL)
  {
    return -1;
  }

  stream = fopen(path, "r");
  if (stream == NULL)
  {
    return -1;
  }

  while (fgets(line, sizeof(line), stream) != NULL)
  {
    if (cleed_line_has_symmetry(line))
    {
      found = 1;
      break;
    }
  }

  fclose(stream);

  if (has_symmetry != NULL)
  {
    *has_symmetry = found;
  }

  return 0;
}
