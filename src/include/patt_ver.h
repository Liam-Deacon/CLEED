/****************************************************************************
                        PATT_PROG_VER.H
                        
    version information for patt

Changes:
         
****************************************************************************/

#ifndef PATT_PROG_VER_H
#define PATT_PROG_VER_H

#include "patt_def.h"

/* additional program information */
#define PATT_PROG "PATT_PROGERN"
#define PATT_PROG_COPYRIGHT "Copyright (C) 1994-2014 Georg Held\n" \
"Copyright (C) 2013-2014 Liam Deacon"
#define PATT_PROG_LICENSE "GNU General Public License v3.0"
#define PATT_PROG_VERSION "2014.04.27 "
#define PATT_PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PATT_PROG_MAINTAINER "Liam Deacon (liam.deacon@diamond.ac.uk)"
#define PATT_PROG_SHORTDESC "Generate LEED patterns"
#define PATT_PROG_NOTES "Pattern is part of the CLEED package by Georg Held"\
"\n\t(contact g.held@reading.ac.uk)"

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PATT_PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PATT_PROG_PLAT "Win32_Cygwin"
#elif defined(__ANDROID__)
#define PATT_PROG_PLAT "Android"
#elif defined(__linux__) || defined(__linux)
#define PATT_PROG_PLAT "Linux"
#else
#define PATT_PROG_PLAT "Unix"
#endif

#endif /* PATT_PROG_VER_H */
