#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "qcompile.hpp"
#include "calculations.hpp"
#include "model.hpp"
#include "preferences.hpp"
#include "lattice.hpp"

#ifndef _USE_MATH_DEFINES
    #define _USE_MATH_DEFINES
#endif
#include <math.h>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QSettings>
#include <QStandardPaths>
#if QT_VERSION >= 0x040300
	#include <QtSvg/QSvgGenerator>
 #endif
#include <QUrl>

#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPalette>
#include <QPolygon>
#include <QGraphicsPolygonItem>
#include <QGraphicsPathItem>
#include "qtarrowitem.hpp"


#include "paintdialog.hpp"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupMenus(ui);
    setupActions(ui);

    zoom = 1.;

    /* mobile operating systems should calculate less spots */
    #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
        this->imax = 5;
    #else
        this->imax = 10;
    #endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::actionOpen()
{
    const QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString defaultFilter = tr("Pattern (*.patt)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    defaultDir,
                                                    tr("Pattern (*.patt);;Text Files(*.txt);;All Files (*.*)"),
                                                    &defaultFilter);
    loadFile(fileName);

    return 0;
}


void MainWindow::actionAddStructure()
{

}


void MainWindow::actionAbout()
{
    /* About Pattern dialog */

    QString title = "About " + QCoreApplication::applicationName();
    QString description = __APPDESCRIPTION__;
    description.append("\n\n");
    description.append("Version: " + QCoreApplication::applicationVersion() + "\n");
    description.append("Author: ").append(__APPAUTHOR__).append("\n");
    description.append("Email: ").append(__APPEMAIL__).append("\n");
    QString contrib = __CONTRIBUTORS__;
    if (!contrib.isEmpty() && ! contrib.isNull()) {
        description.append("Contributors:").append(__CONTRIBUTORS__).append("\n");
    }
    description.append("\n").append(__APPNOTES__).append("\n\n");
    description.append("Compiled ");
    description.append("by ").append(__USER__).append("@").append(__HOST__).append("\n");
    description.append("Time-stamp: ").append(__TIME__).append(" on ").append(__DATE__).append("\n");
    description.append("Platform: ").append(__OS__).append("\n");
    description.append("Qt Version: ").append(QT_VERSION_STR);
    QString depends = __DEPENDS__;
    if (!depends.isEmpty() && !depends.isNull() && depends.compare(QT_VERSION_STR)) {
        description.append("\n(").append(depends).append(")");
    }
    QMessageBox about;
    QIcon winIcon(":/info_16x32.png");
    QPixmap pixmap(":/pattern_128x128.png");
    about.setIconPixmap(pixmap);
    about.setWindowIcon(winIcon);
    about.setText(description);
    about.setWindowTitle(title);
    about.exec();
}

void MainWindow::actionContact()
{
    //Open email client for app contact
    //openUrl is static method of class QDesktopServices, so no need to create instance of it.
    QDesktopServices::openUrl(QUrl(QString("mailto: %1?subject=%2 feedback&body=").arg(__APPEMAIL__).arg(__APPNAME__)));
}


void MainWindow::actionCopy()
{
//    QGraphicsScene scene(ui->graphicsView->scene());
//    scene.clearSelection();                                                  // Selections would also render to the file
//    scene.setSceneRect(scene.itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
//    QImage image(scene.sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
//    image.fill(Qt::transparent);                                              // Start all pixels transparent

//    QPainter painter(&image);
//    scene.render(&painter);
//    image.save("C:\\Users\\Liam\\Desktop\\file_name.png");
    //QClipboard().setImage(ui->graphicsView);
}


int MainWindow::actionExport()
{
    /* Export QGraphicsView as image using QPrinter */

    return 0;
}

void MainWindow::actionModelInput()
{
    Model *model = new Model(this);
    model->show();
}

void MainWindow::actionPrefences()
{
    preferences *prefs = new preferences;
    prefs->show();
}


int MainWindow::actionSave()
{
    QString selfilter = tr("Portable Network Graphics (*.png)");
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save As...",
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            tr("Windows Bitmap (*.bmp);;") +
            tr("JPEG (*.jpg);;") +
            tr("Portable Document Format (*.pdf);;") +
            tr("Portable Network Graphics (*.png);;") +
            tr("Scalable Vector Graphics (*.svg);;") +
            tr("Tag Image Format (*.tif);;"),
            &selfilter
    );


    //render output based on format chosen in fileDialog
    int format = -1;

    QSvgGenerator svgGen;
    QPainter painter(&svgGen);


    switch(format)
    {
        case FMT_SVG:

            svgGen.setFileName(fileName);
            svgGen.setSize(QSize(200, 200));
            svgGen.setViewBox(QRect(0, 0, 200, 200));
            svgGen.setTitle(tr("SVG Generator Example Drawing"));
            svgGen.setDescription(tr("An SVG drawing created by the SVG Generator "
                                        "Example provided with Qt."));
            ui->graphicsView->scene()->render(&painter);

        break;

        default:
            break;
    }


    return 0; //save successful
}


void MainWindow::actionTranslate(QPointF translation)
{
    ui->graphicsView->translate(translation.x(), translation.y());
}


void MainWindow::actionZoom(bool state)
{
    if (state)
    {
        ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else
    {
        ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
}


void MainWindow::actionZoom(int steps)
{

    #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) //mobile platform => less zoom out
        double scaleFactor = 0.1;
        const qreal minFactor = 0.1;
        const qreal maxFactor = 10.0;
    #else
        double scaleFactor = 0.1;
        const qreal minFactor = 0.1;
        const qreal maxFactor = 1.9;
    #endif

    qreal zoomFactor = 1;
    if (steps > 0)
    {
        if (zoom < maxFactor)
        {
            zoom += scaleFactor;
            zoomFactor = 1.1;
        }
    }
    else
    {
        if (zoom > minFactor+scaleFactor)
        {
            zoom -= scaleFactor;
            zoomFactor = 0.9;
        }
    }

    ui->graphicsView->scale(zoomFactor, zoomFactor);
    ui->statusBar->showMessage("Zoom: " + QString::number(zoom) + "x", 500);
}

void MainWindow::loadFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //!TODO: load data
    QStringList comments;
    QString title;

    QPointF a1, a2;
    Matrix2x2 mat;

    QStringList ops;
    quint8 nDom;

    /* Lines beginning with '#' are interpreted as comments. */
    QStringList text;
    QString line;
    while(! in.atEnd())
    {
        line = in.readLine();
        if (!line.startsWith("#"))
        {
            if (!line.startsWith('c'))
                text.append(line.simplified());
            else
                comments.append(line);
        }
    }
    file.close();

    //process text
    line = text.at(0);
    QTextStream stream(&line);


    qreal a[2], radius;
    for (quint8 i=0; i < text.size(); i++)
    {
        line = text.at(i);
        stream.seek(0);

        switch (i)
        {
            case 0: //a1
                stream >> a[0] >> a[1];
                a1.setX(a[0]);
                a1.setY(a[1]);
                break;

            case 1: //a2
                stream >> a[0] >> a[1];
                a2.setX(a[0]);
                a2.setY(a[1]);
                break;

            case 2:
                stream >> radius;
                break;

            case 3:
                qreal domains;
                stream >> domains;
                nDom = (quint8)domains;
                break;

            case 4: //superstructure matrix line 1
                stream >> mat.m[0][0] >> mat.m[0][1];
                break;

            case 5: //superstructure matrix line 2
                stream >> mat.m[1][0] >> mat.m[1][1];
                break;

            default: //domain rotation or mirror
                QString op;
                stream >> op;
                if (!(op.startsWith('R') || op.startsWith('S')))
                {
                    QString bufferString = op, superMatrix;
                    QStringList superMatrixList = op.replace("\t", " ").trimmed().split(" ");
                    for(int k = superMatrixList.count() - 1; k >= 0; --k)
                    {
                        QString m = superMatrixList.at(k);
                        if (m.trimmed().isEmpty())
                        {
                            superMatrixList.removeAt(k);
                        }
                    }
                    superMatrix.append(superMatrixList[0] + " " + superMatrixList[1]);

                    //2nd matrix line
                    stream >> op;
                    bufferString = op;
                    superMatrixList = op.replace("\t", " ").trimmed().split(" ");
                    for(int k = superMatrixList.count() - 1; k >= 0; --k)
                    {
                        QString m = superMatrixList.at(k);
                        if (m.trimmed().isEmpty() || m.startsWith('M'))
                        {
                            superMatrixList.removeAt(k);
                        }
                    }
                    superMatrix.append(" " + superMatrixList[0] + " " + superMatrixList[1]);

                    //check validity
                    superMatrixList = superMatrix.trimmed().split(" ");
                    bool checkc, check;
                    foreach(QString elem, superMatrixList)
                    {

                        qreal m = (qreal)elem.toFloat();
                        if (fabs(m) < 0.0005 || fabs(m) > 100)
                        {
                            checkc = true;
                        }
                        else {
                            checkc = false;
                        }
                        if (check && checkc)
                            check = checkc;
                        else
                            check = false;

                    }
                    op = superMatrix;
                }
                ops.append(op);
        }
    }

    if ((a1.x() == 0 && a1.y() == 0) ||
            (a2.x() == 0 && a2.y() == 0))
    {
        ui->statusBar->showMessage("Invalid basis vectors in '"
                                   + QFileInfo(fileName).fileName() + "'", 2000);
        QApplication::restoreOverrideCursor();
        return;
    }

    if (fabs(mat.m[0][0]) == 0 && fabs(mat.m[0][1]) == 0
            && fabs(mat.m[1][0]) == 0 && fabs(mat.m[1][0]))
    {
        ui->statusBar->showMessage("Warning: Invalid supercell");
        mat = Matrix2x2();
    }

    Lattice lattice(a1, a2);
    SuperLattice superLattice(lattice, mat);

    QList<Domain> domainList;
    QString R0("R0");
    Domain dom(superLattice, R0);
    domainList.append(dom);
    foreach(QString op, ops)
    {
        Domain domain(superLattice, op);
        domainList.append(domain);
    }

    QList<SuperLattice> superLatticeList;
    superLatticeList.append(superLattice);

    LatticeStructure model(lattice, superLatticeList, domainList);

    if(latticeStructures.count() < 3)
        latticeStructures.append(model);
    else
    {
        latticeStructures.move(0,1);
        latticeStructures.move(1,2);
        latticeStructures[0] = model;
    }

    calculatePattern(model);

    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Loaded \"") +
                             QFileInfo(fileName).fileName() +
                             "\"", 2000);
}

void MainWindow::loadSettings() {
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setFallbacksEnabled(true);
    settings.setDefaultFormat(QSettings::IniFormat);

    settings.beginGroup("MAIN_WINDOW");
    restoreGeometry(settings.value("WindowGeometry").toByteArray());
    restoreState(settings.value("WindowState").toByteArray());
    settings.endGroup();

//    settings.beginGroup("EXTERNAL_PATHS");
//    savePath = settings.value("savePath", savePath).toString();
//    settings.endGroup();

    settings.beginGroup("USER_DEFINED");
//    lattui.checkBox_Advanced->setChecked(settings.value("chkAdvanced", lattui.checkBox_Advanced->isChecked()).toBool());
//    lattui.checkBox_d->setChecked(settings.value("chkd", lattui.checkBox_d->isChecked()).toBool());
//    lattui.checkBox_UseInputFile->setChecked(settings.value("chkUseInput", lattui.checkBox_UseInputFile->isChecked()).toBool());
//    lattui.comboPacking->setCurrentIndex(settings.value("packing", lattui.comboPacking->currentIndex()).toInt());
//    lattui.dblspin_h->setValue(settings.value("h", lattui.dblspin_h->value()).toDouble());
//    lattui.dblspin_k->setValue(settings.value("k", lattui.dblspin_k->value()).toDouble());
//    lattui.dblspin_l->setValue(settings.value("l", lattui.dblspin_l->value()).toDouble());
//    lattui.doubleSpinBox_a->setValue(settings.value("a", lattui.doubleSpinBox_a->value()).toDouble());
//    lattui.doubleSpinBox_c->setValue(settings.value("c", lattui.doubleSpinBox_c->value()).toDouble());
//    lattui.doubleSpinBox_d->setValue(settings.value("d", lattui.doubleSpinBox_d->value()).toDouble());
//    lattui.lineInput->setText(settings.value("inputFile", lattui.lineInput->text()).toString());
//    lattui.lineOutput->setText(settings.value("outputFile", lattui.lineOutput->text()).toString());
//    lattui.lineModel->setText(settings.value("model", lattui.lineModel->text()).toString());
//    advancedText = settings.value("jmolScript", advancedText).toString();
    settings.endGroup();

}


void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}


void MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Recent Files"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    //!TODO: output model
    //out << textEdit->toPlainText();



    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    QString message = fileName;
    statusBar()->showMessage( message.prepend("\"").append("\" ").append(tr("saved")), 2000);
}


void MainWindow::saveSettings() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setFallbacksEnabled(true);
    settings.setDefaultFormat(QSettings::IniFormat);

    settings.beginGroup("MAIN_WINDOW");
    settings.setValue("WindowGeometry", saveGeometry());
    settings.setValue("WindowState", saveState());
    settings.endGroup();

    // leave for user dialog
    /*
    settings.beginGroup("EXTERNAL_PATHS");
    settings.setValue("lattExePath",lattExePath);
    settings.endGroup();
    */

    settings.beginGroup("USER_DEFINED");
    //settings.setValue("chkAdvanced", lattui.checkBox_Advanced->isChecked());
    settings.endGroup();

}


void MainWindow::setupActions(Ui::MainWindow *ui)
{
    /* setup initial main window actions here */
    connect(ui->actionAboutPattern, SIGNAL(triggered()), this, SLOT(actionAbout()));
    connect(ui->actionContact, SIGNAL(triggered()), this, SLOT(actionContact()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(actionCopy()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));
    connect(ui->actionInput, SIGNAL(triggered()), this, SLOT(actionModelInput()));
    connect(ui->actionPreferences, SIGNAL(triggered()), this, SLOT(actionPrefences()));
    connect(ui->actionRedraw, SIGNAL(triggered()), this, SLOT(drawPattern()));
    connect(ui->actionZoom, SIGNAL(toggled(bool)), this, SLOT(actionZoom(bool)));

    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}


void MainWindow::setupMenus(Ui::MainWindow *ui)
{
    /* setup additional menus on initialisation here */
    for (int i = 0; i < maxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }
    for (int i = 0; i < maxRecentFiles; ++i)
        ui->menuOpenRecent->addAction(recentFileActs[i]);
    updateRecentFileActions();
}


void MainWindow::setupToolbar(Ui::MainWindow *ui)
{
    ui->statusBar->font();
}



void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setFallbacksEnabled(true);
    settings.setDefaultFormat(QSettings::IniFormat);

    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > maxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}


inline QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void MainWindow::updateRecentFileActions()
{
    /* update the recent files list in menu bar */
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setFallbacksEnabled(true);
    settings.setDefaultFormat(QSettings::IniFormat);

    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)maxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1. %2").arg(i).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < maxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

}

void MainWindow::calculateGS()
{

}

void MainWindow::calculateSS()
{

}

void MainWindow::drawPattern()
{
    calculatePattern(latticeStructures[0]);

//    QPainter *painter = new QPainter();
//    painter->setWindow(-200, -200, 200, 200);
//    painter->drawArc(0, 0, 100, 100, 360*16, 1);

//    QGraphicsScene *scene = new QGraphicsScene();
//    ui->graphicsView->setScene(scene);
//    ui->graphicsView->setSceneRect(0, 0, 400, 400);
//    QGraphicsLineItem *line = scene->addLine( QLineF( 0, 0, 200, 200 ) );
//    QBrush brush = QBrush(QColor("blue"), Qt::SolidPattern);
//    QPen pen = QPen(brush, 10, Qt::SolidLine);
//    line->setPen(pen);

//    pen.setStyle(Qt::SolidLine);
//    QGraphicsEllipseItem *circ = scene->addEllipse(200, 200, 200, 200);

}


void MainWindow::calculatePattern(LatticeStructure &structure)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Calculations c;

    /* determine scaling */
    qreal radius = 1.1;

    quint8 eV = 0;
    if (eV)
    {
      //calculate radius of Ewald construction at given energy
      long double lambda;
      lambda = sqrt(2*9.11*1.6018*eV)/6.62;
      lambda /= 2*M_PI;
      radius = (qreal) lambda;
    }

    /************/
    /* GS SPOTS */
    /************/

    Lattice lattice = structure.getLattice();
    QList <QPointF> listGS;
    QStringList indicesGS;

    QPointF a[2];
    a[0] = lattice.getBasisVector(0);
    a[1] = lattice.getBasisVector(1);

    qreal magnitude[2];
    magnitude[0] = c.magnitude(a[0].x(), a[0].y());
    magnitude[1] = c.magnitude(a[1].x(), a[1].y());

    radius *= qMax(magnitude[0], magnitude[1]);

    radius = MAX_RADIUS/radius; //scale to radius


    /* calculate GS vectors */
    QPointF a1, a2;
    a1.setX( radius*a[1].y());
    a1.setY(-radius*a[1].x());
    a2.setX(-radius*a[0].y());
    a2.setY( radius*a[0].x());

    /* define max. values */
    int imax_1 = this->imax * (int)(MAX_RADIUS / c.magnitude(a1.x(), a1.y()));
    int imax_2 = this->imax * (int)(MAX_RADIUS / c.magnitude(a2.x(), a2.y()));


    /* get GS spot coordinates */
    QPointF gsSpot;
    for (int i1 = -imax_1; i1 < imax_1; i1 ++)
    {
        for (int i2 = -imax_2; i2 < imax_2; i2 ++)
        {
            gsSpot.setX(i1 * a1.x() + i2 * a2.x());
            gsSpot.setY(i1 * a1.y() + i2 * a2.y());
            listGS.append(gsSpot);
            indicesGS.append(QString::number(i1) + "," + QString::number(i2));
        }
    }


    /************/
    /* SS SPOTS */
    /************/

    QList<Domain> domainList = structure.getDomainList();
    QVector <QList <QPointF> > listSS;
    QVector <QStringList> indicesSS;

    listSS.resize(domainList.count());
    indicesSS.resize(domainList.count());


    /* loop over domains */
    for(int i_dom = 0; i_dom < domainList.count(); i_dom++)
    {

        Domain domain = domainList[i_dom];

        /* setup superlattice */
        SuperLattice superLattice = domain.getSuperLattice();
        Matrix2x2 ssMatrix = superLattice.getSuperMatrix();

        qreal M11, M12, M21, M22;           /* SS matrix */
        qreal m11, m12, m21, m22;           /* used SS matrix */
        qreal N11, N12, N21, N22;           /* Rotation  matrix */
        qreal phi;                          /* Rotation angle */

        qreal aux2;


        /* setup SS matrix */

        M11 = ssMatrix.m[0][0];
        M12 = ssMatrix.m[0][1];
        M21 = ssMatrix.m[1][0];
        M22 = ssMatrix.m[1][1];

        /* matrix */
        qreal det;
        QString operation = domain.getDomainOp();

        if (operation.startsWith('R'))
        {
            const QString lineBuffer = operation.mid(1);
            phi = lineBuffer.toFloat();
            phi *= M_PI/180;
            det  = a[0].x()*a[1].y() - a[0].y()*a[1].x();
            aux2 = (a[0].x()*a[1].x() + a[0].y()*a[1].y()) / det;

            N11 = cos(phi) - aux2*sin(phi);
            N12 = sin(phi) * (magnitude[0] * magnitude[0]) / det;
            N21 = -sin(phi)* (magnitude[1] * magnitude[1]) / det;
            N22 = cos(phi) + aux2*sin(phi);

            m11 = M11*N11 + M12*N21;
            m12 = M11*N12 + M12*N22;
            m21 = M21*N11 + M22*N21;
            m22 = M21*N12 + M22*N22;

            M11 = m11; M12 = m12; M21 = m21; M22 = m22;

            qDebug() << "Rotation operation: " << operation;

        } else if (operation.startsWith('S'))
        {
            QString lineBuffer = operation;
            det  = a[0].x()*a[1].y() - a[0].y()*a[1].x();
            aux2 = (a[0].x()*a[1].y() + a[0].y()*a[1].x()) / det;

            if (lineBuffer.at(1) == 'x')
            {
                N11 = aux2;
                N12 = -2 * a[0].x()*a[0].y() / det;
                N21 =  2 * a[1].x()*a[1].y() / det;
                N22 = -aux2;
            }
            if (lineBuffer.at(1) == 'y')
            {
                N11 = -aux2;
                N12 =  2 * a[0].x()*a[0].y() / det;
                N21 = -2 * a[1].x()*a[1].y() / det;
                N22 = aux2;
            }

            m11 = M11*N11 + M12*N21;
            m12 = M11*N12 + M12*N22;
            m21 = M21*N11 + M22*N21;
            m22 = M21*N12 + M22*N22;

            M11 = m11; M12 = m12; M21 = m21; M22 = m22;

            qDebug() << "Mirror operation: " << operation;

        } else {
            /* default matrix operation */
            m11 = M11; m12 = M12; m21 = M21; m22 = M22;
            N11 = N22 = 1.; N12 = N21 = 0.;
        }

        /* commensurate or not? */
        bool comm = !( ( fabs(m11) - (int)(fabs(m11)+0.1) > 0.05) ||
                       ( fabs(m12) - (int)(fabs(m12)+0.1) > 0.05) ||
                       ( fabs(m21) - (int)(fabs(m21)+0.1) > 0.05) ||
                       ( fabs(m22) - (int)(fabs(m22)+0.1) > 0.05) );

        qDebug() << "Commensurate: " << comm;

        /* det of matrix */
        det  = m11*m22 - m12*m21;
        qreal aux1 = 1/det;
        det = fabs(det);

        QPointF b[2];
        b[0].setX(aux1 *(m22*a1.x() - m21*a2.x()));
        b[0].setY(aux1 *(m22*a1.y() - m21*a2.y()));
        b[1].setX(aux1 *(m11*a2.x() - m12*a1.x()));
        b[1].setY(aux1 *(m11*a2.y() - m12*a1.y()));

        /* print info */
        printf("%d:\tDet: %.1f\n",i_dom+1, det);
        printf("N = (%5.1f %5.1f)\n", N11, N12);
        printf("    (%5.1f %5.1f)\n", N21, N22);
        printf("M = (%5.1f %5.1f)\t(M^-1)t = (%5.2f %5.2f)",
              m11,  m12,              aux1*m22,-aux1*m21);
        printf("\tb1* = (%6.2f)\tb2* = (%6.2f)\n", b[0].x()/radius, b[1].x()/radius);
        printf("    (%5.1f %5.1f)\t          (%5.2f %5.2f)",
              m21,  m22,             -aux1*m12, aux1*m11);
        printf("\t      (%6.2f)\t      (%6.2f)\n", b[0].y()/radius, b[1].y()/radius);

        /* SS spots */
        /* define max. values */
        int smax_1 = this->imax * (int)(MAX_RADIUS / c.magnitude(b[0].x(), b[0].y()) );
        int smax_2 = this->imax * (int)(MAX_RADIUS / c.magnitude(b[1].x(), b[1].y()) );

        /* write SS spot positions  to output */
        QPointF ssSpot;
        for (int s1 = -smax_1; s1 < smax_1; s1 ++)
            for (int s2 = -smax_2; s2 < smax_2; s2 ++)
            {
                ssSpot.setX(s1 * b[0].x() + s2 * b[1].x());
                ssSpot.setY(s1 * b[0].y() + s2 * b[1].y());

                listSS[i_dom].append(ssSpot);

                qreal ind_1 = (s1*m22 - s2*m12);
                qreal ind_2 = (s2*m11 - s1*m21);

                /* check if integral order spot */
                if ((fabs(ind_1*aux1) - (int)(fabs(ind_1*aux1)+0.1) > 0.05) ||
                    (fabs(ind_2*aux1) - (int)(fabs(ind_2*aux1)+0.1) > 0.05))
                {

                    if (!comm)
                    /* incommensurate */
                        indicesSS[i_dom].append(QString::number(ind_1*aux1,'f', 2) +
                                                "," + QString::number(ind_2*aux1));
                    else
                    {
                    /* commensurate (fractions) */

                        QString text;
                        int ind_1_int = (int) (ind_1*1.01);
                        int ind_2_int = (int) (ind_2*1.01);

                        int det_int = (int) (det*1.01);
                        Calculations calc;
                        switch (calc.isFraction(&ind_1_int, &det_int))
                        {
                            case(-1):
                                text.append("0, ");
                                break;
                            case(0):
                                text = QString("%1, ").arg(QString::number(ind_1_int));
                                break;
                            case(1):
                                text = QString("%1/%2, ").arg(ind_1_int).arg(det_int);
                                break;
                        }

                        det_int = (int) (det*1.01);
                        switch (calc.isFraction(&ind_2_int, &det_int))
                        {
                            case(-1):
                                text.append("0");
                                break;
                            case(0):
                                text.append(QString::number(ind_2_int));
                                break;
                            case(1):
                            text.append(QString("%1/%2").arg(ind_2_int).arg(det_int));
                                break;
                        }

                        indicesSS[i_dom].append(text);

                    }
                }
                else
                {
                    indicesSS[i_dom].append("");
                }
            }

    /*
     If the Mii are not integer, the superstructure is incommensurate.
     In this case the multple scattering spots must be calculated
     separately
    */

    if (!comm)
    {
    /*
     write multiple scattering SS spots to output
    */

        QPointF ssSpot;
        for (int i1 = -imax_1; i1 < imax_1; i1 ++)
            for (int i2 = -imax_2; i2 < imax_2; i2 ++)
            {
                if( (i1 != 0) || (i2 != 0) )
                {
                    gsSpot.setX(i1 * a[0].x() + i2 * a[1].x());
                    gsSpot.setY(i1 * a[0].y() + i2 * a[1].y());
                    for (int s1 = -smax_1; s1 < smax_1; s1 ++)
                        for (int s2 = -smax_2; s2 < smax_2; s2 ++)
                            {
                                ssSpot.setX(gsSpot.x() + s1 * b[0].x() + s2 * b[1].x());
                                ssSpot.setY(gsSpot.y() + s1 * b[0].x() + s2 * b[1].y());
                                if((s1 != 0) || (s2 != 0))
                                {
                                    listSS[i_dom].append(ssSpot);
                                    indicesSS[i_dom].append("");
                                }
                            } /* for s1/2 */
                }   /* if != 0  */
            }    /* for i1/2 */
        }     /* if fabs .. */
    } /* for i_dom */


    qreal mag, imax;
    if (magnitude[0] > magnitude[1])
    {
        mag = magnitude[0];
        imax = imax_1;
    } else {
        mag = magnitude[1];
        imax = imax_2;
    }


    QGraphicsScene *scene = new QGraphicsScene();
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setScene(scene);
    //ui->graphicsView->setSceneRect(-imax_1 * mag, -imax_2 * mag, imax_1*mag, imax_2*mag*2);
    ui->graphicsView->centerOn(0, 0);

    qreal spotSize = 30;


    QtArrowItem arrow(0,0, true, 10, 10, true, 0, scene);
    arrow.show();

    QBrush brush = QBrush(QColor("blue"), Qt::NoBrush);
    QPen pen = QPen(brush, 10, Qt::SolidLine);
    pen.setColor(QColor("blue"));
    pen.setCapStyle(Qt::FlatCap);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(spotSize/8);
    pen.setJoinStyle(Qt::MiterJoin); //square
    //pen.setJoinStyle(Qt::RoundJoin); //square

    ui->graphicsView->centerOn(MAX_RADIUS, MAX_RADIUS);



    qreal rt2 = sqrt(2);
    //qreal width = spotSize/rt2; //square
    qreal width = spotSize * (sqrt(3)/2); //triangle

    QGraphicsItemGroup *indices =  new QGraphicsItemGroup;
    indices->setFlag(QGraphicsItem::ItemIsSelectable);
    QGraphicsItemGroup *spots = new QGraphicsItemGroup;
    spots->setFlag(QGraphicsItem::ItemIsSelectable);

    //Circle circle = Circle(spotSize);


    qreal pSize = spotSize;
    qreal pWidth = pSize / sqrt(2);
    qreal delta = pSize * (0.5 * sqrt(3));
    qreal quad = pSize * 0.5;

    QPainterPath path;
    path.moveTo(-quad, 2*quad);
    path.lineTo(quad, 2*quad);
    path.lineTo(quad, quad);
    path.lineTo(2*quad, quad);
    path.lineTo(2*quad, -quad);
    path.lineTo(quad, -quad);
    path.lineTo(quad, -2*quad);
    path.lineTo(-quad, -2*quad);
    path.lineTo(-quad, -quad);
    path.lineTo(-2*quad, -quad);
    path.lineTo(-2*quad, quad);
    path.lineTo(-quad, quad);
    path.closeSubpath();
    //path.lineTo(2*quad, quad);
    //path.lineTo(pWidth, 0);
    //path.lineTo(0, -pWidth);
    //path.closeSubpath(); //plus

    int iSpot = 0;
    foreach(QPointF spot, listGS)
    {

        QPainterPath t = path;
        t.translate(spot);



        QGraphicsPathItem *p = scene->addPath(t, pen, brush);

        QGraphicsItem *c = scene->addRect(spot.x()-spotSize, spot.y()-spotSize, 2*spotSize, 2*spotSize);
        //p->setRotation(45);
        //p->translate(spot.x(), spot.y());

        //QGraphicsEllipseItem *circ = scene->addEllipse(spot.x(), spot.y(), 5, 5);
        QGraphicsTextItem *label = scene->addText(indicesGS[iSpot]);

        //circ->setPen(pen);
        //circ->setBrush(brush);

        label->setPos(spot.x()-spotSize/2 - spotSize*0, spot.y()+spotSize);
        label->setFlag(QGraphicsItem::ItemIsMovable);
        label->setFlag(QGraphicsItem::ItemIsSelectable);
        label->setDefaultTextColor(pen.color());

        ui->graphicsView->setAlignment(Qt::AlignLeft);
        label->setFont(QFont("Times", spotSize, 800));
        indices->addToGroup(label);

        //indices->hide();
        iSpot++;

    }

    spotSize /= 3;
    for(int i = 0; i < listSS.count(); i++)
    {
        for (int j = 0; j < listSS[i].count(); j++)
        {
            QPointF spot = listSS[i][j];
            QPainterPath t = path;
            t.translate(spot);

            QGraphicsEllipseItem *p = scene->addEllipse(spot.x()-spotSize, spot.y()-spotSize, spotSize*2, spotSize*2);

            QGraphicsTextItem *label = scene->addText(indicesSS[i][j]);

            label->setPos(spot.x()-spotSize/2 - spotSize, spot.y()+spotSize);
            label->setFlag(QGraphicsItem::ItemIsMovable);
            label->setFlag(QGraphicsItem::ItemIsSelectable);
            label->setDefaultTextColor(pen.color());

        }
    }

    scene->addItem(indices);
    scene->addItem(spots);


//    PaintDialog *pd = new PaintDialog;
//    pd->show();

    QApplication::restoreOverrideCursor();
}

void MainWindow::wheelEvent(QWheelEvent* event)
{

    // Declare below as class member vars and set default values as below

    if (ui->actionZoom->isChecked())
    {    // Typical Calculations (Ref Qt Doc)
        const int degrees = event->angleDelta().y() / 8;
        int steps = degrees / 15;
        actionZoom(steps);
        return;
    }

    if (!ui->actionTranslate->isChecked() && event->type() == QEvent::GraphicsSceneWheel)
    {

    }


}
