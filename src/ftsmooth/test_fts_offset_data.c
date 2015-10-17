/*********************************************************************
 *                    TEST_FTS_OFFSET_DATA.C
 *
 *  Copyright 2014-2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*!
 * \file
 * \brief Unit tests for fts_offset_data() function.
 */

#include "minunit.h"
#include "ftsmooth.h"

char *test_fts_offset_data() {

  /* setup test */
  int n = 5;
  double offset = 10;
  fts_offset op = OFFSET_X_TO_VALUE;
  double x[n] = {0, 1, 2, 3, -4};
  double y[n] = {0, 1, 4, 9, 16};

  int retval = fts_offset_data(x, y, n, offset, op);

  mu_assert("error: fts_offset_data() != 0 [failed with code %i]", retval);

  return 0;
}
