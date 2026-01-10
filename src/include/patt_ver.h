/****************************************************************************
                        PATT_VER.H 
                        
    version information for patt

Changes:
         
****************************************************************************/

#ifndef PATT_VER_H
#define PATT_VER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "patt_def.h"

/* additional program information */
#define PROG "PATTERN"
#define PROG_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 2013-2014 Liam Deacon"
#define PROG_LICENSE ""
#define PROG_VERSION "2014.04.27 "
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon (liam.deacon@diamond.ac.uk)" 
#define PROG_SHORTDESC "Generate LEED patterns"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define PROG_PLAT "Linux"
#else
#define PROG_PLAT "Unix"
#endif

#ifdef __cplusplus
}
#endif

#endif /* PATT_VER_H */
