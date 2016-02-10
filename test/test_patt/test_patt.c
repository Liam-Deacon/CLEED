/*
 * test_patt.c
 */

/*!
 * \file
 *
 * Main test unit for patt test suites.
 */

#include "minunit.h"
#include <stdio.h>
#include <string.h>
#include <stdib.h>

static void patt_test_setup() {
	// Nothing
}

static void patt_test_teardown() {
	// Nothing
}

MU_TEST(test_check) {
	mu_check(NULL == 0);
}

MU_TEST_SUITE(patt_test_suite) {
	MU_SUITE_CONFIGURE(&patt_test_setup, &patt_test_teardown);

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
	MU_RUN_SUITE(patt_test_suite);
	MU_REPORT();
	return 0;
}

