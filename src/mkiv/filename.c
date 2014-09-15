#include <strings.h>
#include "mkiv_funcs.h"

char *filename(char *fname, int n)
{
     char *strrchr();

     ito3a( n,1+strrchr(fname,'.') );
     
     return (fname);
}

int ito3a(int n, char *s)
{
     unsigned int i;
     int sign;

     if ( (sign=n) < 0 )
        n = -n;
        
     i = 0;
     do
     {
	   s[i++] = n % 10 +'0';
     } while ((n /= 10) > 0);
     
     while (i<strlen(s))
	   s[i++] = '0';
     
     if (sign < 0)
	   s[i++] = '-';
     
     s[i] = '\0';
     reverse(s);
     
     return 0;
}

char *reverse(char *s)
{
     int c,i,j;

     for(i=0,j=strlen(s)-1;i<j;i++,j--)
     {
	   c = s[i];
	   s[i] = s[j];
	   s[j] = c;
     }
     
     return 0;
}
