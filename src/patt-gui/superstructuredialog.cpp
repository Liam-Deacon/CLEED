#include "superstructuredialog.hpp"
#include "ui_superstructuredialog.h"
#include "model.hpp"

#include <QVector>
#include <QInputDialog>
#include <QDebug>
#include <math.h>

SuperstructureDialog::SuperstructureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SuperstructureDialog)
{
    ui->setupUi(this);

    connect(ui->toolUnity,  SIGNAL(clicked()), this, SLOT(unityMatrix()));
    connect(ui->checkComm, SIGNAL(stateChanged(int)), this, SLOT(toggleCommensurate()));
    connect(ui->comboSuperlattice, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMatrix()));
    connect(ui->toolAddRotation, SIGNAL(clicked()), this, SLOT(addRotation()));
    connect(ui->toolClear, SIGNAL(clicked()), this, SLOT(clearAll()));
    connect(ui->toolRemoveRotation, SIGNAL(clicked()), this, SLOT(deleteRotation()));


}

void SuperstructureDialog::closeEvent(QCloseEvent *event)
{
    /* get list of domain operations */
    QStringList stringList;
    stringList << "R0";
    for(int i = 0; i < ui->listDomains->count(); i++)
    {
        QListWidgetItem *item = ui->listDomains->item(i);
        stringList.append(item->text());
    }

    /* add superlattice to parent */
    ((Model*)parentWidget())->addMatrix(ui->dblSpinM11->value(),
                                        ui->dblSpinM12->value(),
                                        ui->dblSpinM21->value(),
                                        ui->dblSpinM22->value(),
                                        stringList);

    event->setAccepted(true);
    event->accept();
}

SuperstructureDialog::~SuperstructureDialog()
{
    delete ui;
}

void SuperstructureDialog::addRotation()
{
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);
    inputDialog->setWindowIcon(QPixmap(":/curved_arrow_32x24.png"));

    /* initialise rotation and increment */
    double defaultRot = 60.;
    int items = ui->listDomains->count();
    if (items)
    {
        defaultRot += (qreal)ui->listDomains->item(items-1)->text().remove("R").toDouble();
    }

    bool ok;
    double rotation = inputDialog->getDouble(this, "Add Rotation",
                                        "Domain rotation:",
                                        defaultRot, -180, 180, 1, &ok);
    if (fabs(rotation) < 0.1 || !ok)
        return; //no change in rotation or cancelled

    /* check for duplicate entries */
    bool duplicate = false;
    for(int i = 0; i <items; i++)
    {
        QString item = ui->listDomains->item(i)->text();
        double tempRotation = item.remove("R").toDouble();
        if (fabs(tempRotation - rotation) < 0.1)
            duplicate = true;
    }

    if (!duplicate)
        ui->listDomains->addItem("R" + QString::number(rotation));

}


void SuperstructureDialog::deleteRotation()
{
    qDeleteAll(ui->listDomains->selectedItems());
}


void SuperstructureDialog::clearAll()
{
    ui->listDomains->clear();
}

void SuperstructureDialog::unityMatrix()
{
    ui->dblSpinM11->setValue(1);
    ui->dblSpinM12->setValue(0);
    ui->dblSpinM21->setValue(0);
    ui->dblSpinM22->setValue(1);
}

void SuperstructureDialog::updateMatrix()
{
    switch(ui->comboSuperlattice->currentIndex())
    {
    case 0:
    {
        break;
    }
    case 1:
        ui->dblSpinM11->setValue(1);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(1);
        break;
    case 2:
        ui->dblSpinM11->setValue(1);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(2);
        break;
    case 3:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(1);
        break;
    case 4:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(2);
        break;
    case 5:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(1);
        break;
    case 6:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(2);
        break;
    case 7:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(3);
        break;
    case 8:
        ui->dblSpinM11->setValue(7);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(7);
        break;
    case 9:
        ui->dblSpinM11->setValue(1);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(1);
        break;
    case 10:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(2);
        ui->dblSpinM22->setValue(1);
        break;
    case 11:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(1);
        ui->dblSpinM21->setValue(0);
        ui->dblSpinM22->setValue(2);
        break;
    case 12:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(-2);
        ui->dblSpinM21->setValue(2);
        ui->dblSpinM22->setValue(2);
        break;
    case 13:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(3);
        ui->dblSpinM22->setValue(1);
        break;
    case 14:
        ui->dblSpinM11->setValue(4);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(4);
        ui->dblSpinM22->setValue(1);
        break;
    case 15:
        ui->dblSpinM11->setValue(1);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(1);
        break;
    case 16:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(1);
        ui->dblSpinM21->setValue(-1);
        ui->dblSpinM22->setValue(2);
        break;
    case 17:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(2);
        ui->dblSpinM21->setValue(-1);
        ui->dblSpinM22->setValue(1);
        break;
    case 18:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(2);
        break;
    case 19:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(2);
        ui->dblSpinM21->setValue(2);
        ui->dblSpinM22->setValue(3);
        break;
    case 20:
        ui->dblSpinM11->setValue(1);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(2);
        break;
    case 21:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(-1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(3);
        break;
    case 22:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(-2);
        ui->dblSpinM21->setValue(2);
        ui->dblSpinM22->setValue(4);
        break;
    case 23:
        ui->dblSpinM11->setValue(4);
        ui->dblSpinM12->setValue(0);
        ui->dblSpinM21->setValue(2);
        ui->dblSpinM22->setValue(4);
        break;
    case 24:
        ui->dblSpinM11->setValue(2);
        ui->dblSpinM12->setValue(2);
        ui->dblSpinM21->setValue(-5);
        ui->dblSpinM22->setValue(5);
        break;
    case 25:
        ui->dblSpinM11->setValue(3);
        ui->dblSpinM12->setValue(1);
        ui->dblSpinM21->setValue(1);
        ui->dblSpinM22->setValue(3);
        break;
    }

}

void SuperstructureDialog::toggleCommensurate()
{
    bool checked = ui->checkComm->isChecked();
    if(checked)
    {
        ui->dblSpinM11->setSingleStep(1);
        ui->dblSpinM12->setSingleStep(1);
        ui->dblSpinM21->setSingleStep(1);
        ui->dblSpinM22->setSingleStep(1);
        ui->dblSpinM11->setDecimals(0);
        ui->dblSpinM12->setDecimals(0);
        ui->dblSpinM21->setDecimals(0);
        ui->dblSpinM22->setDecimals(0);
    } else {
        ui->dblSpinM11->setSingleStep(0.05);
        ui->dblSpinM12->setSingleStep(0.05);
        ui->dblSpinM21->setSingleStep(0.05);
        ui->dblSpinM22->setSingleStep(0.05);
        ui->dblSpinM11->setDecimals(4);
        ui->dblSpinM12->setDecimals(4);
        ui->dblSpinM21->setDecimals(4);
        ui->dblSpinM22->setDecimals(4);
    }
}
