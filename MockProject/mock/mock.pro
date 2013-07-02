#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T15:44:04
#
#-------------------------------------------------

QT       -= gui

TARGET = mock
TEMPLATE = lib

DEFINES += MOCK_LIBRARY

SOURCES += mock.cpp\
    ../../speaker.cpp\
    ../../component.cpp

HEADERS += mock.h\
        mock_global.h
QMAKE_CXXFLAGS += -std=c++11
