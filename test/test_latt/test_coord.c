/*********************************************************************
 *                       TEST_COORD.C
 *
 *  Copyright 2015-2016 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 *
 * Unit tests for #coord objects.
 */

#include "minunit.h"
#include "coord.h"
#include <stdlib.h>

static coord *pos = NULL;

static void coord_test_setup() {
	// Nothing
	pos = coord_init();
}

static void coord_test_teardown() {
	// Nothing
	coord_free(pos);
}

MU_TEST(test) {

}

MU_TEST_SUITE(latt_test_suite) {
	MU_SUITE_CONFIGURE(&coord_test_setup, &coord_test_teardown);

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
