#ifndef PATTERN_HPP
#define PATTERN_HPP

#include "config.hpp"

#include <QtGlobal>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QVector>
#include <QPointF>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include "shapes.hpp"

typedef QVector < QVector < qreal > > BASIS;
typedef QVector < QVector < QPointF > > SPOTS; /*2d QPointF array - 1: layer, 2: lattice */

#define LATTICE_GS 0
#define LATTICE_SS 1
typedef quint8 LATTICE;
typedef quint8 DOMAIN;

class Pattern : public QGraphicsItemGroup
{
   Q_OBJECT
public:
    explicit Pattern();
    explicit Pattern(BASIS &basis, QVector<BASIS> &matrix);
    explicit Pattern(BASIS &basis, QVector<BASIS> &matrix,
                     Shape &shapeGS, Shape &shapeSS);
    ~Pattern();


public slots:
    /* getters */
    quint8 getDomainSize();
    bool vectorsVisible(LATTICE lattice=LATTICE_GS);

    QVector<QPointF> getPositions(LATTICE lattice=LATTICE_GS,
                                  DOMAIN domain=0);


    void recalculateSpots(LATTICE lattice=LATTICE_GS,
                          DOMAIN domain=0);


private:

    BASIS gsBasis;
    QVector<BASIS> ssMatrix;

    bool showGSVectors;
    QVector<bool> showSSVectors;
    bool showGSIndices;
    QVector<bool> showSSIndices;

    Shape gsShape;
    QVector<Shape> ssShape;

    QVector<QPointF> gs;
    SPOTS ss;

private slots:
    void calculateGS();
    void calculateSS();
};
#endif // PATTERN_HPP
