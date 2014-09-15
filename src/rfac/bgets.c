/********************************************************************
  GH/25.09.92
********************************************************************/

int bgets(char *buffer, long offset, long max, char *string)

/*
 copy a single line (terminated by '\n' or '\r') from a
 nullterminated buffer into string.

 input: buffer, string
	offset starting point in buffer.
	max    max. length of string.
 return value: length of string (including '\0')
*/

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
