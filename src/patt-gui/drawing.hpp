#ifndef DRAWING_HPP
#define DRAWING_HPP

#include "config.hpp"
#include "shapes.hpp"
#include "pattern.hpp"

#include <QString>
#include <QStringList>
#include <QGraphicsScene>
#include <QColor>
#include <QVector>
#include <QPointF>

typedef QVector < QVector < qreal > > BASIS;
typedef QVector < QVector < BASIS > > SUPERMATRIX;
typedef QVector < QVector < QPointF > > SPOTS; /*2d QPointF array - 1: layer, 2: lattice */
typedef QVector < QVector < SPOTS > > SUPERSPOTS; /*4d QPointF array - 1: layer, 2: lattice, 3. superlattice, 4. domains*/
typedef QVector < QVector < QColor > > COLOR;
typedef QVector < QVector < COLOR > > SUPERCOLOR;
typedef QVector < QVector < SHAPE > > SHAPES;
typedef QVector < QVector < SHAPES > > SUPERSHAPES;
typedef QVector < QVector < bool > > BOOLS;
typedef QVector < QVector < BOOLS > > SUPERBOOLS;
typedef QVector < QVector < quint8 > > QUINTS;
typedef QVector < QVector < QUINTS > > SUPERQUINTS;
typedef QVector < Pattern > PATTERNS;

class Drawing : public QGraphicsScene
{
   Q_OBJECT
public:
    Drawing();
    ~Drawing();

public slots:
    /* getters */
    void getVectors(int type);
    void getDomains();

    QColor getScreenEdgeColor();
    qreal getScreenEdgeWidth();
    QColor getScreenFillColor();
    quint8 getScreenAlpha();
    qreal getScreenTransparency();

    qreal getGunAngle();
    qreal getGunWidth();
    qreal getGunRadius();

    QString getFooter();
    QColor getFooterColor();
    quint8 getFooterSize();
    QString getTitle();
    QColor getTitleColor();
    quint8 getTitleSize();

    qreal getRotation();

    bool getPattern(int lattice);

    /* setters */
    void setOffset(QPointF offset = QPointF(0,0));
    //void setOffset(QString offset = QString::null);
    void setRotation(qreal theta = 0.);
    void setAzimuthal(qreal phi = 0.);

    void setClipToScreen(bool clip = true);

    void setScreenFillColor(QColor color = QColor("yellowgreen"));
    void setScreenEdgeColor(QColor color = QColor("green"));
    void setScreenEdgeWidth(qreal width = 2.);
    void setScreenAlpha(quint8 alpha = 128);
    void setScreenTransparency(qreal percent = 50.);

    void setGunRadius(qreal radius = 2.);
    void setGunWidth(qreal width = 2.);
    void setGunAngle(qreal angle = 35.);
    void setGunColor(QColor color = QColor("black"));

    void setFooter(QString string);
    void setFooterColor(QColor color = QColor("black"));
    void setFooterPosition(QPointF pos);
    void setFooterPosition(QString pos);
    void setFooterSize(quint8 size);
    void setTitle(QString string);
    void setTitleColor(QColor color = QColor("black"));
    void setTitlePosition(QPointF pos);
    void setTitlePosition(QString pos);
    void setTitleSize(quint8 size);
    void setLegendSize(quint8 size);
    void setLegendColor(QColor color = QColor("black"));
    void setLegendPosition(QPointF pos);
    void setLegendPosition(QString pos);

    void addLabel(QString label);
    void addDomains(int lattice,
                    int superlattice, QStringList &domains);
    void addSuperlattice(int lattice, SUPERMATRIX &m);
    void addBasis(BASIS basis);

    void recalculateGS(int lattice);
    void recalculateSS(int lattice, int superlattice);
    void recalculateSSDomain(int lattice, int superlattice, int domain);

    void removeLayer(int layer);
    void removeLattice(int layer, int lattice);
    void removeSuperlattice(int layer, int lattice, int superlattice);


private:

    /* variables */
    QStringList labels;
    QString title;
    QString footer;

    QColor labelColor;
    quint8 labelSize;

    QColor footerColor;
    quint8 footerSize;

    QColor titleColor;
    quint8 titleSize;

    bool colorLegendText;
    quint8 legendSize;

    QPointF titlePosition;
    QPointF footerPosition;
    QPointF legendPosition;

    QList<quint8> nDomains; //number of domains for each GS pattern

    BASIS a; //list of substrate GS 'vectors' (as QPointF list)
    SUPERMATRIX b; //list of superstructure SS 'vectors' (as QPointF list)

    QPointF center;
    qreal rotation;

    BASIS theta; //rotation
    BASIS phi; //azimuthal

    QColor screenEdgeColor;
    qreal screenEdgeWidth;
    QColor screenFillColor;
    quint8 screenTransparency;

    qreal gunRadius;
    qreal gunWidth;
    qreal gunAngle;

    bool showFooter;
    bool showGun;
    bool showScreen;
    bool showTitle;

    PATTERNS patterns;

};

#endif // DRAWING_HPP
