
/* readvar.h contains the input variable names for readinp.c */

#ifndef __READ_INP_H__
#define __READ_INP_H__

#define	INP_LEN		7	  /*! number of relevant characters */
#define	INP_MAX		15	/*! number of names */

const char INP[INP_MAX][20] = {
    "REF_NAME",
    "MASK_NAME",
    "NSTART",
    "NSTOP",
    "NSTEP",
    "H_CENTER",
    "V_CENTER",
    "ROUTER",
    "RINNER",
    "BACKGROUND",
    "SMOOTH",
    "KP_LEN_10",
    "NDOM",
    "DOMAIN",
    "NDESIRE",
};

#endif /* __READINP_H__ */
