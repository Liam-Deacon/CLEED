/*********************************************************************
 *                       SEARCH_OPTIMIZER.H
 *
 *  Optimizer registry + configuration helpers for SEARCH (csearch).
 *********************************************************************/

#ifndef SEARCH_OPTIMIZER_H
#define SEARCH_OPTIMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

// cppcheck-suppress missingIncludeSystem
#include <stddef.h>
// cppcheck-suppress missingIncludeSystem
#include <stdint.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>

#include "real.h"

typedef struct sr_optimizer_config {
  // cppcheck-suppress unusedStructMember
  int max_evals;
  // cppcheck-suppress unusedStructMember
  int max_iters;
  // cppcheck-suppress unusedStructMember
  uint64_t seed;
  // cppcheck-suppress unusedStructMember
  int pso_swarm_size;
  // cppcheck-suppress unusedStructMember
  real pso_inertia;
  // cppcheck-suppress unusedStructMember
  real pso_c1;
  // cppcheck-suppress unusedStructMember
  real pso_c2;
  // cppcheck-suppress unusedStructMember
  real pso_vmax;
  // cppcheck-suppress unusedStructMember
  int de_population;
  // cppcheck-suppress unusedStructMember
  real de_weight;
  // cppcheck-suppress unusedStructMember
  real de_crossover;
  // cppcheck-suppress unusedStructMember
  real de_init_span;
} sr_optimizer_config;

typedef struct sr_optimizer_def {
  // cppcheck-suppress unusedStructMember
  const char *name;
  // cppcheck-suppress unusedStructMember
  const char *primary;
  // cppcheck-suppress unusedStructMember
  const char *description;
  // cppcheck-suppress unusedStructMember
  const char *aliases_help;
  // cppcheck-suppress unusedStructMember
  int type;
  // cppcheck-suppress unusedStructMember
  int implemented;
  // cppcheck-suppress unusedStructMember
  int uses_delta;
  // cppcheck-suppress unusedStructMember
  int is_default;
} sr_optimizer_def;

void sr_optimizer_config_init(sr_optimizer_config *cfg);
void sr_optimizer_config_from_env(sr_optimizer_config *cfg);
void sr_optimizer_config_apply(const sr_optimizer_config *cfg);
void sr_optimizer_log_config(FILE *output, const sr_optimizer_config *cfg);

size_t sr_optimizer_count(void);
const sr_optimizer_def *sr_optimizer_at(size_t index);
const sr_optimizer_def *sr_optimizer_by_name(const char *name);
const sr_optimizer_def *sr_optimizer_by_type(int type);
int sr_optimizer_run(const sr_optimizer_def *opt, const sr_optimizer_config *cfg,
                     int ndim, real dpos, const char *bak_file,
                     const char *log_file);
void sr_optimizer_print_help(FILE *output);

int sr_parse_int_arg(const char *val, int *out);
int sr_parse_real_arg(const char *val, real *out);
int sr_parse_seed_arg(const char *val, uint64_t *out);
void sr_optimizer_log_results(FILE *log_stream, const char *name, int ndim,
                              int evals, const real *best, real best_val);
uint64_t sr_optimizer_get_seed(void);

#ifdef __cplusplus
}
#endif

#endif /* SEARCH_OPTIMIZER_H */
