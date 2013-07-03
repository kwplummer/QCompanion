#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T13:34:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCompanion
TEMPLATE = app

LIBS += -ltbb -lnotify

INCLUDEPATH += "/usr/include/glib-2.0/"

SOURCES += main.cpp\
        qcompanion.cpp \
    UnitTests.cpp \
    component.cpp \
    speaker.cpp

HEADERS  += qcompanion.h \
    component.h \
    speaker.h

FORMS    += qcompanion.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    icons.qrc
