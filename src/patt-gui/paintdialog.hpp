#ifndef PAINTDIALOG_HPP
#define PAINTDIALOG_HPP

#include "paintrenderer.hpp"
#include <QDialog>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QGridLayout>
#include <QLayout>

class RenderArea;

class PaintDialog : public QDialog
{
    Q_OBJECT

public:
    PaintDialog();
    ~PaintDialog();

private slots:
    void shapeChanged();
    void penChanged();
    void brushChanged();

private:
    PaintRenderer *renderArea;
    QLabel *shapeLabel;
    QLabel *penWidthLabel;
    QLabel *penStyleLabel;
    QLabel *penCapLabel;
    QLabel *penJoinLabel;
    QLabel *brushStyleLabel;
    QLabel *otherOptionsLabel;
    QLabel *penColorLabel;
    QLabel *brushColorLabel;
    QComboBox *shapeComboBox;
    QSpinBox *penWidthSpinBox;
    QComboBox *penStyleComboBox;
    QComboBox *penCapComboBox;
    QComboBox *penJoinComboBox;
    QComboBox *brushStyleComboBox;
    QCheckBox *antialiasingCheckBox;
    QCheckBox *transformationsCheckBox;
    QComboBox *penColorComboBox;
    QComboBox *brushColorComboBox;
};

#endif // PAINTDIALOG_HPP
