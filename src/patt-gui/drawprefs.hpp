#ifndef DRAWPREFS_HPP
#define DRAWPREFS_HPP

#include <QWidget>

namespace Ui {
class DrawPrefs;
}

class DrawPrefs : public QWidget
{
    Q_OBJECT
    
public:
    explicit DrawPrefs(QWidget *parent = 0);
    ~DrawPrefs();
    
private:
    Ui::DrawPrefs *ui;
};

#endif // DRAWPREFS_HPP
