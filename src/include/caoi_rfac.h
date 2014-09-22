/*********************************************************************
ZZ/03.03.2008

include file for angle of incidence additions
 - type definitions
 - constant values
*********************************************************************/
#ifndef RFAC_AOI_H
#define RFAC_AOI_H

#include <string.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef STRSIZE
#define STRSIZE 256
#endif

#ifndef STDERR
#define STDERR stderr
#endif

#define PROG "crfac_aoi"
#define PROG_COPYRIGHT "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2007-2009 Zhasmina Zheleeva" \
"Copyright (C) 2014 Liam Deacon"
#define PROG_LICENSE ""
#define PROG_VERSION ".. Development"
#define PROG_AUTHOR "Georg Held (g.held@reading.ac.uk)"
#define PROG_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define PROG_SHORTDESC "Angle of Incidence (AOI) R-Factor program"

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

/*********************************************************************
include global values for multiple datasets
*********************************************************************/
size_t sa;	  /*! global memory variable for number of aoi datasets */
int ctrinp(char filectr[STRSIZE]);
char proj_name[STRSIZE]; /* global memory variable for storing
													   project name */


/*********************************************************************
END
*********************************************************************/
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

#endif /* RFAC_AOI_H */
