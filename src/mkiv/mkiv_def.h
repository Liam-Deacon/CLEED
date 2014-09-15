/**********************************************************************
GH/
CS/28.7.93   PYTH2, PYTH, MIN, MAX
LD/28.02.14
**********************************************************************/

#ifndef MKIV_DEF_H
#define MKIV_DEF_H

#include <tiffio.h>
#include <float.h>
#include <math.h>

/* macros */
#define    ARG_IS(str)     (strcmp(argv[i], str) == 0)
#define    SQUARE(x)       (x)*(x)   /* the brackets are necessary 
                                        because of PYTH and the    
                                        priority of multiplication        */
#define    PYTH2(x,y)      SQUARE(x) + SQUARE(y)
#define    PYTH(x,y)       sqrt( (float) PYTH2(x,y) )
#define    RND(x)          (int)( (x) + .5 )
#define    RNDUP(x)        (int)( (x) + 1. )
#define    NAT(x)          ( fabs((x) - RND(x)) < DBL_EPSILON )
#ifndef    MIN
#define    MIN(x,y)        ((x) < (y)) ? (x) : (y)
#endif
#ifndef    MAX
#define    MAX(x,y)        ((x) > (y)) ? (x) : (y)
#endif
#ifndef    YES
#define    YES             1
#define    NO              0
#endif

#define  ERR_EXIT(text)                                                     \
{                                                                           \
    fprintf(stderr, "*** error " #text "\n");                               \
    exit(1);                                                                \
}

#define  ERR_EXIT_X(text, x)                                                \
{                                                                           \
    fprintf(stderr, "*** error " #text "\n", x);                            \
    exit(1);                                                                \
}

#define STRCPY_ARG(x)                                                       \
if (i+1 < argc) {i++; strcpy(x, argv[i]);}                                  \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}

#define FLOAT_ARG(x)                                                        \
if (i+1 < argc) {i++; x = atof(argv[i]);}                                   \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}

#define INT_ARG(x)                                                          \
if (i+1 < argc) {i++; x = atoi(argv[i]);}                                   \
else {                                                                      \
  fprintf(stderr, "***error (mkiv_main): no argument to %s\n", argv[i]);    \
  exit(-1);                                                                 \
}


/* macros for mkiv.c */
#define  VERBOSE           1
#define  QUICK             2
#define  VPRINT            if ( verb & VERBOSE ) printf
#define  QQ                if ( !(verb & QUICK) )

/* array dimensions */
#define    STRSZ          2048       /* max. string size                  */
#define    I_MAX          2048       /* max. number of IV curves (spots)  */
#define    D_MAX            20       /* max. number of domains            */
#define    E_MAX          2048       /* max. number of Energy points      */

/* adjustable values */
#define    KMAX            5.        /* max. k-value                      */
#define    THICK           1.        /* thickness,                        */
#define    WHITE           255       /* greylevel of circle line          */
#define    TOLERANCE       0.05      /* two identical float values        */

/*  constants  */
#define    PI     3.141592653589793  /* pi                                */
#define    H2M    1.951939668076095  /* h- / sqrt(2*m) in (eV)^-0.5*A     */
#define    K10    2.6771             /* kp_len of (1,0) beam in A^-1      */
#define    ROOT2  1.4142136          /* sqrt(2) */
#define    INT_OUT  -999             /* default value if sum not defined */

/* control bits/values */
#define    BG_YES           1
#define    DEV_BIG          1
#define    DEV_SMALL        0


/* structures */
typedef struct spot
{
    float  kp_len;                  /* length of k parallel to surface   */
    float  lind1;                   /* first index of reflex */
    float  lind2;                   /* second index of reflex */
    float  y0;                      /* calculated vert. reflex position  */
    float  x0;                      /* calculated horiz. reflex position */
    float  yy;                      /* vert. comp. of reflex pos.        */
    float  xx;                      /* hor. comp. of reflex pos.         */
    float  cos_th;                  /* cos of reflex                     */
    float  intensity;               /* intensity of reflex               */
    float  s2n;                     /* signal to noise ratio of reflex   */ 
    float  s2u;		                /* signal to underground ratio       */
    long   control;                 /* control byte */
/* spot->control bits */
#define    SPOT_EXCL        1
#define    SPOT_DESI        2
#define    SPOT_REF         4
#define    SPOT_GOOD_S2N    0x20
#define    SPOT_OUT         0x40
} Spot;

typedef struct lindex
{
     float  lind1;                   /* first index of reflex */
     float  lind2;                   /* second index of reflex */
     long   control;                 /* control byte */
} Lindex;

typedef struct domain
{
    float  lind11;                   /* first index of reflex */
    float  lind12;                   /* second index of reflex */
    float  lind21;                   /* first index of reflex */
    float  lind22;                   /* second index of reflex */
} Domain;

typedef struct coord
{
    int xx;
    int yy;
} Coord;

typedef struct vector
{
    float  len;
    float  xx;
    float  yy;
} Vector;

typedef struct imagematrix 
{
  uint32 rows;
  uint32 cols;
  uint32* imagedata;
} ImageMatrix;

typedef struct _tifvalues
{
    TIFF *tif_in;
    TIFF *tif_out;
    unsigned long imagewidth;
    unsigned long imagelength;
    unsigned short bitspersample;
    unsigned short compression;
    unsigned short photometricinterpretation;
    unsigned long stripoffsets;
    unsigned long rowsperstrip;
    unsigned long stripbytecounts;
    float xresolution;
    float yresolution;
    unsigned short planarconfiguration;
    unsigned short resolutionunit;

    char *buf;

    int ENERGY;
    int CURRENT;
} tifvalues;

#endif /* MKIV_DEF_H */