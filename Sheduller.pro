#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T14:33:28
#
#-------------------------------------------------

QT       -= gui
QT       += testlib

TARGET   = Sheduller
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app


SOURCES += main.cpp \
    MyScheduller.cpp \
    CronParser.cpp \
    Timer.cpp \
    ../tests/test.cpp

HEADERS += \
    MyScheduller.h \
    CronParser.h \
    Timer.h \
    ../tests/test.h
