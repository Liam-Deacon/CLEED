#include "patt.h"
#include <strings.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

/* decode input string to determine which vectors to draw
 
   Note: more than 10 input files is not currently supported as the conversion
   is done using single characters (i.e. "0"-"9")
*/
int decode_vectors(int spot_type, bool *do_vectors,
                   char *argv, size_t str_length)
{
  size_t pos, jj, val[2], imax;
  char str[2][STRSZ];
  char ichar[2];
  
  pos = 0;
  val[0] = 0;
  val[1] = 0;
  ichar[1] = '\0';
  
  if (spot_type == SPOT_GS) imax = MAX_INPUT_FILES;
  if (spot_type == SPOT_SS) imax = STRSZ;
	
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
			  do_vectors[val[0]] = true;
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
		  val[1] = abs(atoi(str[1]));
		  jj=0;
		  if ((val[0] < imax) && (val[1] < imax))
		    while (jj <= val[1])
		    {
			    do_vectors[jj] = true;
			    jj++;
		    }
		}
			
		if (argv[pos] == '*') 
		  for (jj=val[0]; jj<imax; jj++)
		  {
		    do_vectors[jj] = true;
		  }
			
		pos++;
	}
  
  return(PATT_SUCCESS);
}
