/************************************************************************                                                               
file name: machdefs.h                             
                                              
contains: machine definitions for correct	      
          byte ordering and known		      
                                              
written by:                                      
                                              
date: 3/5/90                                     
                                              
modifications:				      
                                              
************************************************************************/ 

#ifndef _machdefs_h_
#define _machdefs_h_




#define NUMBER_MACHINES 18

static char *machine_names[] =
{
	"Apollo",
	"Dec 3100/2100, Mips cpu, little endian mode", 
	"Encore, Sequent, NS32000",
        "IBM RT",
        "IBM032",
	"MC680x0",
        "MC68k32",
	"Sony News",
	"Sun 3,  Sun 4, SunSparc",
	"Titan",
	"Vax",
	"Dec 3100/2100, Mips cpu, big endian mode", 
	"MIPS",
	"Silicon Graphics",
	"IEEE Order",
	"NS Order",
	"DEC Order",
        "unknown"   /* this should always be the last entry in the list */
};

static int machine_type[] =
{
	VFF_DEP_IEEEORDER,
	VFF_DEP_NSORDER,
	VFF_DEP_NSORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_DECORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_NSORDER,
	VFF_DEP_IEEEORDER,
	VFF_DEP_DECORDER,
	255   /* this should always be the last entry in the list */
};

#endif
/* do not add after this line */
