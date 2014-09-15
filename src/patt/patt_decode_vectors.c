#include "patt.h"
#include <strings.h>
#include <ctype.h>
#include <stdlib.h>

/* decode input string to determine which vectors to draw
 
   Note: more than 10 input files is not currently supported as the conversion
   is done using single characters (i.e. "0"-"9")
*/
int decode_vectors(int spot_type, int *do_vectors, char *argv, int str_length)
{
  int pos, jj, val[2], imax;
  char str[2][STRSZ];
  char ichar[2];
  
  pos = 0;
  val[0] = 0;
  val[1] = 0;
  ichar[1] = '\0';
  
  if (spot_type == SPOT_GS)
	imax = MAX_INPUT_FILES;
  if (spot_type == SPOT_SS)
    imax = STRSZ;
	
	while ((argv[pos] != '\0') && (pos < str_length))
	{
		strcpy(str[0], "\0");
		strcpy(str[1], "\0");
		
		while (isdigit(argv[pos]) && (pos < str_length))
		{
		  ichar[0] = argv[pos]; 
		  strcat(str[0], ichar);
		  pos++;
		}
		val[0] = abs(atoi(str[0]));
		
		if ((argv[pos] == ',') || (argv[pos] == '\0'))
		  if (val[0] < imax)
		  {
			do_vectors[val[0]] = 1;
		  }
			
		if (argv[pos] == '-')
		{
		  pos++;
  		  while (isdigit(argv[pos]) && (pos < str_length))
		  {
		    ichar[0] = argv[pos];
		    strcat(str[1], ichar);		  
			pos++;
		  }
		  val[1] = abs(atoi(str[1])); jj=0;
		  if ((val[0] < imax) && (val[1] < imax))
		    while (jj <= val[1])
		    {
			  do_vectors[jj] = 1;
			  jj++;
		    }
		}
			
		if (argv[pos] == '*') 
		  for (jj=val[0];jj<imax;jj++) 
		    do_vectors[jj] = 1;
			
		pos++;
	}
	
/*
currentpoint translate 0 5 sub 0 moveto
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for text
/Times-Bold findfont fontsize scalefont setfont text show
currentpoint translate %translate for easy subscript
y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %rotate back 
0 subfontsize 8 div 0 subfontsize 5 div sub moveto %offset y for subscript
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for subscript text
/Times-Bold findfont subfontsize scalefont setfont subtext show
y1 y0 sub x1 x0 sub atan textangle -1 mul rotate %rotate back
currentpoint translate 0 4 moveto %offset y for superscript
y1 y0 sub x1 x0 sub atan textangle rotate %rotate for superscript text
/Times-Bold findfont fontsize 0.8 mul scalefont setfont (*) show*/
	
  
  return 0;
}