/************************************************************************
 *                            FTSMOOTH_VER.H
 *
 *  Copyright 1995-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 ************************************************************************/

/*! \file
 *
 * Header file for \c ftsmooth (Fourier Transformation Smooth) versioning info.
 */

#ifndef FTSMOOTH_VER_H
#define FTSMOOTH_VER_H

/* define version information */
static const char FTSMOOTH_PROG[] = "ftsmooth";
static const char FTSMOOTH_PROG_COPYRIGHT[] = "Copyright (C) 2008-2014 Georg Held\n" \
"Copyright (C) 2014 Liam Deacon";
static const char FTSMOOTH_PROG_LICENSE[] = "GNU General Public License v3";
static const char FTSMOOTH_PROG_VERSION[] = "2014.07.04 Development";
static const char FTSMOOTH_PROG_AUTHOR[] = "Georg Held (g.held@reading.ac.uk)";
static const char FTSMOOTH_PROG_MAINTAINER[] = "Liam Deacon <liam.m.deacon@gmail.com>";
static const char FTSMOOTH_PROG_SHORTDESC[] = "Manipulate IV curves";

/* platform information */
#if defined(WIN32) || defined(_WIN32) \
                   || defined(__WIN32) || defined(__WIN32__)
static const char FTSMOOTH_PROG_PLAT[] = "Win32";
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
static const char FTSMOOTH_PROG_PLAT[] = "Win32_Cygwin";
#elif defined(__ANDROID__)
static const char FTSMOOTH_PROG_PLAT[] = "Android";
#elif defined(__linux__) || defined(__linux)
static const char FTSMOOTH_PROG_PLAT[] = "Linux";
#else
static const char FTSMOOTH_PROG_PLAT[] = "Unix";
#endif

#endif /* FTSMOOTH_VER_H */

