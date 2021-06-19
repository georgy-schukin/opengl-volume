TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

TARGET = VolumeConvert
DESTDIR = $$PWD

SOURCES += \
        main.cpp

HEADERS += \
    ../common/types.h
