/*********************************************************************
 *                             MKIV.H
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 1993-2014 Christian Stellwag <leed@iron.E20.physik.tu-muenchen.de>
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *
 * Master include for the \c mkiv program.
 */

#ifndef _MKIV_H_
#define _MKIV_H_

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <tiffio.h>
#include "mkiv_def.h"
#include "mkiv_funcs.h"

#if defined(_WIN32) || defined(WIN32)
#include "pwd.h"     /* local Windows approximant */
#include <time.h>
#else   /* Unix-derived */
#include <pwd.h>
#include <sys/dir.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <sys/file.h>
#include <sys/time.h>
#include <strings.h>
#endif

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

/* handle images internally using libtiff */
#include <tiffio.h>

#endif
