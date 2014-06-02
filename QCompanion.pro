#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T13:34:50
#
#-------------------------------------------------

QT = core gui
unix {
    QT += dbus
    LIBS += -ltbb -lflite_cmu_us_kal -lflite_usenglish -lflite_cmulex -lflite
    SOURCES += dbusadaptor.cpp
    HEADERS += dbusadaptor.h
}

win32 {
    LIBS += -L$$PWD/lib -ltbb -lole32
    INCLUDEPATH += ./include
}

DEFINES += QT_NO_KEYWORDS

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCompanion
TEMPLATE = app

QMAKE_CXXFLAGS_RELEASE += -g

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

RESOURCES += icons.qrc

CONFIG(DEBUG)
{
#    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
#    LIBS += -lgcov
    DEFINES += DBG
}
