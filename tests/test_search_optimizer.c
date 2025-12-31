#include "search_optimizer.h"
#include "search.h"
#include "test_support.h"

// cppcheck-suppress missingIncludeSystem
#include <inttypes.h>
// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

static int test_set_env_value(const char *name, const char *value)
{
#ifdef _WIN32
    return _putenv_s(name, value);
#else
    return setenv(name, value, 1);
#endif
}

static int test_unset_env_value(const char *name)
{
#ifdef _WIN32
    return _putenv_s(name, "");
#else
    return unsetenv(name);
#endif
}

static int test_set_env_triplet(const char *evals, const char *iters, const char *seed)
{
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_EVALS", evals) == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_ITERS", iters) == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_SEED", seed) == 0);
    return 0;
}

static int test_unset_env_triplet(void)
{
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_EVALS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_ITERS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_SEED") == 0);
    return 0;
}

static int test_assert_env_config(const char *evals, const char *iters, const char *seed,
                                  int expected_evals, int expected_iters, uint64_t expected_seed)
{
    sr_optimizer_config cfg;
    sr_optimizer_config_init(&cfg);

    if (test_set_env_triplet(evals, iters, seed) != 0) {
        return 1;
    }
    sr_optimizer_config_from_env(&cfg);

    CLEED_TEST_ASSERT(cfg.max_evals == expected_evals);
    CLEED_TEST_ASSERT(cfg.max_iters == expected_iters);
    CLEED_TEST_ASSERT(cfg.seed == expected_seed);

    if (test_unset_env_triplet() != 0) {
        return 1;
    }
    return 0;
}

static void test_restore_globals(int amoeba, int powell, int sa, uint64_t seed)
{
    sr_amoeba_eval_limit = amoeba;
    sr_powell_iter_limit = powell;
    sr_sa_iter_limit = sa;
    sa_idum = seed;
}

static int test_lookup_by_name(void)
{
    const sr_optimizer_def *opt = NULL;

    opt = sr_optimizer_by_name("si");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_SIMPLEX);

    opt = sr_optimizer_by_name("simplex");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_SIMPLEX);

    opt = sr_optimizer_by_name("po");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_POWELL);

    opt = sr_optimizer_by_name("sa");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_SIM_ANNEALING);

    opt = sr_optimizer_by_name("ga");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_GENETIC);

    opt = sr_optimizer_by_name("ps");
    CLEED_TEST_ASSERT(opt != NULL);
    CLEED_TEST_ASSERT(opt->type == SR_PSO);

    opt = sr_optimizer_by_name("unknown");
    CLEED_TEST_ASSERT(opt == NULL);

    return 0;
}

static int test_config_from_env(void)
{
    if (test_assert_env_config("123", "456", "789", 123, 456, 789) != 0) {
        return 1;
    }

    return 0;
}

static int test_config_from_env_invalid_values(void)
{
    if (test_assert_env_config("nope", "n/a", "oops", 0, 0, 0) != 0) {
        return 1;
    }

    return 0;
}

static int test_config_from_env_edge_values(void)
{
    if (test_assert_env_config("-5", "-10", "0", 0, 0, 0) != 0) {
        return 1;
    }
    if (test_assert_env_config("9999999999", "9999999999", "18446744073709551615",
                               0, 0, UINT64_MAX) != 0) {
        return 1;
    }

    return 0;
}

static int test_config_from_env_pso(void)
{
    sr_optimizer_config cfg;
    sr_optimizer_config_init(&cfg);

    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_EVALS", "123") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_ITERS", "456") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_SEED", "789") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_PSO_SWARM", "32") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_PSO_INERTIA", "0.73") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_PSO_C1", "1.50") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_PSO_C2", "1.60") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_PSO_VMAX", "2.00") == 0);

    sr_optimizer_config_from_env(&cfg);

    CLEED_TEST_ASSERT(cfg.max_evals == 123);
    CLEED_TEST_ASSERT(cfg.max_iters == 456);
    CLEED_TEST_ASSERT(cfg.seed == 789);
    CLEED_TEST_ASSERT(cfg.pso_swarm_size == 32);
    CLEED_TEST_ASSERT(fabs(cfg.pso_inertia - (real)0.73) < (real)1e-6);
    CLEED_TEST_ASSERT(fabs(cfg.pso_c1 - (real)1.50) < (real)1e-6);
    CLEED_TEST_ASSERT(fabs(cfg.pso_c2 - (real)1.60) < (real)1e-6);
    CLEED_TEST_ASSERT(fabs(cfg.pso_vmax - (real)2.00) < (real)1e-6);

    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_EVALS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_ITERS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_SEED") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_PSO_SWARM") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_PSO_INERTIA") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_PSO_C1") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_PSO_C2") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_PSO_VMAX") == 0);

    return 0;
}

static int test_config_apply(void)
{
    const int default_amoeba = MAX_ITER_AMOEBA;
    const int default_powell = MAX_ITER_POWELL;
    const int default_sa = MAX_ITER_SA;
    const int orig_amoeba = sr_amoeba_eval_limit;
    const int orig_powell = sr_powell_iter_limit;
    const int orig_sa = sr_sa_iter_limit;
    const uint64_t orig_seed = sa_idum;
    sr_optimizer_config cfg;

    test_restore_globals(1, 2, 3, UINT64_C(55));
    memset(&cfg, 0, sizeof(cfg));
    sr_optimizer_config_apply(&cfg);

    CLEED_TEST_ASSERT(sr_amoeba_eval_limit == default_amoeba);
    CLEED_TEST_ASSERT(sr_powell_iter_limit == default_powell);
    CLEED_TEST_ASSERT(sr_sa_iter_limit == default_sa);
    CLEED_TEST_ASSERT(sa_idum == 0);

    test_restore_globals(1, 2, 3, UINT64_C(55));

    memset(&cfg, 0, sizeof(cfg));
    cfg.max_evals = 1234;
    cfg.max_iters = 456;
    sr_optimizer_config_apply(&cfg);

    CLEED_TEST_ASSERT(sr_amoeba_eval_limit == 1234);
    CLEED_TEST_ASSERT(sr_powell_iter_limit == 456);
    CLEED_TEST_ASSERT(sr_sa_iter_limit == 456);
    CLEED_TEST_ASSERT(sa_idum == 0);

    test_restore_globals(1, 2, 3, UINT64_C(55));

    memset(&cfg, 0, sizeof(cfg));
    cfg.max_iters = 456;
    sr_optimizer_config_apply(&cfg);

    CLEED_TEST_ASSERT(sr_amoeba_eval_limit == default_amoeba);
    CLEED_TEST_ASSERT(sr_powell_iter_limit == 456);
    CLEED_TEST_ASSERT(sr_sa_iter_limit == 456);
    CLEED_TEST_ASSERT(sa_idum == 0);

    test_restore_globals(1, 2, 3, UINT64_C(55));

    memset(&cfg, 0, sizeof(cfg));
    cfg.max_evals = 789;
    cfg.max_iters = 321;
    sr_optimizer_config_apply(&cfg);

    CLEED_TEST_ASSERT(sr_amoeba_eval_limit == 789);
    CLEED_TEST_ASSERT(sr_powell_iter_limit == 321);
    CLEED_TEST_ASSERT(sr_sa_iter_limit == 321);
    CLEED_TEST_ASSERT(sa_idum == 0);

    test_restore_globals(1, 2, 3, UINT64_C(55));

    memset(&cfg, 0, sizeof(cfg));
    cfg.seed = UINT64_C(0x123456789);
    sr_optimizer_config_apply(&cfg);

    CLEED_TEST_ASSERT(sa_idum == cfg.seed);

    test_restore_globals(orig_amoeba, orig_powell, orig_sa, orig_seed);

    return 0;
}

int main(void)
{
    if (test_lookup_by_name() != 0) {
        return 1;
    }
    if (test_config_from_env() != 0) {
        return 1;
    }
    if (test_config_from_env_invalid_values() != 0) {
        return 1;
    }
    if (test_config_from_env_edge_values() != 0) {
        return 1;
    }
    if (test_config_from_env_pso() != 0) {
        return 1;
    }
    if (test_config_apply() != 0) {
        return 1;
    }
    return 0;
}
