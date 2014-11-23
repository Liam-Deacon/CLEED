/*********************************************************************
 *                        MKIV_VER.H
 *
 *  Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/02.03.14 - creation
 *********************************************************************/

/*! \file
 *
 * Header contains mkiv version information
 */

#ifndef MKIV_VER_H
#define MKIV_VER_H

#define MKIV "mkiv"
#define MKIV_COPYRIGHT                                                        \
"Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>\n"                     \
"Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>"\
"Copyright 2013-2014 Liam Deacon <liam.deacon@diamond.ac.uk>"
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
