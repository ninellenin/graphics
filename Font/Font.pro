#-------------------------------------------------
#
# Project created by QtCreator 2015-05-22T02:58:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Font
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    drawwidget.cpp \
    font.cpp \
    xmlparser.cpp \
    ../Common/myexception.cpp \
    ../Common/controller.cpp

HEADERS  += mainwindow.h \
    drawwidget.h \
    font.h \
    xmlparser.h \
    ../Common/myexception.h \
    ../Common/controller.h

