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
int sr_pso_swarm_size = 0;
real sr_pso_inertia = (real)0.0;
real sr_pso_c1 = (real)0.0;
real sr_pso_c2 = (real)0.0;
real sr_pso_vmax = (real)0.0;
int sr_de_eval_limit = MAX_EVAL_DE;
int sr_de_iter_limit = MAX_ITER_DE;
int sr_de_population = 0;
real sr_de_weight = (real)0.0;
real sr_de_crossover = (real)0.0;
real sr_de_init_span = (real)0.0;
