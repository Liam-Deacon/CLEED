// cppcheck-suppress missingIncludeSystem
#include <math.h>
// cppcheck-suppress missingIncludeSystem
#include <stdio.h>

#include "crfac.h"
#include "test_support.h"

static real linear(real x)
{
    return (real)(2.0 * x + 1.0);
}

static int test_linear_exact(void)
{
    struct crelist list[5];
    for (int i = 0; i < 5; i++) {
        list[i].energy = (real)i;
        list[i].intens = linear((real)i);
        list[i].deriv2 = 123.0;
    }

    cr_spline(list, 5);

    for (int i = 0; i < 5; i++) {
        CLEED_TEST_ASSERT_NEAR(list[i].deriv2, 0.0, 1e-10);
        CLEED_TEST_ASSERT_NEAR(cr_splint(list[i].energy, list, 5), list[i].intens, 1e-10);
    }

    CLEED_TEST_ASSERT_NEAR(cr_splint((real)2.5, list, 5), linear((real)2.5), 1e-10);
    CLEED_TEST_ASSERT_NEAR(cr_splint((real)0.25, list, 5), linear((real)0.25), 1e-10);
    CLEED_TEST_ASSERT_NEAR(cr_splint((real)-10.0, list, 5), list[0].intens, 1e-10);
    CLEED_TEST_ASSERT_NEAR(cr_splint((real)10.0, list, 5), list[4].intens, 1e-10);

    return 0;
}

static int test_interpolates_knots(void)
{
    struct crelist list[6];
    for (int i = 0; i < 6; i++) {
        const real x = (real)i;
        list[i].energy = x;
        list[i].intens = x * x;
        list[i].deriv2 = 0.0;
    }

    cr_spline(list, 6);

    for (int i = 0; i < 6; i++) {
        const real y = cr_splint(list[i].energy, list, 6);
        CLEED_TEST_ASSERT_NEAR(y, list[i].intens, 1e-10);
    }

    return 0;
}

static int test_degenerate_spacing(void)
{
    struct crelist list[4];

    list[0].energy = (real)0.0;
    list[0].intens = (real)0.0;
    list[0].deriv2 = (real)123.0;

    list[1].energy = (real)1.0;
    list[1].intens = (real)1.0;
    list[1].deriv2 = (real)123.0;

    /* Duplicate energy: exercises the degenerate-spacing path. */
    list[2].energy = (real)1.0;
    list[2].intens = (real)1.0;
    list[2].deriv2 = (real)123.0;

    list[3].energy = (real)2.0;
    list[3].intens = (real)4.0;
    list[3].deriv2 = (real)123.0;

    cr_spline(list, 4);

    const real y_at = cr_splint((real)1.0, list, 4);
    const real y_left = cr_splint((real)0.999999, list, 4);
    const real y_right = cr_splint((real)1.000001, list, 4);

    CLEED_TEST_ASSERT(y_at == y_at);
    CLEED_TEST_ASSERT(y_left == y_left);
    CLEED_TEST_ASSERT(y_right == y_right);

    CLEED_TEST_ASSERT(y_at > (real)-1e6 && y_at < (real)1e6);
    CLEED_TEST_ASSERT(y_left > (real)-1e6 && y_left < (real)1e6);
    CLEED_TEST_ASSERT(y_right > (real)-1e6 && y_right < (real)1e6);

    /* Exact knot interpolation should still work with duplicates. */
    CLEED_TEST_ASSERT_NEAR(y_at, (real)1.0, 1e-10);

    /* Close probes should remain well-behaved near the duplicate knot. */
    CLEED_TEST_ASSERT_NEAR(y_left, (real)1.0, 1e-4);
    CLEED_TEST_ASSERT_NEAR(y_right, (real)1.0, 1e-4);

    return 0;
}

int main(void)
{
    if (test_linear_exact() != 0) {
        return 1;
    }
    if (test_interpolates_knots() != 0) {
        return 1;
    }
    if (test_degenerate_spacing() != 0) {
        return 1;
    }
    return 0;
}
