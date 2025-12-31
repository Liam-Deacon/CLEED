/*********************************************************************
 *                       CLEED_IO.C
 *
 *  Stub I/O layer for CLEED curve data.
 *********************************************************************/

// cppcheck-suppress missingIncludeSystem
#include <errno.h>

#include "cleed_io.h"

int cleed_io_read_curve(const char *path, cleed_curve *out_curve)
{
  (void)path;
  (void)out_curve;
  errno = ENOSYS;
  return -1;
}

int cleed_io_write_curve(const char *path, const cleed_curve *curve)
{
  (void)path;
  (void)curve;
  errno = ENOSYS;
  return -1;
}
