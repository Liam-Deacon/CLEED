/*********************************************************************
 *													leed_var.c
 *
 *  Copyright (C) 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/19 Mar 2015 - creation
 *********************************************************************/

/*!
 * \file leed_var.c
 * \author Liam Deacon
 * \date 19 Mar 2015
 */

#include "leed.h"
#include <stdlib.h>

void leed_var_free(leed_var *var) {
  matfree(var->p_tl);
  free(var);
  var = NULL;
}
