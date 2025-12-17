/*********************************************************************
 *                        SR_PARSE.H
 *
 *  GPL-3.0-or-later
 *
 *  Small parsing helpers shared by SEARCH utilities.
 *********************************************************************/

#ifndef SR_PARSE_H
#define SR_PARSE_H

#include "real.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Parses two whitespace-delimited reals from a line. Returns 0 on success. */
int sr_parse_two_reals(const char *line, real *out_a, real *out_b);

/* Scans a file until a line containing two reals is found. Returns 0 on success. */
int sr_read_two_reals_from_file(const char *path, real *out_a, real *out_b);

#ifdef __cplusplus
}
#endif

#endif /* SR_PARSE_H */

