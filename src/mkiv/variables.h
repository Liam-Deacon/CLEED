
/* readvar.h contains the input variable names for readvar.c */

#ifndef _readvar_h_
#define _readvar_h_

#define		VAR_LEN		7	/* number of relevant characters */
#define		VAR_MAX		23	/* number of variables */

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
