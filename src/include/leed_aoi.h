/*********************************************************************
 *                     LEED_AOI.H
 *
 *  Copyright 2015 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *********************************************************************/

/*! \file
 *  \brief Include file for angle of incidence code.
 */

#ifndef LEED_AOI_H
#define LEED_AOI_H

#include "leed_def.h"

#ifdef __cplusplus
namespace cleed {
#endif

static const char leed_aoi_extension[] = "ia_"; /*!< Extension for multiple angles */

/* angle of incidence functions */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

size_t leed_get_number_of_angles(const char *filebsr);
int leed_merge_result_files(const char *par_file, size_t sa);
leed_args *leed_args_init_aoi(leed_args *src_args, size_t i_ang);

#ifdef __cplusplus
} /* extern C */
} /* namespace cleed */
#endif /* __cplusplus */

#endif /* LEED_AOI_H */
