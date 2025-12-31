/*********************************************************************
 *                       CLEED_IO.H
 *
 *  I/O helpers for CLEED curves (stub layer).
 *********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef CLEED_IO_H
#define CLEED_IO_H

#include "cleed_core.h"

/**
 * @brief Read a curve from disk.
 *
 * @return 0 on success, non-zero on failure.
 */
int cleed_io_read_curve(const char *path, cleed_curve *out_curve);

/**
 * @brief Write a curve to disk.
 *
 * @return 0 on success, non-zero on failure.
 */
int cleed_io_write_curve(const char *path, const cleed_curve *curve);

#endif /* CLEED_IO_H */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
