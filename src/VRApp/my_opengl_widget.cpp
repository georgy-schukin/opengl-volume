#include "my_opengl_widget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer),
    num_of_vertices(0)
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

    const std::vector<QVector3D> cv = {
        // front
        QVector3D(-1.0f, -1.0f,  1.0f),
        QVector3D(1.0f, -1.0f,  1.0f),
        QVector3D(1.0f,  1.0f,  1.0f),
        QVector3D(-1.0f,  1.0f,  1.0f),
        // back
        QVector3D(-1.0f, -1.0f, -1.0f),
        QVector3D(1.0f, -1.0f, -1.0f),
        QVector3D(1.0f,  1.0f, -1.0f),
        QVector3D(-1.0f,  1.0f, -1.0f)
    };

    const std::vector<QVector3D> cc = {
        // front colors
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        // back colors
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f)
    };

    const std::vector<QVector3D> vertices = {
        // front
        cv[0], cv[1], cv[2],
        cv[2], cv[3], cv[0],
        // right
        cv[1], cv[5], cv[6],
        cv[6], cv[2], cv[1],
        // back
        cv[7], cv[6], cv[5],
        cv[5], cv[4], cv[7],
        // left
        cv[4], cv[0], cv[3],
        cv[3], cv[7], cv[4],
        // bottom
        cv[4], cv[5], cv[1],
        cv[1], cv[0], cv[4],
        // top
        cv[3], cv[2], cv[6],
        cv[6], cv[7], cv[3]
    };

    const std::vector<QVector3D> colors = {
        // front
        cc[0], cc[1], cc[2],
        cc[2], cc[3], cc[0],
        // right
        cc[1], cc[5], cc[6],
        cc[6], cc[2], cc[1],
        // back
        cc[7], cc[6], cc[5],
        cc[5], cc[4], cc[7],
        // left
        cc[4], cc[0], cc[3],
        cc[3], cc[7], cc[4],
        // bottom
        cc[4], cc[5], cc[1],
        cc[1], cc[0], cc[4],
        // top
        cc[3], cc[2], cc[6],
        cc[6], cc[7], cc[3]
    };

    const int v_loc = program->attributeLocation("vertex");
    const int c_loc = program->attributeLocation("color");

    vao.create();
    vao.bind();    

    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(&vertices[0], vertices.size()*sizeof(QVector3D));
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);
    vertex_buffer.release();

    color_buffer.create();
    color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    color_buffer.bind();
    color_buffer.allocate(&colors[0], colors.size()*sizeof(QVector3D));
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);
    color_buffer.release();

    /*index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(indices, sizeof(indices));*/
    //index_buffer.release();

    vao.release();
    program->release();

    num_of_vertices = vertices.size();
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
    //const auto count = 12*3;
    //gl->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    gl->glDrawArrays(GL_TRIANGLES, 0, num_of_vertices);
    vao.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    angle += 1.0f;
    update();
}
