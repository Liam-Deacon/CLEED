/**************************************************************************
   
             File Name: file_functs.c
    
**************************************************************************/

#include <time.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#include "cleed_string.h"
#include "cleed_cstring.h"
#include <ctype.h>
#include <sys/stat.h>

/************************************************************************ 
   LD/02.03.14

  Purpose:
   provide simple cross-platform file functions to replace system calls.

************************************************************************/

/* return timestamp in format YYYY-mm-dd_HHMM */
const char *timestamp()
{
    time_t rawtime;
    struct tm *timeinfo;
    char* buffer = malloc(80);
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime(buffer,80,"%Y-%m-%d_%H%M",timeinfo);
    
    return buffer;
}

/* test whether file exists */
int file_exists(const char *filename)
{
    struct stat buffer;   
    return (stat (filename, &buffer) == 0);
}

/* copy contents of src file into dst file */
int file_copy(const char *src_path, const char *dst_path)
{
    FILE *p,*q;
    /* char src_path, dst_path; */
    char ch;

    p=fopen(src_path,"r");
    if(p==NULL){
      fprintf(stderr, "cannot open: %s\n", src_path);
      return (-1);
    }
    
    q=fopen(dst_path,"w");
    if(q==NULL){
      fprintf(stderr, "cannot open: %s\n", dst_path);
      return (-1);
    }
    while((ch=getc(p))!=EOF)
        putc(ch,q);
    
    /* tidy up */
    fclose(p);
    fclose(q);
    
    return 0;
} 

/* convenience function for moving files */
int file_move(const char *src_path, const char *dst_path)
{
    int ret;
    ret = file_copy(src_path, dst_path);
    return abs(ret) + abs(remove(src_path));
}

/* convenience function for backing up files */
int file_backup(const char *filepath)
{
    if (file_exists(filepath)) {
        const char *stamp = timestamp();
        char *backup = (char *)malloc(cleed_strnlen(filepath, 4096) +
                    cleed_strnlen(stamp, 80) + 5 * sizeof(char)); 
        strcpy(backup, filepath);
        strcat(backup, ".bak");
        strcat(backup, "_");
        strcat(backup, stamp);
        return file_copy(filepath, backup);
    }
    
    return 0;
}

/* get the content of the file (like 'cat' command) */
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

    fread(content,1,size,file);
    fclose(file);

    return content;
}

/***************************************************************************** 
remove_ext: 
   
   removes the "extension" from a file specified
   str is the string to process.
   dot is the extension separator.
   sep is the path separator (0 means to ignore).
   
   Returns an allocated string identical to the original but
   with the extension removed. It must be freed when you're finished with it.
   If you pass in NULL or the new string can't be allocated,
   it returns NULL 
******************************************************************************/
char *remove_ext(char* str, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    /* Error checks and allocate string */
    if (str == NULL)
        return NULL;
    if ((retstr = (char*)malloc(cleed_strnlen(str, 4096)+1*sizeof(char))) == NULL)
        return NULL;

    /* Make a copy and find the relevant characters */
    strcpy (retstr, str);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    /* If it has an extension separator */
    if (lastdot != NULL) {
        /* and it's before the extension separator */
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                /* then remove it */
                *lastdot = '\0';
            }
        } else {
            /* It has extension separator with no path separator */
            *lastdot = '\0';
        }
    }

    /* Return the modified string */
    return retstr;
}
