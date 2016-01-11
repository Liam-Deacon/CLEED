/*********************************************************************
 *                       PATT_FGET_NOCOMM.C
 *
 *  Copyright 1992-2014 Georg Held <g.held@reading.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *********************************************************************/

/*! \file
 *
 * Contains fget_nocomm() function.
 */

#include <stdio.h>
#include "patt.h"

/*!
 * Lines beginning with '#' are interpreted as comments.
 *
 * \param[out] buffer string buffer.
 * \param in_stream File pointer for input e.g. \c stdin or a file
 * \param out_stream File pointer for output e.g. \c stdout or a file
 * \return Pointer to \p buffer string.
 */
char *fget_nocomm(char *buffer, FILE *in_stream, FILE *out_stream)
{
	if (buffer == NULL) {
		ERROR_MSG("buffer is NULL\n");
		return NULL;
	}
  while( (fgets(buffer, STRSZ, in_stream) != NULL) && (*buffer == '#') )
  {
   printf ("%s", buffer);
   fprintf(out_stream, "%s %s","%", buffer);
  }
  if (buffer == NULL) ERROR_MSG("input error\n");
 return(buffer);
}
