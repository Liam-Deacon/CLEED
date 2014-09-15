#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <QDialog>

namespace Ui {
class preferences;
}

class preferences : public QDialog
{
    Q_OBJECT

public:
    explicit preferences(QDialog *parent = 0);
    ~preferences();
    
private:
    Ui::preferences *ui;
};

#endif // PREFERENCES_HPP
