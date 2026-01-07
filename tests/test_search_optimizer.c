#include "search_optimizer.h"
#include "search.h"
#include "test_support.h"

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

    opt = sr_optimizer_by_name("unknown");
    CLEED_TEST_ASSERT(opt == NULL);

    return 0;
}

static int test_config_from_env(void)
{
    sr_optimizer_config cfg;
    sr_optimizer_config_init(&cfg);

    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_EVALS", "123") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_MAX_ITERS", "456") == 0);
    CLEED_TEST_ASSERT(test_set_env_value("CSEARCH_SEED", "789") == 0);

    sr_optimizer_config_from_env(&cfg);

    CLEED_TEST_ASSERT(cfg.max_evals == 123);
    CLEED_TEST_ASSERT(cfg.max_iters == 456);
    CLEED_TEST_ASSERT(cfg.seed == 789);

    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_EVALS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_MAX_ITERS") == 0);
    CLEED_TEST_ASSERT(test_unset_env_value("CSEARCH_SEED") == 0);

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
    return 0;
}
