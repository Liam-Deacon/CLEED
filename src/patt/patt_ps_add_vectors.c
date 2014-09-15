#include <stdio.h>

/* add lattice vector postscript subroutines */
void ps_add_vectors(FILE *out_stream)
{
  /* print section banner */
  fprintf(out_stream,"%%=======================================================================\n");
  fprintf(out_stream,"%%Lattice vector subroutines:\n");
  fprintf(out_stream,"%%=======================================================================\n");
  
  /* add text angle PS subroutine */
  fprintf(out_stream,"/textangle { %%returns correct angle for text\n");
  fprintf(out_stream,"dup 90 gt { %%>90\n");
  fprintf(out_stream,"	dup 180 gt \n");
  fprintf(out_stream,"	{ %%>180\n");
  fprintf(out_stream,"		dup 270 gt \n");
  fprintf(out_stream,"			{dup pop dup 360 sub} %%>270\n");
  fprintf(out_stream,"			{dup pop dup 180 sub} %%> 180\n");
  fprintf(out_stream,"			ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"	{dup pop dup 180 sub} %%90 < x < 180\n");
  fprintf(out_stream,"	ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"{dup pop dup}\n");
  fprintf(out_stream,"ifelse\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"%%\n");

  /* add rotated text PS subroutine */
  fprintf(out_stream,"/positiontext { %%calculate text offset\n");
  fprintf(out_stream,"2 dict begin\n");
  fprintf(out_stream,"/sf exch def\n");
  fprintf(out_stream,"/y1 exch def\n");
  fprintf(out_stream,"/x1 exch def \n");
  fprintf(out_stream,"y1 0 gt \n");
  fprintf(out_stream,"{ %%y +ve\n");
  fprintf(out_stream,"	x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	{x1 sf sub y1 0.95 mul sf add moveto} %%x +ve\n");
  fprintf(out_stream,"	{\n");
  fprintf(out_stream,"	  x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	  {x1 1.25 mul y1 1.1 mul 5 add moveto} %%x -ve\n");
  fprintf(out_stream,"	  {x1 5 sf mul sub y1 0.95 mul 5 sf mul add moveto} %%special case x=0\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"	ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"{\n");
  fprintf(out_stream,"	y1 0 lt\n");
  fprintf(out_stream,"	{ %%y -ve\n");
  fprintf(out_stream,"	  x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	  {x1 sf 0.5 mul add y1 0.65 mul sf 2 mul add moveto} %%x +ve\n");
  fprintf(out_stream,"	  {\n");
  fprintf(out_stream,"	    x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	    {x1 1.30 mul y1 0.8 mul moveto} %%x -ve\n");
  fprintf(out_stream,"	    {x1 5 sf mul sub y1 0.95 mul 5 add moveto} %%special case x=0\n");
  fprintf(out_stream,"	    ifelse\n");
  fprintf(out_stream,"	  }\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"    { %%********* special case: y=0 ***********%%\n");
  fprintf(out_stream,"	  x1 0 gt %%x +ve?  \n");
  fprintf(out_stream,"	  {x1 sf mul 6 sub y1 0.95 mul 5 sf mul add moveto} %%x +ve\n");
  fprintf(out_stream,"	  {\n");
  fprintf(out_stream,"	    x1 0 lt %%x -ve?  \n");
  fprintf(out_stream,"	    {x1 sf 5 mul sub y1 0.95 mul 5 sf mul add moveto} %%x -ve\n");
  fprintf(out_stream,"	    if\n");
  fprintf(out_stream,"	  }\n");
  fprintf(out_stream,"	  ifelse\n");
  fprintf(out_stream,"	}\n");
  fprintf(out_stream,"    ifelse\n");
  fprintf(out_stream,"}\n");
  fprintf(out_stream,"ifelse\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"\n");
  fprintf(out_stream,"%%\n");
  
  /* add rotate label subroutine */
  fprintf(out_stream, 
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
  fprintf(out_stream,
      "currentpoint translate gsave %%translate for easy subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "0 subfontsize 8 div 0 subfontsize 5 div sub moveto "
      "%%offset y for subscript\n"
      "y1 y0 sub x1 x0 sub atan textangle rotate %%rotate for subscript text\n"
      "/Times-Bold findfont subfontsize scalefont setfont subtext show\n"
      "y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %%rotate back \n"
      "grestore 0 fontsize subfontsize div 2 add moveto "
      "%%offset y for superscript\n");
  fprintf(out_stream,
      "/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show\n"
      "grestore %%restore original coordinate system\n"
      "} def\n"
      "%%\n");
  
  /* add arrowhead PS subroutine */
  fprintf(out_stream,"/arrowhead {%% stack: s x1 y1, current point: x0 y0\n");
  fprintf(out_stream,"gsave\n");
  fprintf(out_stream,"currentpoint %% s x1 y1 x0 y0\n");
  fprintf(out_stream,"4 2 roll exch %% s x0 y0 y1 x1\n");
  fprintf(out_stream,"4 -1 roll exch %% s y0 y1 x0 x1\n");
  fprintf(out_stream,"sub 3 1 roll %% s x1-x2 y0 y1\n");
  fprintf(out_stream,"sub exch %% s y0-y1 x1-x2\n");
  fprintf(out_stream,"atan rotate %% rotate over arctan((y0-y1)/(x1-x2))\n");
  fprintf(out_stream,"dup scale %% scale by factor s\n");
  fprintf(out_stream,"-7 2 rlineto 1 -2 rlineto -1 -2 rlineto\n");
  fprintf(out_stream,"closepath fill %% fill arrowhead\n");
  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"newpath\n");
  fprintf(out_stream,"} def\n");
  fprintf(out_stream,"%%\n");
  
  /* add arrow PS subroutine */
  fprintf(out_stream,"/arrow {\n");
  fprintf(out_stream,"5 dict begin\n");
  fprintf(out_stream,"/arrowsize exch def\n");
  fprintf(out_stream,"/y1 exch def\n");
  fprintf(out_stream,"/x1 exch def\n");
  fprintf(out_stream,"/y0 exch def\n");
  fprintf(out_stream,"/x0 exch def\n");
  fprintf(out_stream,"gsave \n");
  fprintf(out_stream,"arrowsize setlinewidth\n");
  fprintf(out_stream,"x0 y0 moveto x1 0.95 mul y1 0.95 mul lineto stroke\n");
  fprintf(out_stream,"x1 y1 moveto arrowsize x0 y0 arrowhead\n");
  fprintf(out_stream,"grestore\n");
  fprintf(out_stream,"} def\n%%\n");

}



