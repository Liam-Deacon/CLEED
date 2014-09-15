#ifndef MODEL_HPP
#define MODEL_HPP

#include <QDialog>
#include <QStringList>
#include <QVector>

typedef QVector <QVector <qreal> > BASIS;

namespace Ui {
class Model;
}

class Model : public QDialog
{
    Q_OBJECT
    
public:
    explicit Model(QWidget *parent = 0);
    ~Model();
    
public slots:
    void addMatrix(qreal m11, qreal m12, qreal m21, qreal m22,
                   QStringList &domains);
    void addDomains(QStringList &domains);

private:
    Ui::Model *ui;

    BASIS matrixGS;
    BASIS matrixSS;

    QStringList domainsSS;
    qreal basis[2];
    bool xySwap[2];

private slots:
    void actionAddSuperstructure();
    void actionRemoveSuperstructure();
    void actionRotate();
    void actionUpdateBasis();
    void actionUpdateScale();
    void actionSwapBasis();
    void actionSwapXY1();
    void actionSwapXY2();
    void actionScaleBasis();
    void actionTwinBasis();
    void actionUpdateVectors();
    void showTreeContextMenu(const QPoint &pos);



};

#endif // MODEL_HPP
