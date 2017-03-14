#-------------------------------------------------
#
# Project created by QtCreator 2015-02-19T11:12:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = circle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    circle.cpp \
    drawwidget.cpp \
    controller.cpp \
    xmlparser.cpp \
    myexception.cpp

HEADERS  += mainwindow.h \
    circle.h \
    drawwidget.h \
    controller.h \
    xmlparser.h \
    myexception.h
