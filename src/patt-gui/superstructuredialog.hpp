#ifndef SUPERSTRUCTUREDIALOG_HPP
#define SUPERSTRUCTUREDIALOG_HPP

#include <QDialog>
#include <QCloseEvent>
#include "model.hpp"

namespace Ui {
class SuperstructureDialog;
}

class SuperstructureDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SuperstructureDialog(QWidget *parent = 0);
    ~SuperstructureDialog();
    
private:
    Ui::SuperstructureDialog *ui;

private slots:
    void clearAll();
    void addRotation();
    void deleteRotation();
    void toggleCommensurate();
    void unityMatrix();
    void updateMatrix();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SUPERSTRUCTUREDIALOG_HPP
