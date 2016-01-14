/*********************************************************************
 *                       FILE2BUFFER.C
 *
 *  Copyright 1992-2014 Georg Held
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   GH/1992.09.25 - Creation
 *   GH/1993.09.08 - include uncompress (uses system call)
 *   GH/2000.10.12 - replace open, lseek, read by fopen, fseek, fread
 ********************************************************************/

/*! \file file2buffer.c
 *  \brief Implementation file for file2buffer() function.
 */

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <string.h>
#include <malloc.h>

#include "gh_stddef.h"

/*!
 * \fn file2buffer
 *
 * Open file specified by filename and copy the content of the file
 * to a null terminated buffer.
 *
 * \param filename string containing name of file to copy into memory
 * buffer.
 *
 * \return pointer to the buffer.
********************************************************************/
char *file2buffer(const char * const filename)
{
  char *buffer = NULL;
  char uncomp_filename[FILENAME_MAX] = "";
  char line_buffer[256] = "";
  char uncomp = 0;                     /* flag, if file was uncompressed */

  size_t len = strlen(filename);
  long int nbytes = 0L, ntest;

  FILE *in_stream = NULL;

  /*
   * - open file;
   * - try uncompressing, if not successful otherwise
   */

  if( strcmp((filename+len-2), ".Z") != 0 )
  {
    if ((in_stream = fopen(filename, "r") ) == NULL)
    {
      ERROR_MSG("could not open \"%s\" (%s)\n", filename, strerror(errno));
      fclose(in_stream);

      /* try uncompressing */
      printf(" *** trying to uncompress \"%s.Z\"\n", filename);
      sprintf(line_buffer, "uncompress \"%s.Z\"", filename);
      if (system(line_buffer))
      {
        ERROR_MSG("uncompression was also unsuccessful!\n");
        fclose(in_stream);
        ERROR_RETURN(NULL);
      } /* if system.. */
      else
      {
        if ((in_stream = fopen (filename, "r") ) == NULL)
        {
          ERROR_MSG("could not open \"%s\" (%s)\n", 
                    filename, strerror(errno));
          fclose(in_stream);
          ERROR_RETURN(NULL);
        }

        uncomp = 1;
        /* uncompressed filename is stored in filename */

      } /* else */

    }  /* if first open failed */

  }   /* if != .Z */
  else
  {
    /* uncompressed filename is stored in filename */
    strncpy(line_buffer, filename, len-2);
    line_buffer[len-2] = '\0';
    strcpy(uncomp_filename, line_buffer);

    /* uncompress */
    printf("\tuncompress \t\"%s.Z\" > \"%s\" \n", filename, filename);
    sprintf(line_buffer, "uncompress \"%s.Z\"", filename);
    if (system(line_buffer))
    {
      ERROR_MSG("uncompressing was unsuccessful!\n");
      ERROR_RETURN(NULL);
    } /* if system.. */
    else
    {
      if ((in_stream = fopen(filename, "r") ) == NULL)
      {
        ERROR_MSG("could not open \"%s\" (%s)\n", 
                  filename, strerror(errno));
        fclose(in_stream);
        ERROR_RETURN(NULL);
      }

      uncomp = 1;
      /* uncompressed filename is stored in filename */

    } /* else */

  }  /* else (.Z) */

/************************************************************ 
 - set file pointer to end of file and find size of file.
 - reset file pointer to beginning of file.
 - allocate memory for buffer.
************************************************************/

  fseek (in_stream, 0L, SEEK_END);
  nbytes = ftell(in_stream);
  fseek (in_stream, 0L, SEEK_SET);

  CONTROL_MSG(CONTROL, "\"%s\" has %ld bytes \n", filename, nbytes);

  CLEED_ALLOC_CHECK(buffer = (char *)
    calloc((size_t)nbytes*13 + 2, sizeof(char)));

/************************************************************ 
 * Read file to buffer
 ************************************************************/

  if ( (ntest = (long int)fread(buffer, 1,
                                (size_t)nbytes, in_stream) ) == nbytes)
  {
    buffer[nbytes] = '\0';
    buffer[nbytes+1] = '\0';

    CONTROL_MSG(CONTROL, "%ld bytes read from \"%s\"\n", nbytes, filename);

    if (uncomp)
    {
      printf("\tcompress \t\"%s\" > \"%s.Z\"\n", filename, filename);
      sprintf(line_buffer,"compress \"%s\"", filename);
      system(line_buffer);
    }
    fclose(in_stream);

    return(buffer);
  }
  else
  {

    ERROR_MSG("%ld of %ld bytes read from \"%s\"\n", ntest, nbytes, filename);

    free(buffer);
    if (uncomp)
    {
      printf("\tcompress \t\"%s\" > \"%s.Z\"\n", filename, filename);
      sprintf(line_buffer, "compress \"%s\"", filename);
      system(line_buffer);
    }

    fclose(in_stream);
    return(NULL);
  }
}
