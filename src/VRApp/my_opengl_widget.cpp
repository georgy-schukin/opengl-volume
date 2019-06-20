#include "my_opengl_widget.h"
#include "frame3d.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#include <cmath>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    texture_3d(QOpenGLTexture::Target3D),
    palette(QOpenGLTexture::Target1D)
{
    QSurfaceFormat format;
    //format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(timer_interval);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    gl->glDisable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_ALWAYS);
    gl->glEnable(GL_MULTISAMPLE);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //program = initProgram("shaders/texture.vert", "shaders/texture.frag");
    program = initProgram("shaders/plane.vert", "shaders/plane.frag");
    initView();
    initObjects();
    initTextures();
}

std::shared_ptr<QOpenGLShaderProgram> MyOpenGLWidget::initProgram(QString vertex_shader_file, QString fragment_shader_file) {
    auto prog = std::make_shared<QOpenGLShaderProgram>();
    prog->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader_file);
    prog->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment_shader_file);
    prog->link();
    return prog;
}

void MyOpenGLWidget::initObjects() {
    /*cube = std::make_shared<Cube>();
    cube->attachVertices(program.get(), "vertex");
    cube->attachTextureCoords(program.get(), "tCoord");*/

    plane = std::make_shared<Plane>();
    plane->attachVertices(program.get(), "vertex");
}

void MyOpenGLWidget::initTextures() {
    const size_t tex_size = 256;
    //const float PI = std::acos(-1.0f);
    const float domain_size = 5.0f;
    Frame3D<GLfloat> frame(tex_size, tex_size, tex_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = domain_size*float(i)/(tex_size - 1);
        const auto y = domain_size*float(j)/(tex_size - 1);
        const auto z = domain_size*float(k)/(tex_size - 1);
        return std::sqrt(x*x + y*y + z*z)/(domain_size*std::sqrt(3.0f));
    });

    texture_3d.setSize(tex_size, tex_size, tex_size);
    //texture_3d.setMinMagFilters(QOpenGLTexture::Nearest,  QOpenGLTexture::Nearest);
    texture_3d.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    texture_3d.setWrapMode(QOpenGLTexture::ClampToEdge);
    texture_3d.setFormat(QOpenGLTexture::R32F);
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
    palette.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    //palette.setMinMagFilters(QOpenGLTexture::Nearest,  QOpenGLTexture::Nearest);
    palette.setWrapMode(QOpenGLTexture::ClampToEdge);
    palette.setFormat(QOpenGLTexture::RGB8_UNorm);
    palette.allocateStorage();
    palette.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, palette_colors.data());
}

void MyOpenGLWidget::initView() {
    model_matrix.setToIdentity();

    view_matrix.setToIdentity();
    view_matrix.lookAt(QVector3D(3.0f, 3.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    projection_matrix.setToIdentity();
    const auto aspect = float(width())/height();
    projection_matrix.perspective(45.0f, aspect, 0.01f, 100.0f);

    texture_matrix.setToIdentity();
    texture_matrix.scale(2.0f);
    texture_matrix.translate(-0.5f, -0.5f, -0.5f);
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

    QMatrix4x4 rotate;
    rotate.setToIdentity();
    rotate.rotate(rotation_y_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(rotation_z_angle, QVector3D(0.0f, 0.0f, 1.0f));

    const auto texture_inverse_matrix = (view_matrix * rotate * texture_matrix).inverted();

    const auto proj_loc = program->uniformLocation("Proj");
    const auto model_loc = program->uniformLocation("Model");
    const auto tex_inv_loc = program->uniformLocation("TexInv");

    program->setUniformValue(proj_loc, projection_matrix);
    program->setUniformValue(tex_inv_loc, texture_inverse_matrix);

    /*const int mvp_loc = program->uniformLocation("MVP");

    QMatrix4x4 mvp = projection_matrix*view_matrix*rotate*model_matrix;
    program->setUniformValue(mvp_loc, mvp);*/

    const int tex3d_loc = program->uniformLocation("texture3d");
    const int pal_loc = program->uniformLocation("palette");

    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(tex3d_loc, 0);
    texture_3d.bind();

    gl->glActiveTexture(GL_TEXTURE1);
    program->setUniformValue(pal_loc, 1);
    palette.bind();

    QMatrix4x4 plane_model;
    plane_model.setToIdentity();
    plane_model.scale(4, 4, 1);
    const float render_length = 4.0f;
    const int volume_size = 128;
    plane_model.translate(0, 0, -3 - render_length);

    const auto step = 1.0f/volume_size;
    for (int i = 0; i < volume_size*render_length; i++) {
        program->setUniformValue(model_loc, plane_model);
        plane->draw(gl);
        plane_model.translate(0, 0, step);
    }

    //cube->draw(gl);

    texture_3d.release();
    palette.release();

    program->release();
}

void MyOpenGLWidget::onTimer() {
    rotation_y_angle += 1.0f;
    update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        mouse_pos = event->pos();
    } else if (event->button() == Qt::RightButton) {
        if (timer.isActive()) {
            timer.stop();
        } else {
            timer.start(timer_interval);
        }
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    rotation_y_angle += float(event->pos().x() - mouse_pos.x());
    rotation_z_angle += float(event->pos().y() - mouse_pos.y());
    mouse_pos = event->pos();
    update();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event) {
    const auto coeff = (event->angleDelta().y() > 0 ? 1.0f : -1.0f);
    view_matrix.translate(coeff*QVector3D(0.2f, 0.2f, 0.2f));
    update();
}
