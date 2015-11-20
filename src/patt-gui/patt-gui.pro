#-------------------------------------------------
#
# Project created by QtCreator 2013-08-13T18:17:04
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = patternGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    shapes.cpp \
    qcompile.cpp \
    calculations.cpp \
    preferences.cpp \
    model.cpp \
    drawprefs.cpp \
    superstructuredialog.cpp \
    drawing.cpp \
    pattern.cpp \
    lattice.cpp \
    latticestructure.cpp \
    matrix.cpp \
    paintdialog.cpp \
    paintrenderer.cpp \
    qtarrowitem.cpp

HEADERS  += mainwindow.hpp \
    config.hpp \
    shapes.h \
    qcompile.h \
    calculations.hpp \
    preferences.hpp \
    model.hpp \
    drawprefs.hpp \
    superstructuredialog.hpp \
    drawing.hpp \
    pattern.hpp \
    lattice.hpp \
    latticestructure.hpp \
    matrix.hpp \
    paintdialog.hpp \
    paintrenderer.hpp \
    qtarrowitem.hpp

FORMS    += mainwindow.ui \
    alignment.ui \
    preferences.ui \
    model.ui \
    drawprefs.ui \
    superstructuredialog.ui

RESOURCES += \
    res/icons.qrc
