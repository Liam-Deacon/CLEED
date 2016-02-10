/*
 * test_leed.c
 */

/*!
 * \file
 *
 * Main test unit for leed test suites.
 */

#include "minunit.h"
#include <stdio.h>
#include <string.h>
#include <stdib.h>

static void leed_test_setup() {
	// Nothing
}

static void leed_test_teardown() {
	// Nothing
}

MU_TEST(test_check) {
	mu_check(NULL == 0);
}

MU_TEST_SUITE(leed_test_suite) {
	MU_SUITE_CONFIGURE(&leed_test_setup, &leed_test_teardown);

	MU_RUN_TEST(test_check);
	/*
	MU_RUN_TEST(test_assert);
	MU_RUN_TEST(test_assert_int_eq);
	MU_RUN_TEST(test_assert_double_eq);

	MU_RUN_TEST(test_check_fail);
	MU_RUN_TEST(test_assert_fail);
	MU_RUN_TEST(test_assert_int_eq_fail);
	MU_RUN_TEST(test_assert_double_eq_fail);

	MU_RUN_TEST(test_fail);
	*/
}

int main(int argc, char *argv[]) {
	MU_RUN_SUITE(leed_test_suite);
	MU_REPORT();
	return 0;
}

