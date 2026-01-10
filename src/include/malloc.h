/*********************************************************************
Compatibility shim for platforms that do not provide <malloc.h>.

Many legacy CLEED sources include <malloc.h> for malloc/free prototypes.
Modern macOS toolchains do not ship that header; the standard declarations
live in <stdlib.h>.
*********************************************************************/

#ifndef CLEED_COMPAT_MALLOC_H
#define CLEED_COMPAT_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

// cppcheck-suppress missingIncludeSystem
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#endif
