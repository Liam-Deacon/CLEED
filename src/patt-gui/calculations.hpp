#ifndef CALCULATIONS_HPP
#define CALCULATIONS_HPP

#include <QVector>
#include <QPointF>

class Calculations
{
public:
    Calculations();
    int isFraction(int *zahler, int *nenner);
    void reduce2x2Matrix(QVector<QVector<qreal> > m);
    qreal magnitude(qreal x, qreal y);
};

#endif // CALCULATIONS_HPP
