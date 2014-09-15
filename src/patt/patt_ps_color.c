#include "patt.h"
#include <stdlib.h>
#include <strings.h>

/* postscript color selection */
char *ps_color(int color_flag, int ifile, int nfiles, 
               int spot_type, int i_dom) 
{

  const char colors[NUM_COLORS][STRSZ] = {
      PSRED, PSBLUE, PSGREEN, PSMAGENTA, PSLTBLUE, PSDKGREEN,
      PSORANGE, PSCYAN, PSYELLOW, PSBROWN, PSPURPLE, PSGREY,
      PSDKRED, PSDKCYAN, PSDKGREY, PSDKORANGE, PSLTBLUE, PSGOLD,
      /* repeat for now as not enough colours */
      PSRED, PSBLUE, PSGREEN, PSMAGENTA, PSLTBLUE, PSDKGREEN,
      PSORANGE, PSCYAN, PSYELLOW, PSBROWN, PSPURPLE, PSGREY
  };
  
  const char file_colors[6][2][STRSZ] = {
      {PSBLACK, PSDKGREY}, {PSDKRED, PSRED},
      {PSBLUE, PSLTBLUE}, {PSDKGREEN, PSGREEN},
      {PSDKORANGE, PSORANGE}, {PSGOLD, PSYELLOW}
  };
  
  const char greyscale[NUM_GREYS][STRSZ] = {
      "0.1 setgray", "0.2 setgray", "0.3 setgray", 
      "0.4 setgray", "0.5 setgray", "0.6 setgray",
      "0.7 setgray", "0.8 setgray", "0.9 setgray"
  };
  
  /* map to postscript symbol font */
  const char substitutes[NUM_SUBS][2][STRSZ] = {
   { "SQRT", "\\326" }, { "√", "\\326" },
   { "DEGREES", "\\260" }, { "°", "\\260" },
   { "MUL", "\\264" }, { "×", "\\264" },
   { "ALPHA", "\\141" }, { "α", "\\141" },
   { "BETA", "\\142" }, { "β", "\\142" },
   { "GAMMA", "\\147" }, { "γ", "\\147" },
   { "ALT-GAMMA", "\\107" }, { "Γ", "\\107" },
   { "DELTA", "\\104" }, { "Δ", "\\104" },
   { "ALT-DELTA", "\\144" }, { "δ", "\\144" },
   { "THETA", "\\161" }, { "θ", "\\161" },
   { "ALT-THETA", "\\112" }, { "Θ", "\\112" },
   { "PHI", "\\146" }, { "Φ", "\\146" },
   { "ALT-PHI", "\\152" }, { "φ", "\\152" },
   { "LAMBDA", "\\154" }, { "λ", "\\154" },
   { "KAPPA", "\\153" }, { "κ", "\\153" },
   { "PLUSMINUS", "\\261" }, { "±", "\\261" },
   { "PROPORTIONAL", "\\265" }, { "∝", "\\265" },
   { "OMEGA", "\\127" }, { "Ω", "\\127" },
   { "ALT-OMEGA", "\\167" }, { "ω", "\\167" },
   { "MU", "\\155" }, { "μ", "\\155" },
   { "NU", "\\156" }, { "ν", "\\156" },
   { "NOTEQ", "\\271" }, { "!=", "\\271" }, { "≠", "\\271" },
   { "APPROXEQ", "\\273" }, { "~=", "\\273" }, { "≈", "\\273" },
   { "PI", "\\160" }, { "π", "\\160" },
   { "PSI", "\\131" }, { "Ψ", "\\131" },
   { "ALT-PSI", "\\171" }, { "ψ", "\\171" }
  };
  
  char *return_string;
 
  if (!spot_type) /* GS spots = 0 */
  {
    if (nfiles > 0) 
    {
	  switch (color_flag)
	  {
	    case PATT_GRAYSCALE: /* greyscale */
		  return greyscale[ifile % (NUM_GREYS-1)];
		  break;
		
	    case PATT_BLACK_ONLY: /* no color */
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	
	    case PATT_COLOR:
		  return file_colors[ifile % 5][spot_type];
		  break;
		
  	    default:
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	   }
    } 
    else 
    {
      return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
      strcpy(return_string, PSBLACK);
      return return_string; /* black if 1st set of GS spots */
    }

  } 
  else /* SS spots */
  {
    if(nfiles == 0)
    {
      switch (color_flag)
	  {
	    case PATT_GRAYSCALE: /* greyscale */
  		  return greyscale[i_dom % (NUM_GREYS-1)];
		  break;
		
	    case PATT_BLACK_ONLY: /* no color */
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	
	    case PATT_COLOR:
		  return colors[i_dom % (NUM_COLORS-1)];
		  break;
		
	    default:
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	  }
    } 
    else 
    {
      switch (color_flag)
	  {
	    case PATT_GRAYSCALE: /* greyscale */
  		  return greyscale[ifile % (NUM_GREYS-1)];
		  break;
		
	    case PATT_BLACK_ONLY: /* no color */
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	
	    case PATT_COLOR:
		  return file_colors[ifile % 5][spot_type];
		  break;
		
	    default:
          return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
          strcpy(return_string, PSBLACK);
	      return return_string;
		  break;
	  }    
    }
  }
 
  /* default if no color selected */
  return_string = (char*) malloc(sizeof(char) * strlen(PSBLACK));
  strcpy(return_string, PSBLACK);
  
  return return_string; 
}

/*========================================================================*/