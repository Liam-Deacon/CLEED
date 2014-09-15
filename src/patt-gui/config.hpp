#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "patt_gui.h"

/* app status */
#define MODE_NORMAL 0
#define MODE_ROTATE 1
#define MODE_TRANSLATE 2
#define MODE_ZOOM 3

/* pattern */

#define SPOT_GS 0
#define SPOT_SS 1
#define STRSZ 128

#define OFF_H      400.
#define OFF_W      300.

#define LINE_WIDTH 2.
#define MAX_RADIUS 200.

#define MAX_INPUT_FILES 6

#define RADIUS_GS 10.
#define GREY_GS    0.

#define RADIUS_SS  5.

/* define postscript colours */
#define PSRED "1 0 0 setrgbcolor"			//red
#define PSDKRED "0.7 0.1 0 setrgbcolor"		//dark red
#define PSGREEN "0 1 0 setrgbcolor"			//green
#define PSBLUE "0 0 1 setrgbcolor"			//blue
#define PSDKBLUE "0 0 0.5 setrgbcolor"		//dark blue
#define PSLTBLUE "0 1 1 setrgbcolor"		//light blue
#define PSMAGENTA "1 0 1 setrgbcolor"		//magenta
#define PSYELLOW "1 1 0 setrgbcolor"		//yellow
#define PSWHITE "1 1 1 setrgbcolor"			//white
#define PSBLACK "0 0 0 setrgbcolor"			//black
#define PSORANGE "1.0 0.7 0.0 setrgbcolor"	//orange
#define PSDKORANGE "1 0.33 0 setrgbcolor"	//dark orange
#define PSPURPLE "0.7 0.3 1.0 setrgbcolor"	//purple
#define PSBROWN "0.7 0.3 0.0 setrgbcolor"	//brown
#define PSDKGREEN "0.0 0.5 0.0 setrgbcolor"	//dark green
#define PSGREY "0.5 setgray"				//grey
#define PSDKGREY "0.25 setgray"				//dark grey
#define PSLTGREY "0.75 setgray"				//light grey
#define PSGOLD "0.83 0.83 0.17 setrgbcolor" //'gold'
#define PSDKCYAN "0 0.67 0.67 setrgbcolor"  //dark cyan
#define PSCYAN "0.08 0.92 0.92 setrgbcolor" //cyan

#define EDGE_COLOUR "0.55 1 0.55 setrgbcolor"
#define SCREEN_COLOUR "0.85 1 0.85 setrgbcolor"
#define GUN_COLOUR "0 0 0 setrgbcolor"

/* alter the following list for order of preference */
#define NUM_COLOURS 30
#define NUM_GREYS 9
#define NUM_SUBS 54

/* image formats */
#define FMT_BMP 0
#define FMT_JPG 1
#define FMT_PNG 2
#define FMT_PDF 3
#define FMT_SVG 4


#endif // CONFIG_HPP
