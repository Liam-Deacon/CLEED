/*********************************************************************
 *                     PATT_DECODE_VECTORS.C
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 * \author Liam Deacon <liam.m.deacon@gmail.com>
 * Implements decode_vectors() function.
 */

#include "patt.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

/*!
 * Decodes input string to determine which vectors to draw
 *
 * \param spotype Either #SPOT_GS for substrate spots or #SPOT_SS for
 * superstructure spots.
 * \param do_vectors Pointer to an array of boolean values indicating whether or
 * not to draw a given vector.
 * \param argv The string to decode.
 * \param str_length Length of string.
 * \return
 *
 * \note more than 10 input files is not currently supported as the conversion
 * is done using single characters (i.e. "0"-"9")
 */
int decode_vectors(int spotype, bool *do_vectors,
                   char *argv, size_t str_length)
{
  size_t pos, jj, val[2], imax=0;
  char str[2][STRSZ];
  char ichar[2];
  
  pos = 0;
  val[0] = 0;
  val[1] = 0;
  ichar[1] = '\0';
  
  if (spotype == SPOT_GS) imax = MAX_INPUT_FILES;
  if (spotype == SPOT_SS) imax = STRSZ;
	
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
