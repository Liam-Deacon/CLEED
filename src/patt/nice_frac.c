/*========================================================================*/

int nice_frac(int *zahler, int *nenner)

/*
 make nicer fractions
 return values:
 0: zero
 1: interger number;
 2: fraction;
*/

{
int i, ggt, number;

 if (*zahler == 0)
 return (0);

 if (*nenner < 0) number = - *nenner/2;
 else             number =   *nenner/2;
 
 for (ggt = 1, i = 2; i <= number; i++)
 {
  if( (*zahler % i == 0 ) && (*nenner % i == 0) ) ggt = i;
 }
 *zahler /= ggt;
 *nenner /= ggt;

 if (*zahler % *nenner == 0)
 {
  *zahler /= *nenner;
  return(1);
 }
 else
 return(2);
}

/*========================================================================*/