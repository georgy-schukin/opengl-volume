#-------------------------------------------------
#
# Project created by QtCreator 2017-07-31T11:52:51
#
#-------------------------------------------------

QT += core gui
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VRApp
TEMPLATE = app

DESTDIR = $$PWD

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    cutoff_dialog.cpp \
        mainwindow.cpp \
    my_opengl_widget.cpp \
    objects/cube.cpp \
    objects/hemisphere.cpp \
    objects/plane.cpp \
    objects/triangulated_shape.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    cutoff_dialog.h \
    my_opengl_widget.h \
    objects/buffer.h \
    objects/cube.h \
    objects/hemisphere.h \
    objects/plane.h \
    objects/shape.h \
    objects/triangulated_shape.h \
    util.h \
    frame3d.h

FORMS    += mainwindow.ui \
    cutoff_dialog.ui

DISTFILES += \
    shaders/basic.vert \
    shaders/basic.frag \
    shaders/plane.frag \
    shaders/plane.vert \
    shaders/texture.frag \
    shaders/texture.vert
