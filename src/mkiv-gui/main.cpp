

#include "ui_MainWindow.h"
#include <QApplication> 
 
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(res/res);
    
    QApplication app(argc, argv);
    QDialog *dialog = new QDialog;
    Ui::Dialog ui;
    ui.setupUi(dialog);

    dialog->show();
    return app.exec();
}

