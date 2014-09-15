#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <QVector>

class Matrix2x2
{
public:
    Matrix2x2();
    ~Matrix2x2();
    QVector<QVector <qreal> > m;
};

#endif // MATRIX_HPP
