#include "mainwindow.hpp"
#include <QApplication>

void setupAppInfo(QApplication &app) {
    app.setApplicationName(__APPNAME__);
    app.setApplicationVersion(__APPVERSION__);
    app.setOrganizationName("Liam Deacon");
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setupAppInfo(a);
    MainWindow w;
    w.show();
    
    return a.exec();
}
