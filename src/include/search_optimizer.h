/*********************************************************************
 *                       SEARCH_OPTIMIZER.H
 *
 *  Optimizer registry + configuration helpers for SEARCH (csearch).
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef SEARCH_OPTIMIZER_H
#define SEARCH_OPTIMIZER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "real.h"

typedef struct sr_optimizer_config {
  int max_evals;
  int max_iters;
  uint64_t seed;
  int pso_swarm_size;
  real pso_inertia;
  real pso_c1;
  real pso_c2;
  real pso_vmax;
} sr_optimizer_config;

typedef struct sr_optimizer_def {
  const char *name;
  const char *primary;
  const char *description;
  const char *aliases_help;
  int type;
  int implemented;
  int uses_delta;
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

#endif /* SEARCH_OPTIMIZER_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
