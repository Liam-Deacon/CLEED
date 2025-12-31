/***********************************************************************
Global state for search library.

Modern toolchains default to -fno-common, so globals must be defined
exactly once (and declared as extern in headers).
***********************************************************************/

#include "search.h"

struct sratom_str *sr_atoms = NULL;
struct search_str *sr_search = NULL;
char *sr_project = NULL;
int sr_amoeba_eval_limit = MAX_ITER_AMOEBA;
int sr_powell_iter_limit = MAX_ITER_POWELL;
int sr_sa_iter_limit = MAX_ITER_SA;
int sr_pso_eval_limit = MAX_EVAL_PSO;
int sr_pso_iter_limit = MAX_ITER_PSO;
