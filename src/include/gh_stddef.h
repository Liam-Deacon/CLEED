/*********************************************************************
GH/03.09.97

constants and macros generally used
*********************************************************************/

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef STD_DEF_H
#define STD_DEF_H

/*********************************************************************
 define machine
*********************************************************************/

/* Alternatives:
#define DEC
#define IBM
*/

#define IBM
/*
#define free(x) free((void *)(x))
*/

/*********************************************************************
*********************************************************************/

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

/*********************************************************************
 output channels
*********************************************************************/

#define STDOUT stdout
#define STDERR stderr
#define STDWAR stderr
#define STDCTR stdout
#define STDCPU stdout

/*********************************************************************
 printing verbosity
*********************************************************************/

#ifndef WARNING
#define WARNING
#endif

#ifndef ERROR
#define ERROR
#endif

#ifndef EXIT_ON_ERROR
#define EXIT_ON_ERROR
#endif

#ifdef DEBUG
#define CONTROL_ALL
#endif

#ifdef CONTROL_ALL
#define CONTROL
#define CONTROL_X
#define CONTROL_FLOW
#define CONTROL_IO
#define CONTROL_MBG
#define CONTROL_LSUM
#define CONTROL_MATB
#define CONTROL_ALL
#endif

#ifndef CONTROL
#define CONTROL 0
#endif

#ifndef CONTROL_X
#define CONTROL_X 0
#endif

/*********************************************************************
 general mathematical definitions / constants
*********************************************************************/

#ifdef _XOPEN_SOURCE
#define PI  M_PI
#else
#define PI  3.1415926535897932385
#endif
/*
*/

#define DEG_TO_RAD 0.017453293         /* conversion degree to radian */
#define RAD_TO_DEG 57.29578            /* conversion radian to degree */

#define M2_H      0.2631894506957162   /* 2*m/h       [eV^-1   A^-2] */
#define SQRT_M2_H 0.5130199320647456   /* sqrt(2*m/h) [eV^-0.5 A^-1] */

/*********************************************************************
 general other definitions / constants
*********************************************************************/

#define KBYTE 1024
#define MBYTE 1048576

/*********************************************************************
 special definitions
*********************************************************************/

#define STRSZ 256                /* maximum length of strings */

#define I_END_OF_LIST   -9999    /* list terminator (integer)*/
#define F_END_OF_LIST   -9999.   /* list terminator (float)  */

#define IEND_OF_LIST   I_END_OF_LIST    /* alias for list terminator (integer)*/
#define FEND_OF_LIST   F_END_OF_LIST    /* alias for list terminator (float)  */

/*********************************************************************
 macros:
*********************************************************************/

#define MAX(x,y)  ((x)>(y))?(x):(y) 
#define MIN(x,y)  ((x)<(y))?(x):(y) 
#define SQUARE(x) (x)*(x)

#define ODD(n)    ((n)%2)
#define M1P(n)    (((n)%2)?(-1.):(1.))

/* basic logging helpers */
#ifndef ERROR_MSG
#define ERROR_MSG(...) fprintf(STDERR, __VA_ARGS__)
#endif

#ifndef WARNING_MSG
#define WARNING_MSG(...) fprintf(STDWAR, __VA_ARGS__)
#endif

#ifndef CONTROL_MSG
#ifdef CONTROL
#define CONTROL_MSG(level, ...) fprintf(STDCTR, __VA_ARGS__)
#else
#define CONTROL_MSG(level, ...) (void)(level)
#endif
#endif

#ifndef CLEED_SSCANF
#define CLEED_SSCANF(buf, fmt, ...) sscanf((buf), (fmt), __VA_ARGS__)
#endif

/* minimal allocation helper */
#ifndef CLEED_ALLOC_CHECK
#define CLEED_ALLOC_CHECK(expr)                                    \
  do                                                               \
  {                                                                \
    if ((expr) == NULL)                                            \
    {                                                              \
      ERROR_MSG("failed to allocate memory for '%s'\\n", #expr);   \
      exit(1);                                                     \
    }                                                              \
  } while(0)
#endif

#ifndef CLEED_REALLOC
#define CLEED_REALLOC(ptr, new_size)                               \
  do                                                               \
  {                                                                \
    void *tmp_ptr = realloc((ptr), (new_size));                    \
    if (tmp_ptr != NULL)                                           \
      (ptr) = tmp_ptr;                                             \
    else                                                           \
    {                                                              \
      ERROR_MSG("could not reallocate memory for '%s'\\n", #ptr);   \
      exit(1);                                                     \
    }                                                              \
  } while(0)
#endif


#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif
