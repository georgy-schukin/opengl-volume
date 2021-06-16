#include "my_opengl_widget.h"
#include "frame3d.h"
#include "frame_util.h"
#include "palette_util.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QMessageBox>

#include <cmath>
#include <exception>

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    data_texture(QOpenGLTexture::Target3D),
    color_texture(QOpenGLTexture::Target1D),
    opacity_texture(QOpenGLTexture::Target1D)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSamples(4);
    setFormat(format);

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(timer_interval);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glDisable(GL_DEPTH_TEST);
    gl->glDisable(GL_CULL_FACE);
    gl->glEnable(GL_MULTISAMPLE);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    try {
        program = loadProgram("shaders/plane.vert", "shaders/plane.frag");
    }
    catch (const std::exception &exp) {
        QMessageBox *box = new QMessageBox(QMessageBox::Critical, "Error", exp.what(), QMessageBox::Ok, this);
        box->show();
    }

    initView();
    initObjects();
    initTextures();

    emit initialized();
}

std::shared_ptr<QOpenGLShaderProgram> MyOpenGLWidget::loadProgram(QString vertex_shader_file, QString fragment_shader_file) {
    auto prog = std::make_shared<QOpenGLShaderProgram>();
    if (!prog->addShaderFromSourceFile(QOpenGLShader::Vertex, vertex_shader_file)) {
        throw std::runtime_error(std::string("Failed to load vertex shaders from ") + vertex_shader_file.toStdString()
                                 + ":\n" + prog->log().toStdString());
    }
    if (!prog->addShaderFromSourceFile(QOpenGLShader::Fragment, fragment_shader_file)) {
        throw std::runtime_error(std::string("Failed to load fragment shaders from ") + fragment_shader_file.toStdString()
                                 + ":\n" + prog->log().toStdString());
    }
    if (!prog->link()) {
        throw std::runtime_error(std::string("Failed to link program:\n") + prog->log().toStdString());
    }
    return prog;
}

void MyOpenGLWidget::initObjects() {
    /*cube = std::make_shared<Cube>();
    cube->attachVertices(program.get(), "vertex");
    cube->attachTextureCoords(program.get(), "tCoord");*/

    plane = std::make_shared<Plane>();
    if (program) {
        plane->attachVertices(program.get(), "vertex");
    }

    //hemisphere = std::make_shared<HemiSphere>(1.0, 16, 16);
    //hemisphere->attachVertices(program.get(), "vertex");
}

void MyOpenGLWidget::initTextures() {
    // Will be initialized in MainWindow
}

void MyOpenGLWidget::setFrame(const Frame3D<GLfloat> &data) {
    data_texture.destroy();
    data_texture.setSize(static_cast<int>(data.width()),
                       static_cast<int>(data.height()),
                       static_cast<int>(data.depth()));
    data_texture.setMinMagFilters(QOpenGLTexture::LinearMipMapLinear,  QOpenGLTexture::LinearMipMapLinear);
    data_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    data_texture.setAutoMipMapGenerationEnabled(true);
    data_texture.setMaximumAnisotropy(16.0f);
    data_texture.setBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
    data_texture.setFormat(QOpenGLTexture::R32F);
    data_texture.allocateStorage();
    data_texture.setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, data.data());
}

void MyOpenGLWidget::setColorPalette(const std::vector<QVector3D> &colors) {
    color_texture.destroy();
    color_texture.setSize(static_cast<int>(colors.size()));
    color_texture.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    color_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    color_texture.setFormat(QOpenGLTexture::RGB8_UNorm);
    color_texture.allocateStorage();
    color_texture.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, colors.data());
}

void MyOpenGLWidget::setOpacityPalette(const std::vector<GLfloat> &values) {
    opacity_texture.destroy();
    opacity_texture.setSize(static_cast<int>(values.size()));
    opacity_texture.setMinMagFilters(QOpenGLTexture::Linear,  QOpenGLTexture::Linear);
    opacity_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    opacity_texture.setFormat(QOpenGLTexture::R32F);
    opacity_texture.allocateStorage();
    opacity_texture.setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, values.data());
}

void MyOpenGLWidget::setBackgroundColor(QColor color) {
    background_color = color;
}

QColor MyOpenGLWidget::getBackgroundColor() const {
    return background_color;
}

void MyOpenGLWidget::setCutoff(float low, float high) {
    cutoff_low = low;
    cutoff_high = high;
}

std::pair<float, float> MyOpenGLWidget::getCutoff() const {
    return std::make_pair(cutoff_low, cutoff_high);
}

void MyOpenGLWidget::initView() {
    model_matrix.setToIdentity();

    view_matrix.setToIdentity();
    view_matrix.lookAt(QVector3D(3.0f, 3.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    projection_matrix.setToIdentity();
    const auto aspect = float(width()) / float(height());
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

    gl->glClearColor(static_cast<GLfloat>(background_color.redF()),
                     static_cast<GLfloat>(background_color.greenF()),
                     static_cast<GLfloat>(background_color.blueF()), 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!program ||
        !plane ||
        !data_texture.isCreated() ||
        !color_texture.isCreated() ||
        !opacity_texture.isCreated())
    {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoff_low"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoff_high"), cutoff_high);
    program->setUniformValue(program->uniformLocation("cutoff_coeff"), 1.0f/(cutoff_high - cutoff_low));

    QMatrix4x4 rotate;
    rotate.rotate(rotation_y_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(rotation_x_angle, QVector3D(1.0f, 0.0f, 0.0f));

    const auto texture_inverse_matrix = (view_matrix * rotate * texture_matrix).inverted();

    const auto proj_loc = program->uniformLocation("Proj");
    const auto tex_inv_loc = program->uniformLocation("TexInv");

    /*const int mvp_loc = program->uniformLocation("MVP");
    QMatrix4x4 mvp = projection_matrix*view_matrix*rotate*model_matrix;
    program->setUniformValue(mvp_loc, mvp);*/

    const int tex3d_loc = program->uniformLocation("texture3d");
    const int palette_loc = program->uniformLocation("palette");
    const int opacity_loc = program->uniformLocation("opacity");

    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(tex3d_loc, 0);
    data_texture.bind();

    gl->glActiveTexture(GL_TEXTURE1);
    program->setUniformValue(palette_loc, 1);
    color_texture.bind();

    gl->glActiveTexture(GL_TEXTURE2);
    program->setUniformValue(opacity_loc, 2);
    opacity_texture.bind();

    QMatrix4x4 plane_model_matrix;
    plane_model_matrix.scale(2, 2, 1); // plane is [-2,2] by X and Y

    static const float cube_half_size = 1.0f; // cube vertices' coords are +1/-1
    static const float cube_extent_radius = cube_half_size * std::sqrt(3.0f); // radius of a sphere around cube
    static const float step_coeff = cube_half_size * std::sqrt(2.0f);

    const auto max_dim = std::max(data_texture.width(), std::max(data_texture.height(), data_texture.depth()));
    // Data cube is located in [0,0,0] in world coordinates and has side length of 2.
    const float view_distance = (view_matrix * QVector4D(0, 0, 0, 1)).length(); // distance from the camera to the origin
    const float step = step_coeff / static_cast<float>(max_dim);
    const int num_of_steps = static_cast<int>(2.0f * cube_extent_radius / step);

    plane_model_matrix.translate(0, 0, -view_distance - cube_extent_radius); // plane is in view space
    for (int i = 0; i <= num_of_steps; i++) {
        program->setUniformValue(proj_loc, projection_matrix * plane_model_matrix);
        program->setUniformValue(tex_inv_loc, texture_inverse_matrix * plane_model_matrix);
        plane->draw(gl);
        //hemisphere->draw(gl);
        plane_model_matrix.translate(0, 0, step);
    }

    //cube->draw(gl);

    data_texture.release();
    color_texture.release();

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
    rotation_x_angle += float(event->pos().y() - mouse_pos.y());
    mouse_pos = event->pos();
    update();
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event) {
    const auto coeff = (event->angleDelta().y() > 0 ? 1.0f : -1.0f);
    const auto shift = coeff*QVector3D(0.2f, 0.2f, 0.2f);
    view_matrix.translate(shift);
    update();
}

