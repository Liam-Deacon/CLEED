#include <math.h>
#include <float.h>

int nearlyEqualFloat(float a, float b, float epsilon)
{
  static float diff = 0;

  diff = (float)fabs(a - b);

  if (a == b) /* shortcut, handles infinities */
  {
    return (1);
  } else if (a == 0 || b == 0 || diff < FLT_MIN)
  {
    /* a or b is zero or both are extremely close to it
       relative error is less meaningful here */
    return (diff < (epsilon * FLT_MIN));
  }
  else
  {
    /* use relative error */
    return (diff / (fabs(a) + fabs(b)) < epsilon);
  }
}

int nearlyEqualDouble(double a, double b, double epsilon)
{
    static float diff = 0;

    diff = (float)fabs(a - b);

    if (a == b) /* shortcut, handles infinities */
    { 
        return (1);
    } else if (a == 0 || b == 0 || diff < DBL_MIN) 
    {
        /* a or b is zero or both are extremely close to it
           relative error is less meaningful here */
        return (diff < (epsilon * DBL_MIN));
    } else { /* use relative error */
        return (diff / (fabs(a) + fabs(b)) < epsilon);
    }
}
