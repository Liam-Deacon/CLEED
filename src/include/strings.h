#ifndef CLEED_COMPAT_STRINGS_H
#define CLEED_COMPAT_STRINGS_H

/*
 * Compatibility shim for platforms that don't provide <strings.h> (notably
 * MSVC/Windows). Many legacy CLEED sources include <strings.h> primarily for
 * strcasecmp/strncasecmp.
 */

#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CLEED_COMPAT_STRINGS_H */

