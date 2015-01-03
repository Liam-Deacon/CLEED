/*********************************************************************
LD/24.04.14
  
  file contains mkiv version information
  
Changes:

*********************************************************************/

#ifndef MKIV_VER_H
#define MKIV_VER_H

#define MKIV "mkiv"
#define MKIV_COPYRIGHT "Copyright unknown"
#define MKIV_LICENSE "GNU General Public License v3"
#define MKIV_VERSION "2014.07.04 Development"
#define MKIV_MAINTAINER "Liam Deacon <liam.deacon@diamond.ac.uk>" 
#define MKIV_SHORTDESC "Extract IV curves from a series of LEED images"
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
#define PROG_PLAT "Win32"
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#define PROG_PLAT "Win32_Cygwin"
#elif defined(__linux__) || defined(__linux)
#define PROG_PLAT "Linux"
#else
#define PROG_PLAT "Unix"
#endif

#endif /* MKIV_VER_H */
