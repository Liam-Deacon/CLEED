#include "paintrenderer.hpp"

#include <QPainter>
#include <QPainterPath>
#include "math.h"

PaintRenderer::PaintRenderer(QWidget *parent)
    : QWidget(parent)
{
    shape = Circle;
    antialiased = false;
    transformed = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize PaintRenderer::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize PaintRenderer::sizeHint() const
{
    return QSize(400, 200);
}

void PaintRenderer::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void PaintRenderer::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void PaintRenderer::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void PaintRenderer::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void PaintRenderer::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void PaintRenderer::paintEvent(QPaintEvent * /* event */)
{
    static const QPoint points[4] = {
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };

    QRect rect(10, 20, 80, 60);

    QPainterPath path = QPainterPath();


    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    for (int x = 0; x < width(); x += 100) {
        for (int y = 0; y < height(); y += 100) {
            painter.save();
            painter.translate(x, y);
            if (transformed) {
                painter.translate(50, 50);
                painter.rotate(60.0);
                painter.scale(0.6, 0.9);
                painter.translate(-50, -50);
            }

            qreal pSize = 30;
            qreal pWidth = pSize / sqrt(2);
            qreal delta = pSize * (0.5 * sqrt(3));
            qreal quad = pSize * 0.25;

            switch (shape) {
            case Circle:
                path.arcTo(0, 0, pSize, pSize, 0, 360*16);
                path.closeSubpath();
                painter.drawPath(path);
                break;
            case Square: case Cross:
                path.moveTo(-pSize, -pSize);
                path.lineTo(-pSize,  pSize);
                path.lineTo( pSize,  pSize);
                path.lineTo( pSize, -pSize);
                path.closeSubpath(); //square
                painter.drawPath(path);
                break;
            case UpTriangle:
                path.moveTo(0, -0.5 * pSize);
                path.lineTo(-delta, delta);
                path.lineTo( delta, delta);
                path.closeSubpath(); //up triangle
                painter.drawPath(path);
                break;
            case DownTriangle:
                path.moveTo(0, delta);
                path.lineTo(-delta, -0.5 * pSize);
                path.lineTo( delta, -0.5 * pSize);
                path.closeSubpath(); //down triangle
                painter.drawPath(path);
                break;
            case Diamond:
                path.moveTo(-pWidth, 0);
                path.lineTo(0, pWidth);
                path.lineTo(pWidth, 0);
                path.lineTo(0, -pWidth);
                path.closeSubpath(); //diamond
                painter.drawPath(path);
                break;
            case Plus:
                path.moveTo(-quad, pSize);
                path.lineTo(quad, pSize);
                path.lineTo(-quad, pSize);
                path.lineTo(pWidth, 0);
                path.lineTo(0, -pWidth);
                path.closeSubpath(); //plus
                painter.drawPath(path);
                break;
            case LeftTriangle:
                path.moveTo(-0.5 * pSize,  0);
                path.lineTo( 0.5 * pSize,  delta);
                path.lineTo( 0.5 * pSize, -delta);
                path.closeSubpath(); //left triangle
                painter.drawPath(path);
                break;
            case RightTriangle:
                path.moveTo(-0.5 * pSize, -delta);
                path.lineTo(-0.5 * pSize,  delta);
                path.lineTo( 0.5 * pSize, 0);
                path.closeSubpath(); //right triangle
                painter.drawPath(path);
                break;
            case Hexagon:
                path.moveTo(0, 0 - pSize);
                path.lineTo(-delta, -0.5 * pSize);
                path.lineTo(-delta,  0.5 * pSize);
                path.lineTo(0, pSize);
                path.lineTo(delta,  0.5 * pSize);
                path.lineTo(delta, -0.5 * pSize);
                path.closeSubpath(); //hexagon
                painter.drawPath(path);
                break;
            }
            painter.restore();
        }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
