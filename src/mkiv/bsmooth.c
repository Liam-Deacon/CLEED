
/**************************************************************************
     
              File Name: bsmooth.c
     
 **************************************************************************/

/************************************************************************/

float b_smooth(float *int_norm, int numb, int nstart, int nstep, int width)

/*************************************************************************
GH/ 02.09.92
CS/21.9.93	allow nstep
  Calculate smoothed value for the "numb". element of int_norm with
  the foregoing "width" values
*************************************************************************/

{
     int i, n_ini;
     float int_avg = 0,
           fac = 1.;

/************************************************************************/

     if ( nstep > 0 )
        n_ini = (numb-nstep*width < nstart) ? nstart : (numb-nstep*width);
     else
        n_ini = (numb-nstep*width > nstart) ? nstart : (numb-nstep*width); 

/* linearly weighted average over the last n_max numbers */

     for ( i=n_ini; (i<=numb&&nstep>0)||(i>=numb&&nstep<0); i+=nstep, fac+=1.)
        int_avg += int_norm[i] * fac; 
     fac = fac * (fac-1.) / 2.;
     int_avg /= fac;

     return(int_avg);
}
/************************************************************************/
