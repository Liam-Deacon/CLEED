/*********************************************************************
 *                       BGETS.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Description:  file contains functions
 *  bgets()
 *    copies a line of contents into a string buffer.
 *
 * Changes:
 *   GH/1992.09.25 - creation
 *********************************************************************/

/*! \file bgets.c
 *  \brief Implementation file for C function bgets() .
 */

/*!
 * \fn bgets
 *
 * Copy a single line (terminated by '\n' or '\r') from a
 * null terminated buffer into string.
 *
 * \param buffer
 * \param string
 * \param offset starting point in buffer.
 * \param max maximum length of string.
 * \return length of string (including '\0')
 * \retval -1 invalid string
*/
int bgets(const char *buffer, long offset, long max, char *string)
{
 int j;
 long i;
 
 if (buffer[offset] == '\0')
 {
  string[0] = '\0';
  return(-1);
 }

 else
 {
  max += offset;
  for (j = 0, i=offset; 
       (buffer[i] != '\n') && (buffer[i] != '\r') && 
       (buffer[i] != '\0') && (i < max);
       i ++ , j ++)
   string[j] = buffer[i];    
  string[j] = '\0';
  return(j+1);
 }
}
