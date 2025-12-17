#ifndef CLEED_COMPAT_STRINGS_H
#define CLEED_COMPAT_STRINGS_H

/*
 * Compatibility shim for platforms that don't provide <strings.h> (notably
 * MSVC/Windows). Many legacy CLEED sources include <strings.h> primarily for
 * strcasecmp/strncasecmp.
 */

#ifdef _MSC_VER
/* MSVC does not provide POSIX strcasecmp/strncasecmp. */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#ifdef __cplusplus
extern "C" {
#endif

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, unsigned long n);

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif

#endif /* CLEED_COMPAT_STRINGS_H */
