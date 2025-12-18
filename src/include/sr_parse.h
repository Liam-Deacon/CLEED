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

/**
 * @file sr_parse.h
 * @brief Small parsing helpers shared by SEARCH utilities.
 *
 * These helpers are intentionally minimal and dependency-free. They are
 * used by refactored SEARCH code to keep parsing logic testable and to
 * reduce cyclomatic complexity in the higher-level routines.
 */

/**
 * @brief Parse two whitespace-delimited @ref real values from a line.
 *
 * Leading whitespace is skipped. Comment lines that begin with `#`
 * (after skipping whitespace) are treated as parse failures.
 *
 * @param line Input line buffer (NUL-terminated).
 * @param out_a Output first parsed value.
 * @param out_b Output second parsed value.
 * @return 0 on success, non-zero on failure.
 */
int sr_parse_two_reals(const char *line, real *out_a, real *out_b);

/**
 * @brief Scan a file until a line containing two @ref real values is found.
 *
 * Comment lines and lines that do not parse are skipped.
 *
 * @param path Path to the input file.
 * @param out_a Output first parsed value.
 * @param out_b Output second parsed value.
 * @return 0 on success, non-zero on failure (including open/read errors).
 */
int sr_read_two_reals_from_file(const char *path, real *out_a, real *out_b);

#ifdef __cplusplus
}
#endif

#endif /* SR_PARSE_H */
