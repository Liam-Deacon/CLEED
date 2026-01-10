#include <stddef.h>

int rf_lines( char *buffer)
/*
 count '\r' and '\n' in buffer.
 
 return value: number of lines.
*/
{
 int lines;
 long i;

 lines = 0;
 for(i = 0L; buffer[i] != '\0'; i++)
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   lines ++;
  }
 return(lines);
}
/**********************************************************************/
int rf_nclines( char *buffer)
/*
 count '\r' and '\n' in buffer. (count lines without comment.)

 return value: number of lines.
*/
{
 int lines, comm;
 long i;

 lines = 0;
 if (buffer[0] == '#') comm = 1;
 else                  comm = 0;

 for(i = 0L; buffer[i] != '\0'; i++)
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   lines ++;
   if ( buffer[i+1] == '#' ) comm ++;
  }
 return(lines-comm);
}
/**********************************************************************/
int rf_clines( char *buffer)
/*
 count '\r' and '\n' in buffer.
	       if 1 count comment lines,
 return value: number of lines.
*/
{
 int comm;
 long i;

 if (buffer[0] == '#') comm = 1;
 else                  comm = 0;

 for(i = 0L; buffer[i] != '\0'; i++)
 {
  if ( (buffer[i] == '\n') || (buffer[i] == '\r') )
  {
   if ( buffer[i+1] == '#' ) comm ++;
  }
 }
 return(comm);
}
/**********************************************************************/

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
