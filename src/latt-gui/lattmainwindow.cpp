#include "lattmainwindow.h"
#include "ui_lattmainwindow.h"
#include "qcompile.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QProcess>
#include <QDebug>

/* Lattic GUI constructor */
lattMainWindow::lattMainWindow(QWidget *parent) : QMainWindow(parent) {
    lattui.setupUi(this);
    advancedText = "#";
    lattExePath.clear();

    //form control signals
    connect(lattui.checkBox_UseInputFile, SIGNAL(toggled(bool)), this, SLOT(checkInput()));
    connect(lattui.checkBox_Advanced, SIGNAL(toggled(bool)), this, SLOT(checkAdvanced()));
    connect(lattui.btn_inpGetFile, SIGNAL(pressed()), this, SLOT(pressedInputBtn()));
    connect(lattui.btn_inpAdvanced, SIGNAL(pressed()), this, SLOT(pressedAdvancedBtn()));
    connect(lattui.btn_output, SIGNAL(pressed()), this, SLOT(pressedOutputBtn()));
    connect(lattui.btn_Generate, SIGNAL(pressed()), this, SLOT(Go()));
    connect(lattui.lineInput, SIGNAL(textEdited(QString)), this, SLOT(updateInput()));
    connect(lattui.lineOutput, SIGNAL(textEdited(QString)), this, SLOT(updateOutput()));
    connect(lattui.checkBox_d, SIGNAL(toggled(bool)), this, SLOT(checkLimitUC()));
    connect(lattui.buttonBox_Script, SIGNAL(clicked(QAbstractButton*)),this, SLOT(updateScript()));
    connect(lattui.btn_scriptFile, SIGNAL(clicked()), this, SLOT(loadScript()));

    //menu action signals
    connect(lattui.actionAbout, SIGNAL(triggered()), this, SLOT(actionAbout()));
    connect(lattui.action_Documentation, SIGNAL(triggered()), this, SLOT(actionHelp()));
    connect(lattui.action_Generate, SIGNAL(triggered()), this, SLOT(Go()));
    connect(lattui.action_Import, SIGNAL(triggered()), this, SLOT(pressedInputBtn()));
    connect(lattui.action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(lattui.action_Settings, SIGNAL(triggered()), this, SLOT(actionSettings()));
    connect(lattui.action_Update, SIGNAL(triggered()), this, SLOT(actionUpdate()));

    //initialise GUI (anything else)
    initGui();
}

lattMainWindow::~lattMainWindow()
{
    saveSettings();
    //delete lattui;
}

void lattMainWindow::checkInput() {
    //update UI after user pressed checkbox
    bool b =  lattui.checkBox_UseInputFile->isChecked();
    lattui.lblInput->setEnabled(b);
    lattui.lineInput->setEnabled(b);
    lattui.groupCrystal->setDisabled(b);
    lattui.group_hkl->setDisabled(b);
}

void lattMainWindow::checkAdvanced() {
    // show advanced dialog
    lattui.btn_inpAdvanced->setEnabled(lattui.checkBox_Advanced->isChecked());
    QString title = LATTWINDOWTITLE;
    if (!lattui.checkBox_Advanced->isChecked()) {
        //setMaximumHeight();
        setMaximumWidth(lattui.groupFileIO->width()+20);
        resize(maximumWidth(), height());
        setWindowTitle(title);
    } else {
        //setMaximumHeight();
        setMaximumWidth(lattui.groupFileIO->width()+20 + lattui.textEdit_Script->width() + 20);
        resize(maximumWidth(), height());
        setWindowTitle(title.append(" (Advanced)"));
    }
}

void lattMainWindow::checkLimitUC() {
    // allow update of limit spinbox
    lattui.doubleSpinBox_d->setEnabled(lattui.checkBox_d->isChecked());
}

int lattMainWindow::Go() {
    QString command;
    QStringList args;

    /* arguments grabbed from ui */
    /* usage: latt
            -h <h> \t-k <k> \t-l <l>
            -a <latt const. a>
            -c <latt const. c>
            -n <name of atom>
            -d <max displacement: d*a>
            -t <lattice type: fcc, bcc, hcp, dia>
            -i <input of basis vectors>
            -o <output>
     */

    QIcon winIcon(":/lattice.ico");
    QMessageBox msgBox;
    QString infoString = "Do you want to save your changes?";
    QFileInfo fi(lattExePath);
    if (!fi.exists()) {

        // Path not found or not executable
        // ask user for path to latt
        QFileDialog dialog;
        QString path;
        while (!fi.isExecutable()) {
            if (!fi.exists()) {
               path = QCoreApplication::applicationDirPath();
            } else if (!fi.isDir()) {
                path = fi.absoluteDir().path();
            } else {
                path = lattExePath;
            }
            lattExePath = dialog.getOpenFileName(0,
                                                 "Specify path to latt executable...",
                                                 path,
                                                 "All Files (*);; Exe Files (*.exe)");
            fi.setFile(lattExePath);
            path = fi.absoluteDir().path();
        }

        // ask user to keep path
        infoString.append("\n\n(It can always be manually changed \nlater in the ").append(
                    QCoreApplication::applicationName()).append(" .ini file)\n");
        msgBox.setText("Lattice executable path has been modified");
        msgBox.setInformativeText(infoString);
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setIconPixmap(QPixmap(":/dialog-question.png"));
        msgBox.setWindowIcon(winIcon);
        msgBox.setWindowTitle(QCoreApplication::applicationName());
        int ret = msgBox.exec();

        // save if user wanted it so
        if (ret == QMessageBox::Save) {
            QSettings settings(QCoreApplication::organizationName(),QCoreApplication::applicationName());
            settings.setFallbacksEnabled(FALLBACK_SETTING);
            settings.setDefaultFormat(QSettings::IniFormat);
            settings.beginGroup("PATHS");
            settings.setValue("Latt_exe",lattExePath);
            settings.endGroup();
        }
    }

    if (lattui.checkBox_UseInputFile->isChecked()) {
        //use input file only (plus d)
        QFileInfo inp(lattui.lineInput->text());
        if (!inp.exists()) {
            QString info = "Please check input filename and try again";
            info.append("\n(or use user specified settings to generate output)");
            msgBox.setText("Error: Input file does not exist!");
            msgBox.setInformativeText(info);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIconPixmap(QPixmap(":/dialog-question.png"));
            msgBox.setWindowIcon(winIcon);
            msgBox.setWindowTitle(QCoreApplication::applicationName());
            msgBox.exec();
            return -1; //input path doesn't exist
        }
        QString input = lattui.lineInput->text();
        if (input.contains(" ") && !input.startsWith("\"")) {
            input.prepend("\"");
        }
        if (input.contains(" ") && !input.endsWith("\"")) {
            input.append("\"");
        }
        args << "-i" << input;
    } else {
        // use user specified controls
        args << "-h" << QString::number(lattui.dblspin_h->value());
        args << "-k" << QString::number(lattui.dblspin_k->value());
        args << "-l" << QString::number(lattui.dblspin_l->value());

        args << "-a" << QString::number(lattui.doubleSpinBox_a->value());
        args << "-c" << QString::number(lattui.doubleSpinBox_c->value());

        if (lattui.checkBox_d->isChecked()) {
            args << "-d" << QString::number(lattui.doubleSpinBox_d->value());
        }

        args << "-t" << lattui.comboPacking->currentText();
        args << "-n" << lattui.lineModel->text().replace(" ", "_");

        QString output = lattui.lineOutput->text();
        QDir of(output);
        QFileInfo out(of.path());
        QFileInfo outDir(out.path());
        qDebug() << "Raw output string: " << output;
        qDebug() << "Output directory: " << out.path();
        if (!outDir.exists()) {
            QString info = "Please check output filename and try again\n";
            msgBox.setText("Error: Output directory does not exist!");
            info.append(out.path());
            msgBox.setInformativeText(info);
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.setIconPixmap(QPixmap(":/dialog-question.png"));
            msgBox.setWindowIcon(winIcon);
            msgBox.setWindowTitle(QCoreApplication::applicationName());
            msgBox.exec();
            return -2; //input path doesn't exist
        }

        // manage output
        if (!output.startsWith("\"") && output.contains(" ")) {
            output.prepend("\"");
        }

        if (!output.endsWith("\"") && output.contains(" ")) {
            output.append("\"");
        }

        args << "-o" << output;
    //    args << "&& pause";

    }

    // add script
    if (lattui.checkBox_Advanced->isChecked()) {
        QString script = advancedText;
//        QString keyword = "jmolscript";
        if (!script.isEmpty() && !script.isNull()) {
            if (script.startsWith("\"")) {
                if (!script.startsWith("\"jmolscript:")) {
                    script.prepend("\"jmolscript:");
                }
            } else {
                if (!script.startsWith("jmolscript:")) {
                    script.prepend("\"jmolscript:");
                } else {
                    script.prepend("\"");
                }
            }
            if (!script.endsWith("\"")) {
                script.append("\"");
            }
            args << "-s" << script;
        }
    }

    command = lattExePath;
    if (!command.startsWith("\"") && command.contains(" ")) {
        command.prepend("\"");
    }
    if (!command.endsWith("\"") && command.contains(" ")) {
        command.append("\"");
    }
    qDebug() << "Command: " << command << args;

    QProcess latt;
    latt.startDetached(command, args);
    latt.waitForFinished(-1);
    QString stdOut = latt.readAllStandardOutput();
    QString stdErr = latt.readAllStandardError();
    if (!stdOut.isEmpty() && !stdOut.isNull()) {
        qDebug() << "stdout: " << stdOut;
    }
    if (!stdErr.isEmpty() && !stdErr.isNull()) {
        // error occurred
        qDebug() << "Error calling latt:\n*** " << stdErr << "\n";
        return -3; //error during latt execution
    }

    return 0;
}

void lattMainWindow::initGui() {
    loadSettings();
    checkInput();
    checkAdvanced();
    checkLimitUC();
}

void lattMainWindow::loadSettings() {
    QSettings settings(QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());
    settings.setFallbacksEnabled(FALLBACK_SETTING);
    settings.setDefaultFormat(QSettings::IniFormat);

    settings.beginGroup("MAIN_WINDOW");
    restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    restoreState(settings.value("mainWindowState").toByteArray());
    settings.endGroup();

    settings.beginGroup("EXTERNAL_PATHS");
    lattExePath = settings.value("lattExePath", lattExePath).toString();
    settings.endGroup();

    settings.beginGroup("USER_DEFINED");
    lattui.checkBox_Advanced->setChecked(settings.value("chkAdvanced", lattui.checkBox_Advanced->isChecked()).toBool());
    lattui.checkBox_d->setChecked(settings.value("chkd", lattui.checkBox_d->isChecked()).toBool());
    lattui.checkBox_UseInputFile->setChecked(settings.value("chkUseInput", lattui.checkBox_UseInputFile->isChecked()).toBool());
    lattui.comboPacking->setCurrentIndex(settings.value("packing", lattui.comboPacking->currentIndex()).toInt());
    lattui.dblspin_h->setValue(settings.value("h", lattui.dblspin_h->value()).toDouble());
    lattui.dblspin_k->setValue(settings.value("k", lattui.dblspin_k->value()).toDouble());
    lattui.dblspin_l->setValue(settings.value("l", lattui.dblspin_l->value()).toDouble());
    lattui.doubleSpinBox_a->setValue(settings.value("a", lattui.doubleSpinBox_a->value()).toDouble());
    lattui.doubleSpinBox_c->setValue(settings.value("c", lattui.doubleSpinBox_c->value()).toDouble());
    lattui.doubleSpinBox_d->setValue(settings.value("d", lattui.doubleSpinBox_d->value()).toDouble());
    lattui.lineInput->setText(settings.value("inputFile", lattui.lineInput->text()).toString());
    lattui.lineOutput->setText(settings.value("outputFile", lattui.lineOutput->text()).toString());
    lattui.lineModel->setText(settings.value("model", lattui.lineModel->text()).toString());
    advancedText = settings.value("jmolScript", advancedText).toString();
    settings.endGroup();

}

void lattMainWindow::saveSettings() {
    QSettings settings(QCoreApplication::organizationName(),QCoreApplication::applicationName());
    settings.setFallbacksEnabled(FALLBACK_SETTING);
    settings.setDefaultFormat(QSettings::IniFormat);

    settings.beginGroup("MAIN_WINDOW");
    settings.setValue("mainWindowGeometry", saveGeometry());
    settings.setValue("mainWindowState", saveState());
    settings.endGroup();

    // leave for user dialog
    /*
    settings.beginGroup("EXTERNAL_PATHS");
    settings.setValue("lattExePath",lattExePath);
    settings.endGroup();
    */

    settings.beginGroup("USER_DEFINED");
    settings.setValue("chkAdvanced", lattui.checkBox_Advanced->isChecked());
    settings.setValue("chkd", lattui.checkBox_d->isChecked());
    settings.setValue("chkUseInput", lattui.checkBox_UseInputFile->isChecked());
    settings.setValue("packing", lattui.comboPacking->currentIndex());
    settings.setValue("h", lattui.dblspin_h->value());
    settings.setValue("k", lattui.dblspin_k->value());
    settings.setValue("l", lattui.dblspin_l->value());
    settings.setValue("a", lattui.doubleSpinBox_a->value());
    settings.setValue("c", lattui.doubleSpinBox_c->value());
    settings.setValue("d", lattui.doubleSpinBox_d->value());
    settings.setValue("inputFile", lattui.lineInput->text());
    settings.setValue("outputFile", lattui.lineOutput->text());
    settings.setValue("model", lattui.lineModel->text());
    settings.setValue("jmolScript", advancedText);
    settings.endGroup();

}

QString lattMainWindow::AdvancedScript(QString rawScript) {
    return rawScript;
}

void lattMainWindow::addScript(QString script, QString filePath) {
    //!TODO
    script = filePath;
    filePath = script;
    
}


void lattMainWindow::updateH() {

}

void lattMainWindow::updateK() {

}

void lattMainWindow::updateL() {

}

void lattMainWindow::updateA() {

}

void lattMainWindow::updateC() {

}

void lattMainWindow::updateInputMethod() {

}

void lattMainWindow::updateStructure() {

}

void lattMainWindow::updateModel() {

}

void lattMainWindow::updateInput() {
    QFileInfo fi(lattui.lineInput->text());
    if (!fi.isFile()) {
        lattui.statusBar->showMessage("Invalid input file!");
    } else {
        lattui.statusBar->showMessage("");
    }
}

void lattMainWindow::updateOutput() {
    QFileInfo fi(lattui.lineInput->text());
    if (!fi.dir().exists()) {
        lattui.lineOutput->setStatusTip("Invalid output directory!");
    }
}

QString lattMainWindow::AdvancedXYZ() {
    return "#";
}

QString lattMainWindow::pressedAdvancedBtn() {
    QIcon winIcon(":/lattice.ico");
    QMessageBox msgBox;
    QString info = "Do you want to save your changes?";
    QFileInfo fi(lattExePath);
    QString path;
    QFileDialog dialog;

    if (!fi.exists()) {
       path = QCoreApplication::applicationDirPath();
    } else if (!fi.isDir()) {
        path = fi.absoluteDir().path();
    } else {
        path = lattExePath;
    }
    lattExePath = dialog.getOpenFileName(0, "Specify path to latt executable...",
                                         path, "All Files (*);; Exe Files (*.exe)");
    fi.setFile(lattExePath);
    path = fi.absoluteDir().path();

    // ask user to keep path
    info.append("\n\n(It can always be manually changed \nlater in the ").append(
                QCoreApplication::applicationName()).append(" .ini file)\n");
    msgBox.setText("Lattice executable path has been modified");
    msgBox.setInformativeText(info);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Ignore);
    msgBox.setDefaultButton(QMessageBox::Save);
    msgBox.setIconPixmap(QPixmap(":/dialog-question.png"));
    msgBox.setWindowIcon(winIcon);
    msgBox.setWindowTitle(QCoreApplication::applicationName());
    int ret = msgBox.exec();

    // save if user wanted it so
    if (ret == QMessageBox::Save) {
        QSettings settings(QCoreApplication::organizationName(),QCoreApplication::applicationName());
        settings.setFallbacksEnabled(FALLBACK_SETTING);
        settings.setDefaultFormat(QSettings::IniFormat);
        settings.beginGroup("PATHS");
        settings.setValue("Latt_exe",lattExePath);
        settings.endGroup();
    }
    return "#";
}

QString lattMainWindow::pressedInputBtn() {
    QFileDialog dialog;
    QFileInfo fi(lattui.lineInput->text());
    QDir dir(fi.canonicalFilePath());
    if (!dir.exists()) {
        dir.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    }
    QString ret = dialog.getOpenFileName(0, "Select Input File...", dir.canonicalPath(), "Lattice Input Files (*.latt *.lat *.inp *.bul)");
    //dialog.exec()
    lattui.lineInput->setText(ret);
    return "#";
}

QString lattMainWindow::pressedOutputBtn() {
    QFileDialog dialog;
    QFileInfo fi(lattui.lineOutput->text());
    QDir dir(fi.canonicalFilePath());
    if (!dir.exists()) {
        dir.setPath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    }
    QString ret = dialog.getSaveFileName(0, "Save As...", dir.canonicalPath(), "xyz File (*.xyz)");
    //dialog.exec()
    lattui.lineOutput->setText(ret);
    return "#";
}

void lattMainWindow::closeEvent(QCloseEvent *event) {
    //saveSettings();
    event->accept();
}

/* update script in memory */
void lattMainWindow::updateScript() {

    QString script = lattui.textEdit_Script->toPlainText();
    QStringList scriptLines = script.split("\n");
    QString line;
    script.clear();
    foreach(line, scriptLines) {
        if (!line.isEmpty() && !line.startsWith("#")) {
            while (line.contains("#")) {
                line.chop(1);
            }
            line.remove("\n");
            line.append(";");
            script.append(line);
        }
    }
    advancedText = script;
    qDebug() << "jmolscript: " << advancedText;
}

/* Load script from file */
void lattMainWindow::loadScript() {

}

/********************************************************
 *                      ACTIONS                         *
 ********************************************************/


/* show about widget */
void lattMainWindow::actionAbout() {
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
    description.append("by ").append(__USER__).append("@").append(__HOST__).append(" \n");
    description.append("at ").append(__TIME__).append(" on ").append(__DATE__).append("\n");
    description.append("using ");
    description.append("");
    description.append(" for ").append(__OS__);
    description.append("\n(Qt Version ").append(QT_VERSION_STR);
    QString depends = __DEPENDS__;
    if (!depends.isEmpty() && !depends.isNull()) {
        description.append("\n").append(depends);
    }
    description.append(")");
    QMessageBox about;
    QIcon winIcon(":/help-contents.png");
    QPixmap pixmap(":lattice.png");
    about.setIconPixmap(pixmap);
    about.setWindowIcon(winIcon);
    about.setText(description);
    about.setWindowTitle(title);
    about.exec();
}

/* handle help - maybe html? */
void lattMainWindow::actionHelp() {
    QMessageBox msg;
    msg.setWindowTitle("Help");
    msg.setText("There is no help at this time - sorry...");
    msg.exec();
}

void lattMainWindow::actionUpdate() {

}

void lattMainWindow::actionSettings() {

}
