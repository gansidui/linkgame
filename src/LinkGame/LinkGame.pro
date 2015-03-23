#-------------------------------------------------
#
# Project created by QtCreator 2013-12-18T14:37:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LinkGame
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    LinkGame.cpp

HEADERS  += mainwindow.h \
    LinkGame.h

FORMS    += mainwindow.ui
