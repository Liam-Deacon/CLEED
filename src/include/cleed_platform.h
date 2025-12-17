#ifndef CLEED_PLATFORM_H
#define CLEED_PLATFORM_H

#include "cleed_string.h"

#ifdef _WIN32
// cppcheck-suppress missingIncludeSystem
#include <windows.h>

static inline void cleed_get_hostname(char *buffer, size_t buffer_size)
{
  if (!buffer || !buffer_size)
    return;

  DWORD hostname_size = (DWORD)buffer_size;
  if (!GetComputerNameA(buffer, &hostname_size))
  {
    cleed_strlcpy(buffer, "unknown", buffer_size);
  }
}
#else
// cppcheck-suppress missingIncludeSystem
#include <unistd.h>

static inline void cleed_get_hostname(char *buffer, size_t buffer_size)
{
  if (!buffer || !buffer_size)
    return;

  if (gethostname(buffer, buffer_size) != 0)
  {
    cleed_strlcpy(buffer, "unknown", buffer_size);
  }
  else
  {
    buffer[buffer_size - 1] = '\0';
  }
}
#endif

#endif /* CLEED_PLATFORM_H */

