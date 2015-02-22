
/* mkiv_read_var.h contains the input variable names for mkiv_read_inp.c */

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

#endif /* __mkiv_read_inp_H__ */
