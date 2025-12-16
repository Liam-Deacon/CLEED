/***********************************************************************
Global state for search library.

Modern toolchains default to -fno-common, so globals must be defined
exactly once (and declared as extern in headers).
***********************************************************************/

#include "search.h"

struct sratom_str *sr_atoms = NULL;
struct search_str *sr_search = NULL;
char *sr_project = NULL;

