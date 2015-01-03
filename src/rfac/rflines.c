/*********************************************************************
 *                           RFLINES.C
 *
 *  Copyright 1994-2014 Georg Held <g.held@reading.ac.uk>
 *  Copyright 2014 Liam Deacon <liam.deacon@diamond.ac.uk>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1995.08.11 - creation
 *   LD/2014.10.09 - added some Doxygen markup
 *                 - changed functions to return size_t
 *                 - now using size_t for counting rather than long to
 *                   help portability.
 *********************************************************************/

/*! \file
 *
 * Implementation file for rfac_lines(), rfac_nclines() and rfac_clines() functions.
 */

#include <stdio.h>

/*!
 * \fn rfac_lines
 *
 * Count '\r' and '\n' in \p buffer.
 *
 * \param *buffer pointer to string constant containing lines to count.
 *
 * \return number of lines.
 */
size_t rfac_lines(const char *buffer)
{
 size_t lines, i;

 lines = 0;
 for(i = 0; buffer[i] != '\0'; i++)
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   lines ++;
  }
 return(lines);
}
/**********************************************************************/

/*!
 * \fn rfac_nclines
 *
 * Count '\r' and '\n' in \p buffer, but exclude comment lines.
 *
 * \param *buffer pointer to string constant containing lines to count.
 *
 * \return number of lines without comments.
 */
size_t rfac_nclines(const char *buffer)
{
 size_t lines, comm, i;

 lines = 0;
 if (buffer[0] == '#') comm = 1;
 else                  comm = 0;

 for(i = 0; buffer[i] != '\0'; i++)
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   lines ++;
   if ( buffer[i+1] == '#' ) comm ++;
  }
 return(lines-comm);
}
/**********************************************************************/

/*!
 * \fn rfac_clines
 *
 * Count '\r' and '\n' in \p buffer.
 * If 1st character in line is '#' then add to count.
 *
 * \param *buffer pointer to string constant containing lines to count.
 *
 * \return number of comment lines.
 */
size_t rfac_clines(const char *buffer)
{
 size_t comm, i;

 if (buffer[0] == '#') comm = 1;
 else                  comm = 0;

 for(i = 0; buffer[i] != '\0'; i++)
 {
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   if ( buffer[i+1] == '#' ) comm ++;
  }
 }
 return(comm);
}
/**********************************************************************/
