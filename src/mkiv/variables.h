
/* mkiv_read_var.h contains the input variable names for mkiv_read_var.c */

#ifndef _mkiv_read_var_h_
#define _mkiv_read_var_h_

enum { VAR_LEN = 7 };	  /*!< number of relevant characters */
enum { VAR_MAX = 23 };	/*!< number of variables */

const char *VAR[VAR_MAX] = {	 
				"COS_MIN",
				"COS_MAX",
				"VERH",
				"ACCI",
				"ACCB",
				"DISTANCE",
				"UPDATE",
				"STEP",
				"TRIP_MAX",
				"SCALE_MIN",
				"RANGE_MIN",
				"S2N_GOOD",
				"S2N_BAD",
				"REF_MIN",
				"SEC_RANGE",
				"S2N_REF",
				"REF_DEVIATION",
				"BAS_RATIO",
				"RANGE",
				"SCALE_A",
				"SCALE_B",
				"ANGLE",
				"RATIO",
	               	     };

#endif
