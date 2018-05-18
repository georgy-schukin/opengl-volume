#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(60);
}

MyOpenGLWidget::~MyOpenGLWidget() {

}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);

    initView();
    initProgram();
}

void MyOpenGLWidget::initProgram() {
    program = std::make_shared<QOpenGLShaderProgram>();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/basic.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/basic.frag");
    program->link();
    program->bind();

    const GLfloat vertices[] = {
      // front
     -1.0f, -1.0f,  1.0f,
      1.0f, -1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,
     -1.0f,  1.0f,  1.0f,
      // back
     -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f,  1.0f, -1.0f,
     -1.0f,  1.0f, -1.0f
    };

    const GLfloat colors[] = {
        // front colors
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // back colors
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f
    };

    const GLuint indices[] = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    const int v_loc = program->attributeLocation("vertex");
    const int c_loc = program->attributeLocation("color");

    vao.create();
    vao.bind();    

    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(vertices, sizeof(vertices));    
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);
    vertex_buffer.release();

    color_buffer.create();
    color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    color_buffer.bind();
    color_buffer.allocate(colors, sizeof(colors));
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);
    color_buffer.release();

    index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(indices, sizeof(indices));
    //index_buffer.release();

    vao.release();
    program->release();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initView();
}

void MyOpenGLWidget::initView() {
    model.setToIdentity();
    view.setToIdentity();    
    view.lookAt(QVector3D(3.0f, 3.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    projection.setToIdentity();
    const auto aspect = float(width())/height();
    projection.perspective(45.0f, aspect, 0.01f, 100.0f);
}

void MyOpenGLWidget::paintGL() {
    auto *gl = context()->functions();

    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program) {
        return;
    }

    program->bind();

    const int mvp_loc = program->uniformLocation("MVP");
    QMatrix4x4 rotate;
    rotate.setToIdentity();
    rotate.rotate(angle, QVector3D(0.0f, 1.0f, 0.0f));
    program->setUniformValue(mvp_loc, projection*view*rotate*model);

    vao.bind();
    const auto count = 12*3;    
    gl->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    vao.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    angle += 1.0f;
    update();
}
