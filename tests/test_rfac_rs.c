// cppcheck-suppress missingIncludeSystem
#include <math.h>

#include "crfac.h"
#include "test_support.h"

static void fill_parabola(real *eng, real *intens, int n, real offset)
{
    for (int i = 0; i < n; i++) {
        const real x = (real)i - (real)(n / 2);
        eng[i] = (real)i;
        intens[i] = x * x + offset;
    }
    eng[n] = F_END_OF_LIST;
    intens[n] = F_END_OF_LIST;
}

static int test_rs_identical_zero(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)0.0);
    for (int i = 0; i < 11; i++) {
        theo_int[i] = exp_int[i];
    }
    theo_int[11] = F_END_OF_LIST;

    const real rs = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT_NEAR(rs, 0.0, 1e-8);

    return 0;
}

static int test_rs_offset_increases(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)0.0);
    fill_parabola(eng, theo_int, 11, (real)0.1);

    const real rs = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT(rs > 0.0);

    return 0;
}

int main(void)
{
    if (test_rs_identical_zero() != 0) {
        return 1;
    }
    if (test_rs_offset_increases() != 0) {
        return 1;
    }
    return 0;
}
