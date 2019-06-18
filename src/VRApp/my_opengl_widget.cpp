#include "my_opengl_widget.h"
#include "frame3d.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include <cmath>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    tex_coord_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer),
    texture_3d(QOpenGLTexture::Target3D),
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

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_MULTISAMPLE);
    gl->glEnable(GL_TEXTURE);
    gl->glEnable(GL_TEXTURE_3D);

    initView();
    initProgram();
    initTextures();
}

void MyOpenGLWidget::initProgram() {
    program = std::make_shared<QOpenGLShaderProgram>();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/texture.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/texture.frag");
    program->link();
    program->bind();

    // Vertex coordinates.
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

    // Vertex texture coordinates.
    const std::vector<QVector3D> ct = {
        // front
        QVector3D(0.0f, 0.0f,  1.0f),
        QVector3D(1.0f, 0.0f,  1.0f),
        QVector3D(1.0f, 1.0f,  1.0f),
        QVector3D(0.0f, 1.0f,  1.0f),
        // back
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f)
    };

    // Vertex colors.
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

    const std::vector<size_t> indices = {
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

    std::vector<QVector3D> vertices, tex_coords, colors;

    for (const auto &idx: indices) {
        vertices.push_back(cv[idx]);
        tex_coords.push_back(ct[idx]);
        colors.push_back(cv[idx]);
    }

    const int v_loc = program->attributeLocation("vertex");
    const int c_loc = program->attributeLocation("color");
    const int tc_loc = program->attributeLocation("tCoord");

    vao.create();
    vao.bind();

    vertex_buffer.create();
    vertex_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertex_buffer.bind();
    vertex_buffer.allocate(&vertices[0], vertices.size()*sizeof(QVector3D));
    program->enableAttributeArray(v_loc);
    program->setAttributeBuffer(v_loc, GL_FLOAT, 0, 3);
    vertex_buffer.release();

    /*color_buffer.create();
    color_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    color_buffer.bind();
    color_buffer.allocate(&colors[0], colors.size()*sizeof(QVector3D));
    program->enableAttributeArray(c_loc);
    program->setAttributeBuffer(c_loc, GL_FLOAT, 0, 3);
    color_buffer.release();*/

    tex_coord_buffer.create();
    tex_coord_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    tex_coord_buffer.bind();
    tex_coord_buffer.allocate(&tex_coords[0], tex_coords.size()*sizeof(QVector3D));
    program->enableAttributeArray(tc_loc);
    program->setAttributeBuffer(tc_loc, GL_FLOAT, 0, 3);
    tex_coord_buffer.release();

    /*index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(indices, sizeof(indices));*/
    //index_buffer.release();

    vao.release();
    program->release();

    num_of_vertices = vertices.size();
}

void MyOpenGLWidget::initTextures() {
    const float PI = std::acos(-1.0);
    Frame3D<QVector3D> frame(256, 256, 256);
    frame.fill([PI](size_t i, size_t j, size_t k) -> auto {
        return QVector3D(std::abs(std::sin(PI*float(i)/128)),
                         std::abs(std::cos(PI*float(j)/128)),
                         std::abs(std::cos(PI*float(k)/128)));
    });

    texture_3d.setSize(256, 256, 256);
    texture_3d.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    texture_3d.setFormat(QOpenGLTexture::RGB8_UNorm);
    texture_3d.allocateStorage();
    texture_3d.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, static_cast<const void*>(frame.data()));
}

void MyOpenGLWidget::initView() {
    model.setToIdentity();
    view.setToIdentity();
    view.lookAt(QVector3D(3.0f, 3.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    projection.setToIdentity();
    const auto aspect = float(width())/height();
    projection.perspective(45.0f, aspect, 0.01f, 100.0f);
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    auto *gl = context()->functions();

    gl->glViewport(0, 0, width, height);

    initView();
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

    texture_3d.bind();

    vao.bind();
    //const auto count = 12*3;
    //gl->glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
    gl->glDrawArrays(GL_TRIANGLES, 0, num_of_vertices);
    vao.release();

    texture_3d.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    angle += 1.0f;
    update();
}
