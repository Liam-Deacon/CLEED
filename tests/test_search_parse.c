#include "sr_parse.h"

// cppcheck-suppress missingIncludeSystem
#include <stdio.h>

#include "test_support.h"

static int test_parse_two_reals(void)
{
    real a = 0.0;
    real b = 0.0;

    CLEED_TEST_ASSERT(sr_parse_two_reals("  1.25   -3.5  \n", &a, &b) == 0);
    CLEED_TEST_ASSERT_NEAR(a, 1.25, 1e-12);
    CLEED_TEST_ASSERT_NEAR(b, -3.5, 1e-12);

    CLEED_TEST_ASSERT(sr_parse_two_reals("# comment\n", &a, &b) != 0);
    CLEED_TEST_ASSERT(sr_parse_two_reals("not-a-number 1\n", &a, &b) != 0);
    CLEED_TEST_ASSERT(sr_parse_two_reals("1\n", &a, &b) != 0);

    return 0;
}

static int test_read_two_reals_from_file(void)
{
    const char *path = "parse_test.dum";
    const char *contents =
        "# header\n"
        "\n"
        "   0.5   2.0  extra\n";

    CLEED_TEST_ASSERT(cleed_test_write_text_file(path, contents) == 0);

    real a = 0.0;
    real b = 0.0;
    CLEED_TEST_ASSERT(sr_read_two_reals_from_file(path, &a, &b) == 0);
    CLEED_TEST_ASSERT_NEAR(a, 0.5, 1e-12);
    CLEED_TEST_ASSERT_NEAR(b, 2.0, 1e-12);

    (void)cleed_test_remove_file(path);
    return 0;
}

int main(void)
{
    if (test_parse_two_reals() != 0) {
        return 1;
    }
    if (test_read_two_reals_from_file() != 0) {
        return 1;
    }
    return 0;
}

