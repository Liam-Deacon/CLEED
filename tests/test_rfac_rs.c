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

static void fill_parabola_scaled(real *eng, real *intens, int n, real offset, real scale)
{
    for (int i = 0; i < n; i++) {
        const real x = (real)i - (real)(n / 2);
        eng[i] = (real)i;
        intens[i] = scale * (x * x) + offset;
    }
    eng[n] = F_END_OF_LIST;
    intens[n] = F_END_OF_LIST;
}

static int test_rs_identical_zero(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)1.0);
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

    fill_parabola(eng, exp_int, 11, (real)1.0);
    fill_parabola(eng, theo_int, 11, (real)1.1);

    const real rs_small = cr_rs(eng, exp_int, theo_int, (real)4.0);
    fill_parabola(eng, theo_int, 11, (real)1.5);
    const real rs_large = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT(rs_large > rs_small);

    return 0;
}

static int test_rs_vi_zero_branch(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)1.0);
    for (int i = 0; i < 11; i++) {
        theo_int[i] = exp_int[i];
    }
    theo_int[11] = F_END_OF_LIST;

    const real rs = cr_rs(eng, exp_int, theo_int, (real)0.0);
    CLEED_TEST_ASSERT_NEAR(rs, 0.0, 1e-8);
    CLEED_TEST_ASSERT(isfinite(rs));

    return 0;
}

static int test_rs_vi_sensitivity(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)1.0);
    for (int i = 0; i < 11; i++) {
        theo_int[i] = exp_int[i];
    }
    theo_int[0] += (real)1.0;
    theo_int[11] = F_END_OF_LIST;

    const real rs_vi2 = cr_rs(eng, exp_int, theo_int, (real)2.0);
    const real rs_vi8 = cr_rs(eng, exp_int, theo_int, (real)8.0);

    CLEED_TEST_ASSERT(rs_vi2 >= (real)0.0);
    CLEED_TEST_ASSERT(rs_vi8 >= (real)0.0);
    CLEED_TEST_ASSERT(isfinite(rs_vi2));
    CLEED_TEST_ASSERT(isfinite(rs_vi8));
    CLEED_TEST_ASSERT(fabs((double)(rs_vi2 - rs_vi8)) > 1e-12);

    return 0;
}

static int test_rs_symmetry(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola(eng, exp_int, 11, (real)1.0);
    fill_parabola(eng, theo_int, 11, (real)1.2);

    const real rs1 = cr_rs(eng, exp_int, theo_int, (real)4.0);
    const real rs2 = cr_rs(eng, theo_int, exp_int, (real)4.0);

    CLEED_TEST_ASSERT_NEAR(rs1, rs2, 1e-8);

    return 0;
}

static int test_rs_shape_sensitivity(void)
{
    real eng[12];
    real exp_int[12];
    real theo_int[12];

    fill_parabola_scaled(eng, exp_int, 11, (real)1.0, (real)1.0);
    fill_parabola_scaled(eng, theo_int, 11, (real)1.0, (real)1.3);

    const real rs = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT(rs > 0.0);
    CLEED_TEST_ASSERT(isfinite(rs));

    return 0;
}

static int test_rs_fails_with_too_few_points(void)
{
    real eng[3];
    real exp_int[3];
    real theo_int[3];

    eng[0] = (real)0.0;
    exp_int[0] = (real)1.0;
    theo_int[0] = (real)1.0;
    eng[1] = F_END_OF_LIST;
    exp_int[1] = F_END_OF_LIST;
    theo_int[1] = F_END_OF_LIST;

    const real rs = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT(rs == F_FAIL);

    return 0;
}

static int test_rs_fails_with_zero_intensity_sum(void)
{
    real eng[4];
    real exp_int[4];
    real theo_int[4];

    eng[0] = (real)0.0;
    eng[1] = (real)1.0;
    eng[2] = F_END_OF_LIST;
    exp_int[0] = (real)0.0;
    exp_int[1] = (real)0.0;
    exp_int[2] = F_END_OF_LIST;
    theo_int[0] = (real)0.0;
    theo_int[1] = (real)0.0;
    theo_int[2] = F_END_OF_LIST;

    const real rs = cr_rs(eng, exp_int, theo_int, (real)4.0);
    CLEED_TEST_ASSERT(rs == F_FAIL);

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
    if (test_rs_vi_zero_branch() != 0) {
        return 1;
    }
    if (test_rs_vi_sensitivity() != 0) {
        return 1;
    }
    if (test_rs_symmetry() != 0) {
        return 1;
    }
    if (test_rs_shape_sensitivity() != 0) {
        return 1;
    }
    if (test_rs_fails_with_too_few_points() != 0) {
        return 1;
    }
    if (test_rs_fails_with_zero_intensity_sum() != 0) {
        return 1;
    }
    return 0;
}
