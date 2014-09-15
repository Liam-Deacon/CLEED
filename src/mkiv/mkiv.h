#ifndef _mkiv_h_
#define _mkiv_h_

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

#include <sys/file.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>

/* image matrix using libtiff */
#include <tiffio.h>

void mkiv_usage(FILE *output);

void mkiv_info();

#endif
