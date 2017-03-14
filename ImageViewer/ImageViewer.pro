#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T19:16:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    drawwidget.cpp \
    image.cpp \
    ../Common/controller.cpp \
    ../Common/myexception.cpp \
    ../Common/doublecontroller.cpp

HEADERS  += mainwindow.h \
    drawwidget.h \
    image.h \
    ../Common/controller.h \
    ../Common/myexception.h \
    ../Common/doublecontroller.h
