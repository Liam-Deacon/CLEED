#include <stdio.h>

/* add lattice vector postscript subroutines */
void patt_draw_ps_vectors_funcs(FILE *file_ptr)
{
  /* print section banner */
  fprintf(file_ptr, "%%=======================================================================\n");
  fprintf(file_ptr, "%%Lattice vector subroutines:\n");
  fprintf(file_ptr, "%%=======================================================================\n");
  
  /* add text angle PS subroutine */
  fprintf(file_ptr, "/textangle { %%returns correct angle for text\n");
  fprintf(file_ptr, "dup 90 gt { %%>90\n");
  fprintf(file_ptr, " dup 180 gt \n");
  fprintf(file_ptr, " { %%>180\n");
  fprintf(file_ptr, " dup 270 gt \n");
  fprintf(file_ptr, "     {dup pop dup 360 sub} %%>270\n");
  fprintf(file_ptr, "     {dup pop dup 180 sub} %%> 180\n");
  fprintf(file_ptr, "     ifelse\n");
  fprintf(file_ptr, " }\n");
  fprintf(file_ptr, " {dup pop dup 180 sub} %%90 < x < 180\n");
  fprintf(file_ptr, " ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "{dup pop dup}\n");
  fprintf(file_ptr, "ifelse\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "%%\n");

  /* add rotated text PS subroutine */
  fprintf(file_ptr, "/positiontext { %%calculate text offset\n");
  fprintf(file_ptr, "2 dict begin\n");
  fprintf(file_ptr, "/sf exch def\n");
  fprintf(file_ptr, "/y1 exch def\n");
  fprintf(file_ptr, "/x1 exch def \n");
  fprintf(file_ptr, "y1 0 gt \n");
  fprintf(file_ptr, "{ %%y +ve\n");
  fprintf(file_ptr, " x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, " {x1 sf sub y1 0.95 mul sf add moveto} %%x +ve\n");
  fprintf(file_ptr, " {\n");
  fprintf(file_ptr, "   x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "   {x1 1.25 mul y1 1.1 mul 5 add moveto} %%x -ve\n");
  fprintf(file_ptr, "   {x1 5 sf mul sub y1 0.95 mul 5 sf mul add moveto} %%special case x=0\n");
  fprintf(file_ptr, "   ifelse\n");
  fprintf(file_ptr, "	}\n");
  fprintf(file_ptr, "	ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "{\n");
  fprintf(file_ptr, "	y1 0 lt\n");
  fprintf(file_ptr, "	{ %%y -ve\n");
  fprintf(file_ptr, "	  x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, "	  {x1 sf 0.5 mul add y1 0.65 mul sf 2 mul add moveto} %%x +ve\n");
  fprintf(file_ptr, "	  {\n");
  fprintf(file_ptr, "	    x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "	    {x1 1.30 mul y1 0.8 mul moveto} %%x -ve\n");
  fprintf(file_ptr, "	    {x1 5 sf mul sub y1 0.95 mul 5 add moveto} %%special case x=0\n");
  fprintf(file_ptr, "	    ifelse\n");
  fprintf(file_ptr, "	  }\n");
  fprintf(file_ptr, "	  ifelse\n");
  fprintf(file_ptr, "	}\n");
  fprintf(file_ptr, "    { %%********* special case: y=0 ***********%%\n");
  fprintf(file_ptr, "	  x1 0 gt %%x +ve?  \n");
  fprintf(file_ptr, "	  {x1 sf mul 6 sub y1 0.95 mul 5 sf mul add moveto} %%x +ve\n");
  fprintf(file_ptr, "	  {\n");
  fprintf(file_ptr, "	    x1 0 lt %%x -ve?  \n");
  fprintf(file_ptr, "	    {x1 sf 5 mul sub y1 0.95 mul 5 sf mul add moveto} %%x -ve\n");
  fprintf(file_ptr, "	    if\n");
  fprintf(file_ptr, "	  }\n");
  fprintf(file_ptr, "	  ifelse\n");
  fprintf(file_ptr, "	}\n");
  fprintf(file_ptr, "    ifelse\n");
  fprintf(file_ptr, "}\n");
  fprintf(file_ptr, "ifelse\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "\n");
  fprintf(file_ptr, "%%\n");

  /* add rotate label subroutine */
  fprintf(file_ptr,
      /* string split into segments as max length supported by C90 is 509 */
      "/rotatedlabel { %%adds rotated label with subscript text\n"
      "8 dict begin\n"
      "/subfontsize exch def\n"
      "/fontsize exch def\n"
      "/subtext exch def\n"
      "/text exch def\n"
      "/y1 exch def\n"
      "/x1 exch def\n"
      "/y0 exch def\n"
      "/x0 exch def\n"
      "gsave %%save coordinate system - important! \n"
      "x1 x0 sub 2 div 0.8 mul y1 y0 sub 2 div moveto %%move to label pos\n"
      "currentpoint fontsize 10 div positiontext\n"
      "y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for text\n"
      "/Times-Bold findfont fontsize scalefont setfont text show\n");
  fprintf(file_ptr,
      "currentpoint translate gsave %%translate for easy subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "0 subfontsize 8 div 0 subfontsize 5 div sub moveto "
      "%%offset y for subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for subscript text\n"
      "/Times-Bold findfont subfontsize scalefont setfont subtext show\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "grestore 0 fontsize subfontsize div 2 add moveto "
      "%%offset y for superscript\n");
  fprintf(file_ptr,
      "/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show\n"
      "grestore %%restore original coordinate system\n"
      "} def\n"
      "%%\n");
  
  /* add arrowhead PS subroutine */
  fprintf(file_ptr, "/arrowhead {%% stack: s x1 y1, current point: x0 y0\n");
  fprintf(file_ptr, "gsave\n");
  fprintf(file_ptr, "currentpoint %% s x1 y1 x0 y0\n");
  fprintf(file_ptr, "4 2 roll exch %% s x0 y0 y1 x1\n");
  fprintf(file_ptr, "4 -1 roll exch %% s y0 y1 x0 x1\n");
  fprintf(file_ptr, "sub 3 1 roll %% s x1-x2 y0 y1\n");
  fprintf(file_ptr, "sub exch %% s y0-y1 x1-x2\n");
  fprintf(file_ptr, "atan rotate %% rotate over arctan((y0-y1)/(x1-x2))\n");
  fprintf(file_ptr, "dup scale %% scale by factor s\n");
  fprintf(file_ptr, "-7 2 rlineto 1 -2 rlineto -1 -2 rlineto\n");
  fprintf(file_ptr, "closepath fill %% fill arrowhead\n");
  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "newpath\n");
  fprintf(file_ptr, "} def\n");
  fprintf(file_ptr, "%%\n");
  
  /* add arrow PS subroutine */
  fprintf(file_ptr, "/arrow {\n");
  fprintf(file_ptr, "5 dict begin\n");
  fprintf(file_ptr, "/arrowsize exch def\n");
  fprintf(file_ptr, "/y1 exch def\n");
  fprintf(file_ptr, "/x1 exch def\n");
  fprintf(file_ptr, "/y0 exch def\n");
  fprintf(file_ptr, "/x0 exch def\n");
  fprintf(file_ptr, "gsave \n");
  fprintf(file_ptr, "arrowsize setlinewidth\n");
  fprintf(file_ptr, "x0 y0 moveto x1 0.95 mul y1 0.95 mul lineto stroke\n");
  fprintf(file_ptr, "x1 y1 moveto arrowsize x0 y0 arrowhead\n");
  fprintf(file_ptr, "grestore\n");
  fprintf(file_ptr, "} def\n%%\n");

}



