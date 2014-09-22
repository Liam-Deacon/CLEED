/*========================================================================*/

int nice_frac(int *numerator, int *denominator)

/*
 make nicer fractions
 return values:
 0: zero
 1: integer number;
 2: fraction;
*/

{
int i, ggt, number;

 if (*numerator == 0)
 return (0);

 if (*denominator < 0) number = - *denominator/2;
 else             number =   *denominator/2;
 
 for (ggt = 1, i = 2; i <= number; i++)
 {
  if( (*numerator % i == 0 ) && (*denominator % i == 0) ) ggt = i;
 }
 *numerator /= ggt;
 *denominator /= ggt;

 if (*numerator % *denominator == 0)
 {
  *numerator /= *denominator;
  return(1);
 }
 else
 return(2);
}

/*========================================================================*/
