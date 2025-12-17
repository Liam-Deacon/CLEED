#ifndef CLEED_STRING_H
#define CLEED_STRING_H

/*
 * Small, reusable string helpers to avoid common null-termination pitfalls in
 * legacy code paths (and to keep Codacy/MSVC happy).
 */

static inline size_t cleed_strnlen(const char *s, size_t maxlen)
{
  size_t i;

  if (!s)
    return 0;

  for (i = 0; i < maxlen; i++)
  {
    if (s[i] == '\0')
      return i;
  }

  return maxlen;
}

static inline size_t cleed_strlcpy(char *dst, const char *src, size_t dstsize)
{
  size_t i = 0;

  if (!dst || !dstsize)
    return 0;

  if (!src)
  {
    dst[0] = '\0';
    return 0;
  }

  while (i + 1 < dstsize && src[i] != '\0')
  {
    dst[i] = src[i];
    i++;
  }

  dst[i] = '\0';
  return i;
}

#endif /* CLEED_STRING_H */
