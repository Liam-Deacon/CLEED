/*********************************************************************
  Unified CLEED entry point with symmetry auto-detection.
*********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <stdio.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

#include "cleed_mode.h"

int cleed_nsym_main(int argc, char *argv[]);
int cleed_sym_main(int argc, char *argv[]);

static void cleed_usage(FILE *output)
{
  fprintf(output,
          "usage: cleed [--auto|--sym|--nsym] -i <par_file> -o <res_file> [options]\n");
  fprintf(output, "Options:\n");
  fprintf(output, "  --auto               : auto-detect symmetry from input files (default)\n");
  fprintf(output, "  --sym                : force symmetrised calculations\n");
  fprintf(output, "  --nsym               : force non-symmetrised calculations\n");
  fprintf(output, "  -i <par_file>        : filepath to parameter input file\n");
  fprintf(output, "  -o <res_file>        : filepath to output file\n");
  fprintf(output, "  -b <bul_file>        : filepath to bulk parameter file\n");
  fprintf(output, "  -r <pro_name>        : (sym only) read project file\n");
  fprintf(output, "  -w <pro_name>        : (sym only) write project file\n");
  fprintf(output, "  -e                   : (nsym only) early return option\n");
  fprintf(output, "  -h --help            : print help and exit\n");
  fprintf(output, "  -V --version         : print version from selected backend\n");
  fprintf(output, "\n");
  fprintf(output, "Environment:\n");
  fprintf(output,
          "  CLEED_SYM=auto|yes|no|true|false|1|0  (ignored when a mode flag is set)\n");
}

static int cleed_is_mode_flag(const char *arg, cleed_mode_t *mode)
{
  if (strcmp(arg, "--auto") == 0)
  {
    *mode = CLEED_MODE_AUTO;
    return 1;
  }
  if (strcmp(arg, "--sym") == 0)
  {
    *mode = CLEED_MODE_SYM;
    return 1;
  }
  if (strcmp(arg, "--nsym") == 0)
  {
    *mode = CLEED_MODE_NSYM;
    return 1;
  }

  return 0;
}

static int cleed_option_takes_value(const char *arg)
{
  return strcmp(arg, "-b") == 0 ||
         strcmp(arg, "-i") == 0 ||
         strcmp(arg, "-o") == 0 ||
         strcmp(arg, "-r") == 0 ||
         strcmp(arg, "-w") == 0;
}

static int cleed_should_show_help(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
    {
      return 1;
    }
  }

  return 0;
}

static int cleed_should_show_version(int argc, char *argv[])
{
  int i;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-V") == 0 || strcmp(argv[i], "--version") == 0)
    {
      return 1;
    }
  }

  return 0;
}

static void cleed_scan_inputs(int argc, char *argv[],
                              const char **par_file,
                              const char **bul_file)
{
  int i;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-i") == 0 && i + 1 < argc)
    {
      *par_file = argv[i + 1];
      i++;
      continue;
    }

    if (strcmp(argv[i], "-b") == 0 && i + 1 < argc)
    {
      *bul_file = argv[i + 1];
      i++;
    }
  }
}

static int cleed_collect_mode_flags(int argc, char *argv[],
                                    cleed_mode_t *mode, int *mode_set)
{
  int i;

  for (i = 1; i < argc; i++)
  {
    cleed_mode_t flag_mode;

    if (!cleed_is_mode_flag(argv[i], &flag_mode))
    {
      if (cleed_option_takes_value(argv[i]) && i + 1 < argc)
      {
        i++;
      }
      continue;
    }

    if (*mode_set && flag_mode != *mode)
    {
      fprintf(stderr,
              "*** error (CLEED): conflicting mode flags (--auto/--sym/--nsym)\n");
      return 1;
    }

    *mode = flag_mode;
    *mode_set = 1;
  }

  return 0;
}

static int cleed_apply_env_mode(cleed_mode_t *mode)
{
  const char *env_value = getenv("CLEED_SYM");

  if (env_value == NULL)
  {
    return 0;
  }

  *mode = cleed_parse_mode_value(env_value);
  if (*mode == CLEED_MODE_INVALID)
  {
    fprintf(stderr,
            "*** error (CLEED): invalid value for CLEED_SYM: \"%s\"\n",
            env_value);
    fprintf(stderr,
            "    expected auto|yes|no|true|false|1|0\n");
    return 1;
  }

  return 0;
}

static cleed_mode_t cleed_resolve_auto(const char *par_file,
                                       const char *bul_file)
{
  int has_symmetry = 0;

  if (bul_file != NULL &&
      cleed_detect_symmetry_file(bul_file, &has_symmetry) == 0 &&
      has_symmetry)
  {
    return CLEED_MODE_SYM;
  }

  if (par_file != NULL &&
      cleed_detect_symmetry_file(par_file, &has_symmetry) == 0 &&
      has_symmetry)
  {
    return CLEED_MODE_SYM;
  }

  return CLEED_MODE_NSYM;
}

static int cleed_strip_mode_flags(int argc, char *argv[])
{
  int out_argc = 1;
  int i;

  for (i = 1; i < argc; i++)
  {
    cleed_mode_t flag_mode;

    if (cleed_is_mode_flag(argv[i], &flag_mode))
    {
      continue;
    }

    argv[out_argc++] = argv[i];

    if (cleed_option_takes_value(argv[i]) && i + 1 < argc)
    {
      argv[out_argc++] = argv[i + 1];
      i++;
    }
  }

  argv[out_argc] = NULL;
  return out_argc;
}

int main(int argc, char *argv[])
{
  cleed_mode_t mode = CLEED_MODE_AUTO;
  int mode_set = 0;
  const char *par_file = NULL;
  const char *bul_file = NULL;
  int show_version = 0;
  int out_argc;

  if (cleed_should_show_help(argc, argv))
  {
    cleed_usage(stdout);
    return 0;
  }

  show_version = cleed_should_show_version(argc, argv);
  cleed_scan_inputs(argc, argv, &par_file, &bul_file);

  if (cleed_collect_mode_flags(argc, argv, &mode, &mode_set) != 0)
  {
    return 1;
  }

  if (!mode_set && cleed_apply_env_mode(&mode) != 0)
  {
    return 1;
  }

  if (mode == CLEED_MODE_AUTO)
  {
    mode = cleed_resolve_auto(par_file, bul_file);
  }

  if (show_version)
  {
    out_argc = cleed_strip_mode_flags(argc, argv);
    if (mode == CLEED_MODE_SYM)
    {
      return cleed_sym_main(out_argc, argv);
    }

    return cleed_nsym_main(out_argc, argv);
  }

  out_argc = cleed_strip_mode_flags(argc, argv);
  if (mode == CLEED_MODE_SYM)
  {
    return cleed_sym_main(out_argc, argv);
  }

  return cleed_nsym_main(out_argc, argv);
}
