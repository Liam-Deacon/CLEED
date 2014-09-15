#include "preferences.hpp"
#include "ui_preferences.h"

preferences::preferences(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::preferences)
{
    ui->setupUi(this);
}

preferences::~preferences()
{
    delete ui;
}
