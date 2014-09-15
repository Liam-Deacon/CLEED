/********************************************************************
  GH/12.10.00
   
     char *file2buffer (char *filename)

Changes:

  GH/25.09.92 - Creation
  GH/08.09.93 - include uncompress
  GH/12.10.00 - replace open, lseek, read by fopen, fseek, fread
********************************************************************/

#define ERROR

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <strings.h>
#include <malloc.h>


char *file2buffer (char *filename)

/********************************************************************
 open file specified by filename and copy the content of the file
 to a nullterminated buffer.

 input: file name.
 return value: pointer to the buffer.
********************************************************************/

{
 char *buffer;
 char line_buffer[256];
 char uncomp;                     /* flag, if file was uncompressed */

 int len;
 long int nbytes;
 size_t   ntest;

 FILE *in_stream;

 uncomp =0;
 len = strlen(filename);
/* 
 - open file;
 - try uncompressing, if not successful otherwise
*/
 if( strcmp((filename+len-2),".Z") != 0 )
 {
  if ((in_stream = fopen(filename, "r") ) == NULL)
  {
#ifdef ERROR
   printf(" *** could not open \"%s\"\n", filename);
   fclose(in_stream);
/*   exit(1); */
#endif
/* try uncompressing */
   printf(" *** trying to uncompress \"%s.Z\"\n", filename);
   sprintf(line_buffer,"uncompress \"%s.Z\"",filename);
   if (system(line_buffer))
   {
#ifdef ERROR
    printf(" *** error: uncompressing was also unsuccessful!\n");
    fclose(in_stream);
    exit(1);
#endif
   } /* if system.. */
   else
   {
    if ((in_stream = fopen (filename, "r") ) == NULL)
    {
#ifdef ERROR
     printf(" *** error: could not open \"%s\"\n", filename);
     fclose(in_stream);
     exit(1);
#endif
    }
   uncomp = 1;
/* uncompressed filename is stored in filename */
   } /* else */
  }  /* if first open failed */
 }   /* if != .Z */
 else
 {
/* uncompressed filename is stored in filename */
  strncpy(line_buffer,filename, len-2);
  line_buffer[len-2] = '\0';
  strcpy(filename,line_buffer);

/* uncompress */
  printf("\tuncompress \t\"%s.Z\" > \"%s\" \n",filename,filename);
  sprintf(line_buffer,"uncompress \"%s.Z\"",filename);
  if (system(line_buffer))
  {
#ifdef ERROR
   printf(" *** error: uncompressing was unsuccessful!\n");
   exit(1);
#endif
  } /* if system.. */
  else
  {
   if ((in_stream = fopen (filename, "r") ) == NULL)
   {
#ifdef ERROR
    printf(" *** error: could not open \"%s\"\n",line_buffer);
    fclose(in_stream);
    exit(1);
#endif
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

#ifdef CONTROL
 fprintf(stdout, "(file2buffer): \"%s\" has %ld bytes \n", filename, nbytes);
#endif

 buffer = (char *)malloc((size_t)nbytes*13 + 2);
 if (buffer == NULL)
 {
#ifdef ERROR
  printf(">file2buffer: *** error occurred allocating memory for \"%s\"\n",
          filename);
  printf("                  (filesize: %ld)\n", nbytes+1);
#endif
  fclose(in_stream);
  exit(1);
 }

/************************************************************ 
 Read file to buffer
************************************************************/

 if ( (signed int)(ntest = fread(buffer, 1, nbytes, in_stream) ) == nbytes) 
 {
  buffer[nbytes] = '\0';
  buffer[nbytes+1] = '\0';

#ifdef CONTROL  /* control output, if required */
  fprintf(stdout,"(file2buffer): %ld bytes read from \"%s\"\n",
	 nbytes, filename);
  /* fprintf(stdout,"(file2buffer): %s",buffer); */
#endif

  if (uncomp) 
  {
   printf("\tcompress \t\"%s\" > \"%s.Z\"\n",filename,filename);
   sprintf(line_buffer,"compress \"%s\"",filename);
   system(line_buffer);
  }
  fclose(in_stream);
  return(buffer);
 }
 else
 {
#ifdef ERROR
  printf(">file2buffer: %d of %ld bytes read from \"%s\"\n",
	 ntest, nbytes, filename);
#endif
  free(buffer);
  if (uncomp) 
  {
   printf("\tcompress \t\"%s\" > \"%s.Z\"\n",filename,filename);
   sprintf(line_buffer,"compress \"%s\"",filename);
   system(line_buffer);
  }
  fclose(in_stream);
  return(NULL);
 }
}
