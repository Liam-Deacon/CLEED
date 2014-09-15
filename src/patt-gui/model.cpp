#include "model.hpp"
#include "ui_model.h"
#include "superstructuredialog.hpp"
#include "calculations.hpp"

#include <QMenu>
#include <QDebug>
#include <math.h>
#include <QMessageBox>
#include <QDialog>

Model::Model(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Model)
{
    ui->setupUi(this);

    ui->domainTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->domainTree, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showTreeContextMenu(const QPoint&)));
    connect(ui->actionAddStructure, SIGNAL(triggered()), this, SLOT(actionAddSuperstructure()));
    connect(ui->actionRemoveStructure, SIGNAL(triggered()), this, SLOT(actionRemoveSuperstructure()));
    connect(ui->toolScaleToOne, SIGNAL(clicked()), this, SLOT(actionScaleBasis()));
    connect(ui->toolSwap, SIGNAL(clicked()), this, SLOT(actionSwapBasis()));
    connect(ui->actionUpdateBasis, SIGNAL(triggered()), this, SLOT(actionUpdateBasis()));
    connect(ui->toolRemove, SIGNAL(clicked()), this, SLOT(actionRemoveSuperstructure()));
    connect(ui->toolAdd, SIGNAL(clicked()), this, SLOT(actionAddSuperstructure()));
    connect(ui->dblMultiply, SIGNAL(valueChanged(double)), this, SLOT(actionUpdateScale()));
    connect(ui->dblSpinA1, SIGNAL(valueChanged(double)), this, SLOT(actionUpdateVectors()));
    connect(ui->dblSpinA2, SIGNAL(valueChanged(double)), this, SLOT(actionUpdateVectors()));
    connect(ui->dblAlpha, SIGNAL(valueChanged(double)), this, SLOT(actionUpdateVectors()));
    connect(ui->dblRotation, SIGNAL(valueChanged(double)), this, SLOT(actionUpdateVectors()));
    connect(ui->toolSwapXY1, SIGNAL(clicked()), this, SLOT(actionSwapXY1()));
    connect(ui->toolSwapXY2, SIGNAL(clicked()), this, SLOT(actionSwapXY2()));


    /* setup vectors */
    basis[0] = 1.;
    basis[1] = 0.;

    matrixGS.resize(2);
    matrixSS.resize(2);
    matrixGS[0].resize(2);
    matrixGS[1].resize(2);
    matrixSS[0].resize(2);
    matrixSS[1].resize(2);

    matrixGS[0][0] = 1.;
    matrixGS[0][1] = 0.;
    matrixGS[1][0] = 0.;
    matrixGS[1][1] = 1.;

    matrixSS[0][0] = 1.;
    matrixSS[0][1] = 0.;
    matrixSS[1][0] = 0.;
    matrixSS[1][1] = 1.;

    ui->comboBox->setCurrentIndex(ui->comboBox->count()-1);
    actionUpdateBasis();
}

Model::~Model()
{
    delete ui;
}


void Model::showTreeContextMenu(const QPoint &pos)
{
    // for most widgets
    QPoint globalPos = ui->domainTree->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction(ui->actionAddStructure);
    myMenu.addAction(ui->actionRemoveStructure);
    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        // something was chosen
    }
    else
    {
        // nothing was chosen
    }
}


void Model::actionAddSuperstructure()
{
    SuperstructureDialog *ssDialog = new SuperstructureDialog(this);
    //connect(ssDialog, SIGNAL(rejected()), this, SLOT(addMatrix(qreal,qreal,qreal,qreal)));
    ssDialog->show();
}

void Model::actionRotate()
{
    qreal a[2], alpha;
    a[0] = ui->dblSpinA1->value();
    a[1] = ui->dblSpinA2->value();
    alpha = M_PI*ui->dblAlpha->value()/180;

    double basis[2][2], rotated[2][2];
    basis[0][0] = a[0]*cos(alpha);
    basis[0][1] = a[0]*sin(alpha);
    basis[1][0] = a[1]*cos(-alpha);
    basis[1][1] = a[1]*sin(-alpha);

    qreal rotation = M_PI * ui->dblRotation->value() / 180;
    qreal mRot[2][2];

    mRot[0][0] = cos(rotation);
    mRot[0][1] = -sin(rotation);
    mRot[0][1] = sin(rotation);
    mRot[0][1] = cos(rotation);

    /* perform rotation */
    rotated[0][0] = basis[0][0]*mRot[0][0] + basis[0][1]*mRot[0][1];
    rotated[0][1] = basis[0][0]*mRot[1][0] + basis[0][1]*mRot[1][1];
    rotated[1][0] = basis[1][0]*mRot[0][0] + basis[1][1]*mRot[0][1];
    rotated[1][1] = basis[1][0]*mRot[1][0] + basis[1][1]*mRot[1][1];


}


void Model::actionRemoveSuperstructure()
{
    qDeleteAll(ui->domainTree->selectedItems());

    /* additional tidying */
    int nItems = ui->domainTree->topLevelItemCount();
    for (int i = 0; i < nItems; i++)
    {
        int children =
                 ui->domainTree->topLevelItem(i)->childCount();
        if (!children)
        {
            delete ui->domainTree->topLevelItem(i);
        }
    }

}


void Model::actionScaleBasis()
{
    qreal a[2], alpha;
    a[0] = ui->dblSpinA1->value();
    a[1] = ui->dblSpinA2->value();
    alpha = M_PI*ui->dblAlpha->value()/180;

    double basis[2][2];
    basis[0][0] = a[0]*cos(alpha);
    basis[0][1] = a[0]*sin(alpha);
    basis[1][0] = a[1]*cos(-alpha);
    basis[1][1] = a[1]*sin(-alpha);

    double scale;
    if (fabs(basis[0][0]) < fabs(basis[0][1]))
    {
        if (fabs(basis[0][0]) < fabs(basis[1][0]))
            scale = 1/fabs(basis[0][0]);
        else
            scale = 1/fabs(basis[1][0]);
    } else {
        if (fabs(basis[0][1]) < fabs(basis[1][1]))
            scale = 1/fabs(basis[0][1]);
        else
            scale = 1/fabs(basis[1][1]);
    }

    if (scale < 10)
    {
        ui->dblMultiply->setValue(scale);
        actionUpdateVectors();
    }


}


void Model::actionSwapBasis()
{
    qreal val = ui->dblSpinA1->value();
    if(ui->dblSpinA2->value() == val)
        return; //return if values the same

    ui->dblSpinA1->setValue(ui->dblSpinA2->value());
    ui->dblSpinA2->setValue(val);

    actionUpdateVectors(); //update numbers

}


void Model::actionSwapXY1()
{
    bool checked = ui->toolSwapXY1->isChecked();
    xySwap[0] = !checked;
    actionUpdateVectors();
}

void Model::actionSwapXY2()
{
    bool checked = ui->toolSwapXY2->isChecked();
    xySwap[1] = !checked;
    actionUpdateVectors();
}

void Model::actionUpdateScale()
{
    ui->dblSpinA1->setValue(basis[0] * ui->dblMultiply->value());
    ui->dblSpinA2->setValue(basis[1] * ui->dblMultiply->value());
}


void Model::actionUpdateVectors()
{
    qreal a[2], alpha;
    a[0] = ui->dblSpinA1->value();
    a[1] = ui->dblSpinA2->value();
    alpha = M_PI*ui->dblAlpha->value()/180;

    double basis[2][2];
    basis[0][0] = a[0]*cos(alpha);
    basis[0][1] = a[0]*sin(alpha);
    basis[1][0] = a[1]*cos(-alpha);
    basis[1][1] = a[1]*sin(-alpha);

    /* get rotation matrix */
    qreal rotation = M_PI * ui->dblRotation->value() / 180;
    qreal mRot[2][2];

    mRot[0][0] = cos(rotation);
    mRot[0][1] = -sin(rotation);
    mRot[1][1] = sin(rotation);
    mRot[1][1] = cos(rotation);


    /* perform rotation */
    double rotated[2][2];
    rotated[0][0] = basis[0][0]*mRot[0][0] + basis[0][1]*mRot[0][1];
    rotated[0][1] = basis[0][0]*mRot[1][0] + basis[0][1]*mRot[1][1];
    rotated[1][0] = basis[1][0]*mRot[0][0] + basis[1][1]*mRot[0][1];
    rotated[1][1] = basis[1][0]*mRot[1][0] + basis[1][1]*mRot[1][1];

    if (xySwap[0])
    {
        double tmp = rotated[0][0];
        rotated[0][0] = rotated[0][1];
        rotated[0][1] = tmp;
    }
    if (xySwap[1])
    {
        double tmp = rotated[1][0];
        rotated[1][0] = rotated[1][1];
        rotated[1][1] = tmp;
    }

    QVector<QVector<QString> > numberStrings;
    numberStrings.resize(2);
    for (int i = 0; i < 2; i++)
    {
        numberStrings[i].resize(2);
        for (int j = 0; j < 2; j++)
            if (fabs(rotated[i][j]) < 1000)
            {
                numberStrings[i][j] = QString::number(rotated[i][j], 'f', 4);
                matrixGS[i][j] = rotated[i][j];
            } else {
                if((rotated[i][j]) < 1)
                {
                    numberStrings[i][j] = "-NaN";
                } else {
                    numberStrings[i][j] = "NaN";
                }
                matrixGS[i][j] = 0.;
            }
    }


    /* reformat text for special cases */
    QString text;
    text = "<html><head/><body><p><span style=\" font-weight:600;\">a</span><span style=\" font-weight:600; vertical-align:sub;\">1</span> =" +
            numberStrings[0][0] + ", " + numberStrings[0][1] + "</p></body></html>";
    text.replace("1.4142", QString::fromUtf8("√2"));
    text.replace("1.7321", QString::fromUtf8("√3"));
    text.replace("0.2500",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">4</span>"));
    text.replace("0.3333",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">3</span>"));
    text.replace("0.5000",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">2</span>"));
    text.replace("0.5774",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">√3</span>"));
    text.replace("0.6667",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">2</span>/<span style=\" vertical-align:sub;\">3</span>"));
    text.replace("0.7071",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">√2</span>"));
    text.replace("0.7500",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">3</span>/<span style=\" vertical-align:sub;\">4</span>"));
    text.replace("0.8000",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">4</span>/<span style=\" vertical-align:sub;\">5</span>"));
    text.replace("0.8660",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">√3</span>/<span style=\" vertical-align:sub;\">2</span>"));
    text.replace("-0.0000", "0.0000");
    text.replace(".0000", "");
    ui->labelA1XY->setText(text);


    text = "<html><head/><body><p><span style=\" font-weight:600;\">a</span><span style=\" font-weight:600; vertical-align:sub;\">2</span> =" +
           numberStrings[1][0] + ", " + numberStrings[1][1] + "</p></body></html>";
    text.replace("1.4142", QString::fromUtf8("√2"));
    text.replace("1.7321", QString::fromUtf8("√3"));
    text.replace("0.2500",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">4</span>"));
    text.replace("0.3333",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">3</span>"));
    text.replace("0.5000",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">2</span>"));
    text.replace("0.5774",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">√3</span>"));
    text.replace("0.6667",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">2</span>/<span style=\" vertical-align:sub;\">3</span>"));
    text.replace("0.7071",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">1</span>/<span style=\" vertical-align:sub;\">√2</span>"));
    text.replace("0.7500",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">3</span>/<span style=\" vertical-align:sub;\">4</span>"));
    text.replace("0.8000",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">4</span>/<span style=\" vertical-align:sub;\">5</span>"));
    text.replace("0.8660",
                 QString::fromUtf8("<span style=\" vertical-align:super;\">√3</span>/<span style=\" vertical-align:sub;\">2</span>"));
    text.replace("-0.0000", "0.0000");
    text.replace(".0000", "");
    ui->labelA2XY->setText(text);


}

void Model::actionTwinBasis()
{
    ui->dblSpinA2->setValue(ui->dblSpinA1->value());
}

void Model::actionUpdateBasis()
{
    int index = ui->comboBox->currentIndex();
    ui->dblSpinA2->setEnabled(true);
    disconnect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
               this, SLOT(actionTwinBasis()));
    ui->toolSwapXY1->setChecked(true);
    ui->toolSwapXY2->setChecked(false);
    xySwap[0] = false;
    xySwap[1] = false;
    switch(index)
    {
        case 0: //oblique p1
            ui->dblAlpha->setValue(55.0);
            ui->dblAlpha->setSingleStep(2.5);
            ui->dblAlpha->setMaximum(360);
            ui->dblAlpha->setMinimum(0);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            break;

        case 1: //oblique p2
            ui->dblAlpha->setValue(55.0);
            ui->dblAlpha->setSingleStep(2.5);
            ui->dblAlpha->setMaximum(360);
            ui->dblAlpha->setMinimum(0);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            break;

        case 2: //prim rect pm
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 3: //prim rect pg
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 4: //cent rect cm
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 5: //prim rect p2mm
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 6: //prim rect p2mg
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 7: //prim rect p2gg
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 8: //cent rect c2mm
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            break;

        case 9: //square p4
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            break;

        case 10: //square p4mm
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            break;

        case 11: //square p4g
            ui->dblAlpha->setValue(90.0);
            ui->dblAlpha->hide();
            ui->labelAlpha->hide();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            break;

        case 12: //hexagonal p3
            ui->dblAlpha->setValue(120);
            ui->dblAlpha->setSingleStep(60);
            ui->dblAlpha->setMaximum(120);
            ui->dblAlpha->setMinimum(60);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            ui->toolSwapXY1->setChecked(false);
            break;

        case 13: //hexagonal p3m1
            ui->dblAlpha->setValue(120);
            ui->dblAlpha->setSingleStep(60);
            ui->dblAlpha->setMaximum(120);
            ui->dblAlpha->setMinimum(60);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            ui->toolSwapXY1->setChecked(false);
            break;

        case 14: //hexagonal p31m
            ui->dblAlpha->setValue(120);
            ui->dblAlpha->setSingleStep(60);
            ui->dblAlpha->setMaximum(120);
            ui->dblAlpha->setMinimum(60);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            ui->toolSwapXY1->setChecked(false);
            break;

        case 15: //hexagonal p6
            ui->dblAlpha->setValue(120);
            ui->dblAlpha->setSingleStep(60);
            ui->dblAlpha->setMaximum(120);
            ui->dblAlpha->setMinimum(60);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            ui->toolSwapXY1->setChecked(false);
            break;

        case 16: //hexagonal p6mm
            ui->dblAlpha->setValue(120);
            ui->dblAlpha->setSingleStep(60);
            ui->dblAlpha->setMaximum(120);
            ui->dblAlpha->setMinimum(60);
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblSpinA2->setDisabled(true);
            connect(ui->dblSpinA1, SIGNAL(valueChanged(double)),
                    this, SLOT(actionTwinBasis()));
            ui->toolSwapXY1->setChecked(false);
            break;

        default:
            ui->dblAlpha->show();
            ui->labelAlpha->show();
            ui->dblAlpha->setValue(60);
            ui->dblAlpha->setSingleStep(1);
            ui->dblAlpha->setMaximum(360);
            ui->dblAlpha->setMinimum(0);
            ui->dblSpinA2->setEnabled(true);
            ui->toolSwapXY1->setChecked(false);
            break;

    }

    if (index < 9)
    {
        basis[0] = 1.4142;
        basis[1] = 1.;
    } else {
        basis[0] = 1.;
        basis[1] = 1.;
    }

    actionUpdateScale();
    actionUpdateVectors();
}


void Model::addMatrix(qreal m11, qreal m12,
                      qreal m21, qreal m22, QStringList &domains)
{
    matrixSS[0][0] = m11;
    matrixSS[0][1] = m12;
    matrixSS[1][0] = m21;
    matrixSS[1][1] = m22;

    QStringList m;
    m.append(QString::number(m11, 'f', 4));
    m.append(QString::number(m12, 'f', 4));
    m.append("|");
    m.append(QString::number(m21, 'f', 4));
    m.append(QString::number(m22, 'f', 4));

    QString label = "[" + m.join(" ").remove(".0000") + "]";

    /* determine if matrix already exists
     * append extra entries if found */
    QList<QTreeWidgetItem *> topItemMatches =
            ui->domainTree->findItems(label, Qt::MatchExactly);

    if (topItemMatches.count())
    {
        qDebug() << "Top level match found";
        int idItemMatch = ui->domainTree->indexOfTopLevelItem(topItemMatches.at(0)); //assume only one
        foreach(QString domain, domains)
        {
            QList<QTreeWidgetItem*> subItemMatches =
                    ui->domainTree->findItems(domain, Qt::MatchExactly | Qt::MatchRecursive, 0);

            for(QList<QTreeWidgetItem*>::iterator it = subItemMatches.begin();
                it != subItemMatches.end(); ++it)
            {
                QTreeWidgetItem* baseItem = *it;
                qDebug() << baseItem->text(0);
                int idTopLevel = ui->domainTree->indexOfTopLevelItem(baseItem);
                if(idTopLevel != idItemMatch)
                {
                    qDebug() << "ignored: " << baseItem->text(0);
                    continue;
                } else {
                    qDebug() << "removed:" << baseItem->text(0);
                    ui->domainTree->removeItemWidget(baseItem, 0);
                    break;
                }
            }


            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setText(0, domain);
            topItemMatches.at(0)->addChild(item);
        }


    } else {
        QTreeWidgetItem *newTopItem = new QTreeWidgetItem(ui->domainTree);
        newTopItem->setText(0, label);

        foreach(QString domain, domains)
        {
            QTreeWidgetItem * item = new QTreeWidgetItem();
            item->setText(0, domain);
            newTopItem->addChild(item);
        }
        ui->domainTree->addTopLevelItem(newTopItem);
    }



}

void Model::addDomains(QStringList &domains)
{

}

