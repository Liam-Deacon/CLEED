/*********************************************************************
 *                        FILE_FUNCTS.C
 *
 *  Copyright 2013-2014 Liam Deacon <liam.m.deacon@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 * Changes:
 *   LD/02.03.14 - creation
 ***************************************************************************/

/*! \file
 * \brief Provides simple cross-platform file functions to replace system()
 * calls.
 *
 * Contains timestamp() , file_exists() , file_copy() , file_move() ,
 * file_backup() , file_content() and remove_ext() helper functions for basic
 * manipulating of files.
 */

#include <time.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <sys/stat.h>

#include "mkiv.h"

/* platform specific implementations - handled with C preprocessor */
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_) || defined(WIN32)
#include <tchar.h>
typedef struct _stat plat_stat;   /*!< platform specific stat struct */
#define STAT_FUNCTION _tstat      /*!< platform specific stat function */
#else /* unix */
typedef struct stat plat_stat;    /*!< platform specific stat struct */
#define STAT_FUNCTION stat        /*!< platform specific stat function */
#endif

/*!
 * Returns a timestamp string in the format 'YYYY-mm-dd_HHMM'
 *
 *  * \return Pointer to timestamp string.
 */
const char *timestamp()
{
  time_t rawtime;
  struct tm *timeinfo;
  char *buffer = (char*)malloc(80*sizeof(char));

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime(buffer, 80, "%Y-%m-%d_%H%M", timeinfo);

  return(buffer);
}

/*!
 * Tests whether a file exists.
 *
 * \param filename Path to file to test.
 *  * \return Integer representing result of test.
 * \retval 0 \p filename does not exist.
 * \retval 1 \p filename exists.
 */
int file_exists(const char *filename)
{
  plat_stat buffer;
  if (filename == NULL) return (0);
  if (filename[0] == '\0') return (0);
  return (STAT_FUNCTION (filename, &buffer) == 0);
}

/*!
 * Copies the contents of \p src into \p dst
 *
 * \param src_path Path to source file.
 * \param dst_path Destination file path.
 *  * \return Integer representing function success.
 * \retval 0 if copy is successful.
 * \retval -1 if copying is unsuccessful.
 */
int file_copy(const char *src_path, const char *dst_path)
{
  FILE *p,*q;
  char ch;

  if( (p = fopen(src_path, "r")) == NULL)
  {
    ERROR_MSG("cannot open: %s\n", src_path);
    return(-1);
  }

  if( (q = fopen(dst_path, "w")) == NULL)
  {
    ERROR_MSG("cannot open: %s\n", dst_path);
    return(-1);
  }
  while( (ch = getc(p)) != EOF) putc(ch,q);

  /* tidy up */
  fclose(p);
  fclose(q);

  return(0);
} 

/*!
 * Convenience function for moving files (emulates unix \c mv command).
 *
 * \param[in] src_path Path string for source file.
 * \param[in] dst_path Destination path string.
 *  * \return Integer representing function success following normal C convention.
 */
int file_move(const char *src_path, const char *dst_path)
{
  int ret;
  ret = file_copy(src_path, dst_path);
  return(abs(ret) + abs(remove(src_path)));
}

/*!
 * Convenience function for backing up files.
 *
 * \param[in] filepath Path to source file to backup.
 *  * \return Integer representing function success following normal C convention.
 * \retval 0 indicates backup was successful.
 * \retval -1 if the backup cannot be copied.
 */
int file_backup(const char *filepath)
{
  if (file_exists(filepath))
  {
    const char *stamp = timestamp();
    char *backup = (char *)malloc(strlen(filepath) +
                strlen(stamp) + 5 * sizeof(char));
    strcpy(backup, filepath);
    strcat(backup, ".bak");
    strcat(backup, "_");
    strcat(backup, stamp);
    return(file_copy(filepath, backup));
  }

  return(0);
}

/*!
 * Gets the content of a file (emulates the unix 'cat' command)
 *
 * \param[in] filename Path to source file.
 *  * \return Pointer to file content string.
 */
char* file_content(const char* filename)
{
  FILE* file = fopen(filename,"r");
  if(file == NULL)
  {
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long int size = ftell(file);
  rewind(file);

  char* content = calloc(size + 1, 1);

  fread(content, 1, size, file);
  fclose(file);

  return(content);
}

/*!
 * Removes the "extension" from a file specified
 *
 * \param str The string to process.
 * \param dot Extension separator character.
 * \param sep Path separator character (0 means to ignore).
 *
 * \return Allocated string identical to the original \p str but
 * with the extension removed. It must be freed when you're finished with it.
 * \retval \c NULL if \p str is \c NULL or the new string can't be allocated
 */
const char *remove_ext(const char* str, char dot, char sep) {
  char *retstr = (char *)calloc(FILENAME_MAX, sizeof(char));
  char *lastdot, *lastsep;

  /* Error checks and allocate string */
  if (str == NULL || retstr) return NULL;

  /* Make a copy and find the relevant characters */
  strncpy(retstr, str, FILENAME_MAX);
  lastdot = strrchr (retstr, dot);
  lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

  /* If it has an extension separator */
  if (lastdot != NULL)
  {
    /* and it's before the extension separator */
    if (lastsep != NULL)
    {
      if (lastsep < lastdot)
      {
        /* then remove it */
        *lastdot = '\0';
      }
    }
    else
    {
      /* It has extension separator with no path separator */
      *lastdot = '\0';
    }
  }

  /* Return the modified string */
  return(retstr);
}
