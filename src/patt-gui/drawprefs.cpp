#include "drawprefs.hpp"
#include "ui_drawprefs.h"

DrawPrefs::DrawPrefs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawPrefs)
{
    ui->setupUi(this);
}

DrawPrefs::~DrawPrefs()
{
    delete ui;
}
