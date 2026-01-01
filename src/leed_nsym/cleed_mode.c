/*********************************************************************
  Helpers for selecting CLEED symmetry mode.
*********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <ctype.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
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

static int cleed_value_in_list(const char *value,
                               const char *const *list,
                               size_t list_size)
{
  size_t i;

  for (i = 0; i < list_size; i++)
  {
    if (cleed_match_value(value, list[i]))
    {
      return 1;
    }
  }

  return 0;
}

cleed_mode_t cleed_parse_mode_value(const char *value)
{
  static const char *const yes_values[] = {"yes", "true", "1"};
  static const char *const no_values[] = {"no", "false", "0"};

  if (value == NULL)
  {
    return CLEED_MODE_INVALID;
  }

  if (cleed_match_value(value, "auto"))
  {
    return CLEED_MODE_AUTO;
  }
  if (cleed_value_in_list(value, yes_values,
                          sizeof(yes_values) / sizeof(yes_values[0])))
  {
    return CLEED_MODE_SYM;
  }
  if (cleed_value_in_list(value, no_values,
                          sizeof(no_values) / sizeof(no_values[0])))
  {
    return CLEED_MODE_NSYM;
  }

  return CLEED_MODE_INVALID;
}

static const unsigned char *cleed_skip_ws(const unsigned char *p)
{
  while (*p != '\0' && isspace(*p))
  {
    p++;
  }

  return p;
}

static int cleed_is_comment_start(unsigned char c)
{
  return c == '#' || c == '!' || c == ';';
}

static int cleed_is_symmetry_token(const unsigned char *p)
{
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

static int cleed_line_has_symmetry(const char *line)
{
  const unsigned char *p = (const unsigned char *)line;

  p = cleed_skip_ws(p);
  if (*p == '\0' || cleed_is_comment_start(*p))
  {
    return 0;
  }

  return cleed_is_symmetry_token(p);
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
