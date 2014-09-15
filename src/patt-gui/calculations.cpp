#include "calculations.hpp"
#include <math.h>

Calculations::Calculations()
{
}


int Calculations::isFraction(int *zahler, int *nenner)
    /*
     make nicer fractions
     return values:
    -1: zero
     0: integer number;
     1: fraction;
    */
{
    int i, ggt, number;

    if (*zahler == 0)
        return (-1);

    if (*nenner < 0)
        number = - *nenner/2;
    else
        number =   *nenner/2;

    for (ggt = 1, i = 2; i <= number; i++)
        if( (*zahler % i == 0 ) && (*nenner % i == 0) ) ggt = i;

    *zahler /= ggt;
    *nenner /= ggt;

    if (*zahler % *nenner == 0)
    {
        *zahler /= *nenner;
        return(0);
    }
    else
        return(1);
}


void Calculations::reduce2x2Matrix(QVector<QVector<qreal> > m)
{
    QVector<QVector<qreal> > K;
    K.resize(2);
    K[0].resize(2);
    K[1].resize(2);

    K[0][0] = m[0][0] - (m[1][0]*m[0][1]/m[1][1]);
    K[0][1] = m[1][1] - (m[0][1]*m[1][0]/m[0][0]);
    K[1][0] = m[0][1] - (m[1][1]*m[0][0]/m[1][0]);
    K[1][1] = m[1][0] - (m[0][0]*m[1][1]/m[0][1]);

    m[0][0] = K[0][0];
    m[0][1] = K[0][1];
    m[1][0] = K[1][0];
    m[1][1] = K[1][1];
}


qreal Calculations::magnitude(qreal x, qreal y)
{
    return sqrt(x*x + y*y);
}
