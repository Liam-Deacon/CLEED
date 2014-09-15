#ifndef LATTMAINWINDOW_H
#define LATTMAINWINDOW_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif
#include <QtNetwork/QHostInfo>
#include <QtCore/QSysInfo>

#include "latt_gui.hpp"

#include "ui_lattmainwindow.h"
#include "ui_advancedoutput.h"

#define LATTWINDOWTITLE "Lattice GUI"

#ifndef FALLBACK_SETTING
#define FALLBACK_SETTING true
#endif

class lattMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit lattMainWindow(QWidget *parent = 0);
    ~lattMainWindow();

    //key voids
    void initGui();
    void loadSettings();
    void saveSettings();
    QString AdvancedScript(QString rawScript);
    void addScript(QString script, QString filePath);

    //variables
    QString advancedText;

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void checkInput();
    void checkAdvanced();
    void checkLimitUC();
    void updateH();
    void updateK();
    void updateL();
    void updateA();
    void updateC();
    void updateInputMethod();
    void updateStructure();
    void updateModel();
    void updateInput();
    void updateOutput();
    QString AdvancedXYZ();
    QString pressedAdvancedBtn();
    QString pressedInputBtn();
    QString pressedOutputBtn();
    int Go();
    void updateScript();
    void loadScript();

    //actions
    void actionAbout();
    void actionHelp();
    void actionUpdate();
    void actionSettings();

private:
    Ui::LatticeMainWindow lattui;
    QString lattExePath;

};

#endif // LATTMAINWINDOW_H
