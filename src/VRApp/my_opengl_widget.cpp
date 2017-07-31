#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    setFormat(format);
}

MyOpenGLWidget::~MyOpenGLWidget() {

}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();
    gl->glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
}

void MyOpenGLWidget::resizeGL(int width, int height) {

}

void MyOpenGLWidget::paintGL() {

}
