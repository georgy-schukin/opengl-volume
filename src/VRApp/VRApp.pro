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
    cube/cube_data.cpp \
    cube/cube_util.cpp \
    cutoff_dialog.cpp \
    frame_loader.cpp \
    frame_util.cpp \
    main_window.cpp \
    my_opengl_widget.cpp \
    objects/cube.cpp \
    objects/hemisphere.cpp \
    objects/plane.cpp \
    objects/triangulated_shape.cpp \
    palette_util.cpp \
    render/ray_cast_renderer.cpp \
    render/renderer.cpp \
    render/slice_renderer.cpp

HEADERS  += \
    ../common/types.h \
    cube/cube_data.h \
    cube/cube_util.h \
    cutoff_dialog.h \
    frame_loader.h \
    frame_util.h \
    main_window.h \
    my_opengl_widget.h \
    objects/buffer.h \
    objects/cube.h \
    objects/hemisphere.h \
    objects/plane.h \
    objects/shape.h \
    objects/triangulated_shape.h \
    palette_util.h \
    frame3d.h \
    render/ray_cast_renderer.h \
    render/renderer.h \
    render/slice_renderer.h

FORMS    += \
    cutoff_dialog.ui \
    main_window.ui

DISTFILES += \
    shaders/basic.vert \
    shaders/basic.frag \
    shaders/plane.frag \
    shaders/plane.vert \
    shaders/raycast.frag \
    shaders/raycast.vert \
    shaders/texture.frag \
    shaders/texture.vert
