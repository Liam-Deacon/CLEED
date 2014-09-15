#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QList>
#include <QPointF>
#include <QWheelEvent>
#include <QVector>

#include "config.hpp"
#include "drawing.hpp"
#include "latticestructure.hpp"

typedef QVector <QVector <qreal> > BASIS;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QList<LatticeStructure> latticeStructures;

public slots:

    void actionAbout();
    void actionContact();
    void actionAddStructure();
    void wheelEvent(QWheelEvent* event);

private:

    /* setup gui */
    void setupGui();
    void setupMenus(Ui::MainWindow *ui);
    void setupActions(Ui::MainWindow *ui);
    void setupToolbar(Ui::MainWindow *ui);

    Ui::MainWindow *ui;
    Pattern *pattern;
    quint8 mode, imax;

    qreal zoom;

    enum {maxRecentFiles = 10};
    QAction *recentFileActs[maxRecentFiles];
    QString curFile;

    inline QString strippedName(const QString &fullFileName);

private slots:
    void actionCopy();
    void actionModelInput();
    void actionPrefences();
    void actionTranslate(QPointF translation);
    void actionZoom(int steps);
    void actionZoom(bool state);


    int actionOpen();
    int actionSave();
    int actionExport();


    void loadSettings();
    void saveSettings();

    /* recent files */
    void setCurrentFile(const QString &fileName);
    void openRecentFile();
    void updateRecentFileActions();

    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);


    void calculateGS();
    void calculateSS();
    void calculatePattern(LatticeStructure &structure);
    void drawPattern();

};

#endif // MAINWINDOW_HPP
