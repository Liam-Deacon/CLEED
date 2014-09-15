#ifndef PAINTRENDERER_HPP
#define PAINTRENDERER_HPP

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>

#include "shapes.hpp"

class PaintRenderer : public QWidget
{
    Q_OBJECT

public:
    enum Shape {Circle, Square, UpTriangle, DownTriangle, Diamond, Hexagon, Cross, Plus, LeftTriangle, RightTriangle};

    PaintRenderer(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;
};
#endif // PAINTRENDERER_HPP
