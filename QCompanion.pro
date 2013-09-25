#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T13:34:50
#
#-------------------------------------------------

QT = core gui dbus
UNIX {
    QT += dbus
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCompanion
TEMPLATE = app

#Enable this to get .clang_complete for vim. It is put next to the executable, so must be moved.
QMAKE_CXX = ~/Code/clang_complete/bin/cc_args.py g++

LIBS += -ltbb -lnotify -lflite_cmu_us_kal -lflite_usenglish -lflite_cmulex -lflite

INCLUDEPATH += /usr/include/glib-2.0/glib
INCLUDEPATH += /usr/include/glib-2.0/
INCLUDEPATH += /usr/include/glib-2.0/gobject

SOURCES += main.cpp\
        qcompanion.cpp \
    UnitTests.cpp \
    component.cpp \
    speaker.cpp \
    hourreader.cpp \
    qsnapper.cpp \
    waiterdialog.cpp \
    waiterwidget.cpp \
    waitercomponent.cpp \
    qlipperwidget.cpp \
    qlippercomponent.cpp \
    waitercrondialog.cpp \
    waitercronoccurance.cpp

HEADERS  += qcompanion.h \
    component.h \
    speaker.h \
    hourreader.h \
    qsnapper.h \
    waiterdialog.h \
    waiterwidget.h \
    waitercomponent.h \
    qlipperwidget.h \
    qlippercomponent.h \
    waitercrondialog.h \
    waitercronoccurance.h

FORMS    += qcompanion.ui \
    waiterdialog.ui \
    qlipperwidget.ui \
    waitercrondialog.ui

QMAKE_CXXFLAGS += -std=c++11

RESOURCES += \
    icons.qrc

CONFIG(DEBUG)
{
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    LIBS += -lgcov
    DEFINES += DBG
}
