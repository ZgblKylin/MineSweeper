#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T19:13:09
#
#-------------------------------------------------

QT += core widgets
CONFIG += c++14

TARGET = MineSweeper
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    MineField.cpp \
    MineSweeper.cpp \
    CustomDialog.cpp \
    Tile.cpp

HEADERS += \
    MainWindow.h \
    MineField.h \
    MineSweeper.h \
    CustomDialog.h \
    Tile.h

FORMS += MainWindow.ui \
    CustomDialog.ui

RESOURCES += \
    Resources.qrc
