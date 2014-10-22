
/*! \file
 *
 * Implements nice_frac() to make fractions nicer.
 */

/*!
 * Makes fractions nicer
 *
 * \param numerator The top part of the fraction.
 * \param denominator The bottom part of the fraction.
 * \return Integer representing type of fraction.
 *
 * \retval 0 zero
 * \retval 1 integer number
 * \retval 2 fraction (e.g. \f$ \frac{1}{3} \f$ )
 */
int nice_frac(int *numerator, int *denominator)
{
  int i, ggt, number;

  if (*numerator == 0)
  return (0);

  if (*denominator < 0) number = - *denominator/2;
  else number = *denominator/2;
 
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
  else return(2);
}
