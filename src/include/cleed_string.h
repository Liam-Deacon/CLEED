#ifndef CLEED_STRING_H
#define CLEED_STRING_H

/*
 * Small, reusable string helpers to avoid common null-termination pitfalls in
 * legacy code paths (and to keep Codacy/MSVC happy).
 */

// cppcheck-suppress missingIncludeSystem
#include <stddef.h>
// cppcheck-suppress missingIncludeSystem
#include <string.h>

static inline size_t cleed_strnlen(const char *s, size_t maxlen)
{
  if (!s)
    return 0;
  const void *terminator = memchr(s, '\0', maxlen);
  return terminator ? (size_t)((const char *)terminator - s) : maxlen;
}

static inline size_t cleed_strlcpy(char *dst, const char *src, size_t dstsize)
{
  if (!dst || !dstsize)
    return 0;
  if (!src)
  {
    dst[0] = '\0';
    return 0;
  }

  const size_t copy_len = cleed_strnlen(src, dstsize - 1);
  memcpy(dst, src, copy_len);
  dst[copy_len] = '\0';
  return copy_len;
}

#endif /* CLEED_STRING_H */
