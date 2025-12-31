/*********************************************************************
 *                       SR_OPTIMIZERS.C
 *
 *  Optimizer registry + configuration helpers for SEARCH (csearch).
 *********************************************************************/

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "search.h"
#include "search_optimizer.h"

typedef int (*sr_optimizer_run_fn)(int ndim, real dpos, const char *bak_file,
                                   const char *log_file);

typedef struct sr_optimizer_entry {
  sr_optimizer_def def;
  const char *aliases[8];
  sr_optimizer_run_fn run;
} sr_optimizer_entry;

static int sr_run_simplex(int ndim, real dpos, const char *bak_file,
                          const char *log_file)
{
  SR_SX(ndim, dpos, bak_file, log_file);
  return 0;
}

static int sr_run_powell(int ndim, real dpos, const char *bak_file,
                         const char *log_file)
{
  (void)dpos;
  SR_PO(ndim, bak_file, log_file);
  return 0;
}

static int sr_run_sa(int ndim, real dpos, const char *bak_file,
                     const char *log_file)
{
  SR_SA(ndim, dpos, bak_file, log_file);
  return 0;
}

static int sr_run_ga(int ndim, real dpos, const char *bak_file,
                     const char *log_file)
{
  (void)ndim;
  (void)dpos;
  (void)bak_file;
  (void)log_file;
  SR_NOT_IMPLEMENTED_ERROR("genetic algorithm");
  return -1;
}

static int sr_run_pso(int ndim, real dpos, const char *bak_file,
                      const char *log_file)
{
  sr_pso(ndim, dpos, bak_file, log_file);
  return 0;
}

static const sr_optimizer_entry sr_optimizers[] = {
    {
        .def = {
            .name = "simplex",
            .primary = "si",
            .description = "simplex method",
            .aliases_help = "sx, simplex",
            .type = SR_SIMPLEX,
            .implemented = 1,
            .uses_delta = 1,
            .is_default = 1,
        },
        .aliases = {"si", "sx", "simplex", NULL},
        .run = sr_run_simplex,
    },
    {
        .def = {
            .name = "powell",
            .primary = "po",
            .description = "Powell's method",
            .aliases_help = "powell",
            .type = SR_POWELL,
            .implemented = 1,
            .uses_delta = 0,
            .is_default = 0,
        },
        .aliases = {"po", "powell", NULL},
        .run = sr_run_powell,
    },
    {
        .def = {
            .name = "annealing",
            .primary = "sa",
            .description = "simulated annealing",
            .aliases_help = "anneal, annealing",
            .type = SR_SIM_ANNEALING,
            .implemented = 1,
            .uses_delta = 1,
            .is_default = 0,
        },
        .aliases = {"sa", "anneal", "annealing", "simulated", "simulated-annealing", NULL},
        .run = sr_run_sa,
    },
    {
        .def = {
            .name = "genetic",
            .primary = "ga",
            .description = "genetic algorithm",
            .aliases_help = "genetic",
            .type = SR_GENETIC,
            .implemented = 0,
            .uses_delta = 0,
            .is_default = 0,
        },
        .aliases = {"ga", "genetic", "genetic-algorithm", NULL},
        .run = sr_run_ga,
    },
    {
        .def = {
            .name = "pso",
            .primary = "ps",
            .description = "particle swarm optimisation",
            .aliases_help = "pso, swarm",
            .type = SR_PSO,
            .implemented = 1,
            .uses_delta = 1,
            .is_default = 0,
        },
        .aliases = {"ps", "pso", "swarm", NULL},
        .run = sr_run_pso,
    },
};

static const sr_optimizer_entry *sr_optimizer_entry_by_type(int type)
{
  size_t i;
  for (i = 0; i < sr_optimizer_count(); i++) {
    if (sr_optimizers[i].def.type == type) {
      return &sr_optimizers[i];
    }
  }
  return NULL;
}

static int sr_optimizer_matches(const char *name,
                                const sr_optimizer_entry *entry)
{
  size_t i;
  if (!name || !entry) {
    return 0;
  }
  for (i = 0; entry->aliases[i] != NULL; i++) {
    const char *alias = entry->aliases[i];
    size_t len = strlen(alias);
    if (len == 0) {
      continue;
    }
    if (strncasecmp(name, alias, len) == 0) {
      return 1;
    }
  }
  return 0;
}

void sr_optimizer_config_init(sr_optimizer_config *cfg)
{
  if (!cfg) return;
  cfg->max_evals = 0;
  cfg->max_iters = 0;
  cfg->seed = 0;
}

static int sr_optimizer_read_int_env(const char *name, int *out_value)
{
  char *end = NULL;
  const char *raw = getenv(name);
  long parsed = 0;

  if (!raw || !out_value) return 0;
  parsed = strtol(raw, &end, 10);
  if (end == raw || parsed <= 0) return 0;
  *out_value = (int)parsed;
  return 1;
}

static int sr_optimizer_read_seed_env(const char *name, uint64_t *out_value)
{
  char *end = NULL;
  const char *raw = getenv(name);
  unsigned long long parsed = 0;

  if (!raw || !out_value) return 0;
  parsed = strtoull(raw, &end, 10);
  if (end == raw || parsed == 0) return 0;
  *out_value = (uint64_t)parsed;
  return 1;
}

void sr_optimizer_config_from_env(sr_optimizer_config *cfg)
{
  if (!cfg) return;
  (void)sr_optimizer_read_int_env("CSEARCH_MAX_EVALS", &cfg->max_evals);
  (void)sr_optimizer_read_int_env("CSEARCH_MAX_ITERS", &cfg->max_iters);
  (void)sr_optimizer_read_seed_env("CSEARCH_SEED", &cfg->seed);
}

void sr_optimizer_config_apply(const sr_optimizer_config *cfg)
{
  if (!cfg) return;
  if (cfg->max_evals > 0) {
    sr_amoeba_eval_limit = cfg->max_evals;
  }
  if (cfg->max_iters > 0) {
    sr_powell_iter_limit = cfg->max_iters;
    sr_sa_iter_limit = cfg->max_iters;
    sr_pso_iter_limit = cfg->max_iters;
    if (cfg->max_evals <= 0) {
      sr_amoeba_eval_limit = cfg->max_iters;
    }
  }
  if (cfg->max_evals > 0) {
    sr_pso_eval_limit = cfg->max_evals;
  }
  if (cfg->seed > 0) {
    sa_idum = (long)(cfg->seed > (uint64_t)LONG_MAX ? LONG_MAX : cfg->seed);
    sa_idum = -sa_idum;
  }
}

void sr_optimizer_log_config(FILE *output, const sr_optimizer_config *cfg)
{
  if (!output || !cfg) return;

  fprintf(output, "=> Optimizer config:");
  if (cfg->max_evals <= 0 && cfg->max_iters <= 0 && cfg->seed == 0) {
    fprintf(output, " defaults\n");
    return;
  }

  if (cfg->max_evals > 0) {
    fprintf(output, " max_evals=%d", cfg->max_evals);
  } else {
    fprintf(output, " max_evals=default");
  }

  if (cfg->max_iters > 0) {
    fprintf(output, " max_iters=%d", cfg->max_iters);
  } else {
    fprintf(output, " max_iters=default");
  }

  if (cfg->seed > 0) {
    fprintf(output, " seed=%llu", (unsigned long long)cfg->seed);
  } else {
    fprintf(output, " seed=default");
  }

  fprintf(output, "\n");
}

size_t sr_optimizer_count(void)
{
  return sizeof(sr_optimizers) / sizeof(sr_optimizers[0]);
}

const sr_optimizer_def *sr_optimizer_at(size_t index)
{
  if (index >= sr_optimizer_count()) {
    return NULL;
  }
  return &sr_optimizers[index].def;
}

const sr_optimizer_def *sr_optimizer_by_name(const char *name)
{
  size_t i;
  if (!name) {
    return NULL;
  }
  for (i = 0; i < sr_optimizer_count(); i++) {
    if (sr_optimizer_matches(name, &sr_optimizers[i])) {
      return &sr_optimizers[i].def;
    }
  }
  return NULL;
}

const sr_optimizer_def *sr_optimizer_by_type(int type)
{
  const sr_optimizer_entry *entry = sr_optimizer_entry_by_type(type);
  return entry ? &entry->def : NULL;
}

int sr_optimizer_run(const sr_optimizer_def *opt, const sr_optimizer_config *cfg,
                     int ndim, real dpos, const char *bak_file,
                     const char *log_file)
{
  const sr_optimizer_entry *entry = NULL;
  if (!opt) {
    return -1;
  }

  entry = sr_optimizer_entry_by_type(opt->type);
  if (!entry || !entry->run) {
    return -1;
  }

  sr_optimizer_config_apply(cfg);
  return entry->run(ndim, dpos, bak_file, log_file);
}

void sr_optimizer_print_help(FILE *output)
{
  size_t i;
  if (!output) {
    return;
  }
  for (i = 0; i < sr_optimizer_count(); i++) {
    const sr_optimizer_def *def = &sr_optimizers[i].def;
    fprintf(output, "                          '%s' = %s",
            def->primary, def->description);
    if (def->aliases_help && def->aliases_help[0] != '\0') {
      fprintf(output, " (aliases: %s)", def->aliases_help);
    }
    if (!def->implemented) {
      fprintf(output, " (not implemented)");
    }
    if (def->is_default) {
      fprintf(output, " (default)");
    }
    fprintf(output, "\n");
  }
}
