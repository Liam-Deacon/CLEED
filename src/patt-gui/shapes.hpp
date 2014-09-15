#ifndef SHAPES_H
#define SHAPES_H

#define SHAPE_GENERIC -1
#define SHAPE_CIRCLE 0
#define SHAPE_SQUARE 1
#define SHAPE_TRIANGLE_UP 2
#define SHAPE_TRIANGLE_DOWN 3
#define SHAPE_DIAMOND 4
#define SHAPE_HEXAGON 5
#define SHAPE_PENTSTAR 6
#define SHAPE_TRIANGLE_RIGHT 7
#define SHAPE_TRIANGLE_LEFT 8
#define SHAPE_HEXSTAR 9
#define SHAPE_PLUS 10
#define SHAPE_CROSS 11

#include <QString>
#include <QColor>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QGraphicsItem>

typedef quint8 SHAPE;

class Shape : public QGraphicsItem
{
   Q_OBJECT
public:
    Shape(qreal size);
    ~Shape();


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    SHAPE shapeID;
    qreal size;
    QPainterPath path;

    void updatePath();

public slots:
    SHAPE getShapeID();
    QString getShapeName();
    qreal getSize();

    void setSize(qreal size);

    /* draw differently depending on shape */
    virtual void draw(QPointF &center, QPainterPath &path);
    virtual QPainterPath draw(QPointF &center);

};


class Circle : public Shape
{
   Q_OBJECT
public:

    void updatePath();

public slots:
    SHAPE getShapeId();


private:
    qreal radius;
};


class Square : public Shape
{
   Q_OBJECT
public:
    Square(qreal size);
    ~Square();
    void updatePath();

public slots:
    SHAPE getShapeId();
};


class Triangle : public Shape
{
   Q_OBJECT
public:
    Triangle(qreal size);
    ~Triangle();

public slots:
    virtual SHAPE getShapeId();

private:
    qreal delta;
};

class UpTriangle : public Triangle
{
   Q_OBJECT
public:
    UpTriangle(qreal size);
    ~UpTriangle();

public slots:
    SHAPE getShapeId();

    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class DownTriangle : public Triangle
{
   Q_OBJECT
public:
    explicit DownTriangle(qreal size);
    ~DownTriangle();

public slots:
    QString getShapeName();
    SHAPE getShapeId();
    void draw(QPointF &center, QPainterPath &path);
    virtual QPainterPath draw(QPointF &center);
};

class LeftTriangle : public Triangle
{
   Q_OBJECT
public:
    explicit LeftTriangle(qreal size);
    ~LeftTriangle();

public slots:
    SHAPE getShapeId();
    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class RightTriangle : public Triangle
{
   Q_OBJECT
public:
    explicit RightTriangle(qreal size);
    ~RightTriangle();

public slots:
    SHAPE getShapeId();
    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class Diamond : public Shape
{
   Q_OBJECT
public:
    explicit Diamond();
    ~Diamond();

public slots:
    QString getShapeName(qreal size);
    SHAPE getShapeId();
    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class Hexagon : public Shape
{
   Q_OBJECT
public:
    explicit Hexagon();
    ~Hexagon();

public slots:
    QString getShapeName(qreal size);
    SHAPE getShapeId();
    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);

private:
    qreal delta;
};

class Star : public Shape
{
   Q_OBJECT
public:
    explicit Star(qreal size);
    ~Star();

public slots:
    virtual SHAPE getShapeId();

    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class HexStar : public Star
{
   Q_OBJECT
public:
    explicit HexStar(qreal size);
    ~HexStar();

public slots:
    SHAPE getShapeId();

    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

class Plus : public Shape
{
   Q_OBJECT
public:
    explicit Plus(qreal size);
    ~Plus();

public slots:
    virtual SHAPE getShapeId();

    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};


class Cross : public Plus
{
   Q_OBJECT
public:
    explicit Cross(qreal size);
    ~Cross();

public slots:
    SHAPE getShapeId();

    void draw(QPointF &center, QPainterPath &path);
    QPainterPath draw(QPointF &center);
};

#endif // SHAPES_H
