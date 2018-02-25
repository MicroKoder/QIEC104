#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T06:11:36
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += network

TARGET = IEC104NA
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    iec104driver.cpp \
    csetting.cpp \
    settingsdialog.cpp \
    ctools.cpp \
    tablemodel.cpp \
    cp56time.cpp \
    ciecsignal.cpp \
    importdialog.cpp

HEADERS += \
        mainwindow.h \
    iec104driver.h \
    csetting.h \
    settingsdialog.h \
    ctools.h \
    tablemodel.h \
    cp56time.h \
    ciecsignal.h \
    importdialog.h

FORMS += \
        mainwindow.ui \
    settingsdialog.ui \
    importdialog.ui

RESOURCES += \
    resources.qrc
