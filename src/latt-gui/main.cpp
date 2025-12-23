#include "lattmainwindow.h"

// cppcheck-suppress missingIncludeSystem
#include <QtGlobal>

#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif
#include <QtCore/QProcess>
#include <stdio.h>

void setupAppInfo(QApplication &app) {
    app.setApplicationName(__APPNAME__);
    app.setApplicationVersion(__APPVERSION__);
    app.setOrganizationName("Liam Deacon");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setupAppInfo(app);
    lattMainWindow win;
    win.show();

    return app.exec();
}
