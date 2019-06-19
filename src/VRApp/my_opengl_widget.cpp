#include "my_opengl_widget.h"
#include "frame3d.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#include <cmath>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    tex_coord_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer),
    texture_3d(QOpenGLTexture::Target3D),
    palette(QOpenGLTexture::Target1D)
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

    const std::vector<unsigned int> indices = {
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
    vertex_buffer.allocate(vertices.data(), vertices.size()*sizeof(QVector3D));
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
    tex_coord_buffer.allocate(tex_coords.data(), tex_coords.size()*sizeof(QVector3D));
    program->enableAttributeArray(tc_loc);
    program->setAttributeBuffer(tc_loc, GL_FLOAT, 0, 3);
    tex_coord_buffer.release();

    /*index_buffer.create();
    index_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    index_buffer.bind();
    index_buffer.allocate(&indices[0], indices.size()*sizeof(size_t));
    index_buffer.release();*/

    vao.release();
    program->release();

    num_of_vertices = vertices.size();
    num_of_indices = indices.size();
}

void MyOpenGLWidget::initTextures() {
    const size_t tex_size = 256;
    //const float PI = std::acos(-1.0f);
    const float domain_size = 5.0f;
    Frame3D<float> frame(tex_size, tex_size, tex_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = domain_size*float(i)/(tex_size - 1);
        const auto y = domain_size*float(j)/(tex_size - 1);
        const auto z = domain_size*float(k)/(tex_size - 1);
        return std::sqrt(x*x + y*y + z*z)/(domain_size*std::sqrt(3.0f));
    });

    texture_3d.setSize(tex_size, tex_size, tex_size);
    texture_3d.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    texture_3d.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture_3d.setFormat(QOpenGLTexture::R16F);
    texture_3d.allocateStorage();
    texture_3d.setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, static_cast<const void*>(frame.data()));

    const std::vector<QVector3D> palette_colors = {
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(0.0f, 1.0f, 1.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(1.0f, 1.0f, 0.0f),
        QVector3D(1.0f, 0.0f, 0.0f)
    };

    palette.setSize(palette_colors.size());
    palette.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,  QOpenGLTexture::LinearMipMapLinear);
    palette.setWrapMode(QOpenGLTexture::ClampToEdge);
    palette.setFormat(QOpenGLTexture::RGB8_UNorm);
    palette.allocateStorage();
    palette.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, palette_colors.data());
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
    rotate.rotate(rotation_y_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(rotation_z_angle, QVector3D(0.0f, 0.0f, 1.0f));
    program->setUniformValue(mvp_loc, projection*view*rotate*model);

    const int tex3d_loc = program->uniformLocation("texture3d");
    const int pal_loc = program->uniformLocation("palette");

    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(tex3d_loc, 0);
    texture_3d.bind();

    gl->glActiveTexture(GL_TEXTURE1);
    program->setUniformValue(pal_loc, 1);
    palette.bind();

    vao.bind();
    //gl->glDrawElements(GL_TRIANGLES, num_of_indices, GL_UNSIGNED_INT, 0);
    gl->glDrawArrays(GL_TRIANGLES, 0, num_of_vertices);
    vao.release();

    texture_3d.release();
    palette.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    rotation_y_angle += 1.0f;
    update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    mouse_pos = event->pos();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    rotation_y_angle += float(event->pos().x() - mouse_pos.x());
    rotation_z_angle += float(event->pos().y() - mouse_pos.y());
    mouse_pos = event->pos();
    update();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event) {
    const auto coeff = (event->angleDelta().y() > 0 ? 1.0f : -1.0f);
    view.translate(coeff*QVector3D(0.2f, 0.2f, 0.2f));
    update();
}
