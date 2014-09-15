#include "shapes.hpp"
#include <QDebug>

//Shape::Shape(SHAPE shapeID, qreal size)
//{
//    switch (shapeID) {
//    case SHAPE_CIRCLE:
//        shapeType = new Circle();
//        break;
//    default:
//        break;
//    }
//}


//Shape::~Shape()
//{

//}

//Shape::Shape(qreal size)
//{
//    this->size = size;
//    shapeID = SHAPE_GENERIC;
//    updatePath();
//}

//void Shape::updatePath()
//{
//    QPainterPath p;
//    path = p;
//    qDebug() << this->getShapeName() << "path updated";
//}


//QRectF Shape::boundingRect() const
//{
//    return QRectF(-size, -size, size, size);
//}

//void Shape::setSize(qreal size)
//{
//    this->size = size;
//}

//void Shape::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
//{

//}

//QPainterPath Shape::draw(QPointF &center)
//{
//    QPainterPath p = path;
//    p.translate(center);
//    return p;
//}

//void Shape::draw(QPointF &center, QPainterPath &path)
//{
//    path = this->path;
//    path.translate(center);
//}

//QString Shape::getShapeName()
//{
//    switch (shapeID)
//    {
//    case SHAPE_CIRCLE:
//        return "Circle";
//    case SHAPE_CROSS:
//        return "Cross";
//    case SHAPE_DIAMOND:
//        return "Diamond";
//    case SHAPE_HEXAGON:
//        return "Hexagon";
//    case SHAPE_HEXSTAR:
//        return "6Star";
//    case SHAPE_PENTSTAR:
//        return "5Star";
//    case SHAPE_PLUS:
//        return "Plus";
//    case SHAPE_SQUARE:
//        return "Square";
//    case SHAPE_TRIANGLE_DOWN:
//        return "Down-Triangle";
//    case SHAPE_TRIANGLE_LEFT:
//        return "Left-Triangle";
//    case SHAPE_TRIANGLE_RIGHT:
//        return "Right-Triangle";
//    default:
//        return "Generic";
//    }
//}

//SHAPE Shape::getShapeID()
//{
//    return shapeID;
//}


//qreal Shape::getSize()
//{
//    return size;
//}


////Circle::Circle(qreal size)
////{
////    this->size = size;
////    shapeID = SHAPE_CIRCLE;
////}


//void Circle::updatePath()
//{
//    this->shapeID = SHAPE_CIRCLE;
//    path - QPainterPath();
//    path.arcTo(-size/2, -size/2, size, size, 0, 360*16);
//    path.closeSubpath();
//}


//void Square::updatePath()
//{
//    this->shapeID = SHAPE_SQUARE;
//    path = QPainterPath();
//    path.moveTo(-size, -size);
//    path.lineTo(-size, size);
//    path.lineTo(size, size);
//    path.lineTo(size, -size);
//    path.closeSubpath();
//}



////        p.lineTo(spot.x()-width, spot.y()-width);
////        p.lineTo(spot.x()-width, spot.y()+width);
////        p.lineTo(spot.x()+width, spot.y()+width);
////        p.lineTo(spot.x()+width, spot.y()-width);
////        QGraphicsPathItem *path = scene->addPath(p, pen, brush);

//width = spotSize/sqrt(2);
//p.moveTo(spot.x()-width, spot.y()-width);
//p.lineTo(spot.x()-width, spot.y()+width);
//p.lineTo(spot.x() + width, spot.y() + width);
//p.lineTo(spot.x() + width, spot.y() - width);
//p.closeSubpath(); //square

//QGraphicsPathItem *path = scene->addPath(p, pen, brush);
//spots->addToGroup(path);
//        p.moveTo(spot.x() - spotSize, spot.y());
//        p.lineTo(spot.x(), spot.y() + spotSize);
//        p.lineTo(spot.x() + spotSize, spot.y());
//        p.lineTo(spot.x(), spot.y()-spotSize);
//        p.closeSubpath(); //diamond

//        p.moveTo(spot.x(), spot.y() + width);
//        p.lineTo(spot.x() - width, spot.y() - (0.5 * spotSize));
//        p.lineTo(spot.x() + width, spot.y() - (0.5 * spotSize));
//        p.closeSubpath(); //down triangle

//        p.moveTo(spot.x(), spot.y() - (0.5 * spotSize));
//        p.lineTo(spot.x() - width, spot.y() + width);
//        p.lineTo(spot.x() + width, spot.y() + width);
//        p.closeSubpath(); //up triangle


//        p.moveTo(spot.x() - (0.5 * spotSize), spot.y());
//        p.lineTo(spot.x() + (0.5 * spotSize), spot.y() + width);
//        p.lineTo(spot.x() + (0.5 * spotSize), spot.y() - width);
//        p.closeSubpath(); //left triangle

//        p.moveTo(spot.x() - (0.5 * spotSize), spot.y() - width);
//        p.lineTo(spot.x() - (0.5 * spotSize), spot.y() + width);
//        p.lineTo(spot.x() + (0.5 * spotSize), spot.y());
//        p.closeSubpath(); //right triangle


//        qreal delta = spotSize * 0.866;
//        p.moveTo(spot.x(), spot.y() - spotSize);
//        p.lineTo(spot.x() - delta, spot.y() - (0.5*spotSize));
//        p.lineTo(spot.x() - delta, spot.y() + (0.5*spotSize));
//        p.lineTo(spot.x(), spot.y() + spotSize);
//        p.lineTo(spot.x() + delta, spot.y() + (0.5*spotSize));
//        p.lineTo(spot.x() + delta, spot.y() - (0.5*spotSize));
//        p.closeSubpath(); //hexagon

//        // pentagon
//        int numberOfSides = 5;
//        p.moveTo(spot.x() +  spotSize*cos(0), spot.y() +  spotSize*sin(0));

//        for (int i = 1; i <= numberOfSides;i ++)
//        {
//            p.lineTo(spot.x() + spotSize * cos(i * 2*M_PI / numberOfSides),
//                     spot.y() + spotSize * sin(i * 2*M_PI / numberOfSides));
//        }
//        p.closeSubpath();
