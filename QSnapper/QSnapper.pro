#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T18:00:00
#
#-------------------------------------------------

QT       -= gui

TARGET = qsnapper

TEMPLATE = lib

SOURCES += qsnapper.cpp\
    ../speaker.cpp\
    ../component.cpp

QMAKE_CXXFLAGS += -std=c++11

HEADERS += qsnapper.h

INCLUDEPATH += /usr/include/glib-2.0/glib
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/include/glib-2.0/gobject
