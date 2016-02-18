#-------------------------------------------------
#
# Project created by QtCreator 2016-02-11T14:33:28
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = Sheduller
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app


SOURCES += main.cpp \
    MyScheduller.cpp \
    CronParser.cpp \
    Timer.cpp

HEADERS += \
    MyScheduller.h \
    CronParser.h \
    Timer.h
