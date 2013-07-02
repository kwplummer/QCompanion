#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T15:58:54
#
#-------------------------------------------------

QT       -= gui

TARGET = hourreader
TEMPLATE = lib

DEFINES += HOURREADER_LIBRARY

SOURCES += hourreader.cpp\
    ../speaker.cpp\
    ../component.cpp

HEADERS += hourreader.h\
        hourreader_global.h

QMAKE_CXXFLAGS += -std=c++11
