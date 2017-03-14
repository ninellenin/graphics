#-------------------------------------------------
#
# Project created by QtCreator 2015-03-20T19:44:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plotter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    drawwidget.cpp \
    ovalofCassini.cpp \
    ../Common/controller.cpp \
    ../Common/xmlparser.cpp \
    ../Common/myexception.cpp

HEADERS  += mainwindow.h \
    drawwidget.h \
    ovalofCassini.h\
    controller.h \
    ../Common/controller.h \
    ../Common/xmlparser.h \
    ../Common/myexception.h
