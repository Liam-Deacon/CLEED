/**************************************************************************
    
             File Name: file_functs.h
    
**************************************************************************/

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

/************************************************************************ 
   LD/02.03.14

  Purpose:
   provide simple cross-platform file functions to replace system calls.

************************************************************************/

/* return timestamp in format YYYY-mm-dd_HHMM */
const char *timestamp();

/* test whether file exists */
int file_exists(const char *filename);

/* copy contents of src file into dst file */
int file_copy(const char *src_path, const char *dst_path);

/* convenience function for backing up files */
int file_backup(const char *filepath);

/* move contents of src file into dst file */
int file_move(const char *src_path, const char *dst_path);

/* cat replacement */
char* file_content(const char* filename);

/* remove extension */
char *remove_ext(char* str, char dot, char sep);