#include "my_opengl_widget.h"
#include "frame3d.h"
#include "frame_util.h"
#include "palette_util.h"
#include "render/slice_renderer.h"
#include "render/ray_cast_renderer.h"

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

    renderer = std::make_shared<RayCastRenderer>();

    connect(&timer, &QTimer::timeout, this, &MyOpenGLWidget::onTimer);
    timer.start(timer_interval);
}

void MyOpenGLWidget::initializeGL() {
    auto *gl = context()->functions();

    gl->glEnable(GL_MULTISAMPLE);

    initView();
    initRenderer();

    emit initialized();
}

void MyOpenGLWidget::initView() {
    model_matrix.setToIdentity();

    view_matrix.setToIdentity();
    view_matrix.lookAt(QVector3D(3.0f, 3.0f, 3.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    projection_matrix.setToIdentity();
    const auto aspect = float(width()) / float(height());
    projection_matrix.perspective(45.0f, aspect, 0.01f, 100.0f);
}

void MyOpenGLWidget::initRenderer() {
    try {
        renderer->init(context()->functions());
        renderer->setDataTexture(&data_texture);
        renderer->setColorTexture(&color_texture);
        renderer->setOpacityTexture(&opacity_texture);
        renderer->enableLighting(lighting_enabled);
    }
    catch (const std::exception &exp) {
        QMessageBox::critical(this, "Error", exp.what());
    }
}

void MyOpenGLWidget::setFrame(const Frame3D<GLfloat> &data) {
    data_texture.destroy();
    data_texture.setSize(static_cast<int>(data.width()),
                       static_cast<int>(data.height()),
                       static_cast<int>(data.depth()));
    data_texture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    data_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    //data_texture.setAutoMipMapGenerationEnabled(true);
    data_texture.setMaximumAnisotropy(16.0f);
    data_texture.setBorderColor(0.0f, 0.0f, 0.0f, 0.0f);
    data_texture.setFormat(QOpenGLTexture::R32F);
    data_texture.allocateStorage();
    data_texture.setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, data.data());
}

void MyOpenGLWidget::setColorPalette(const std::vector<QVector3D> &colors) {
    color_texture.destroy();
    color_texture.setSize(static_cast<int>(colors.size()));
    color_texture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    color_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    color_texture.setFormat(QOpenGLTexture::RGB8_UNorm);
    color_texture.allocateStorage();
    color_texture.setData(QOpenGLTexture::RGB, QOpenGLTexture::Float32, colors.data());
}

void MyOpenGLWidget::setOpacityPalette(const std::vector<GLfloat> &values) {
    opacity_texture.destroy();
    opacity_texture.setSize(static_cast<int>(values.size()));
    opacity_texture.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    opacity_texture.setWrapMode(QOpenGLTexture::ClampToEdge);
    opacity_texture.setFormat(QOpenGLTexture::R32F);
    opacity_texture.allocateStorage();
    opacity_texture.setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, values.data());
}

void MyOpenGLWidget::setRenderer(std::shared_ptr<Renderer> rend) {
    renderer = rend;
    update_renderer = true;
}

void MyOpenGLWidget::enableLighting(bool enabled) {
    lighting_enabled = enabled;
    if (renderer) {
        renderer->enableLighting(enabled);
    }
}

void MyOpenGLWidget::enableCorrectScale(bool enabled) {
    correct_scale = enabled;
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

void MyOpenGLWidget::setCutoffLow(float low) {
    cutoff_low = low;
}

void MyOpenGLWidget::setCutoffHigh(float high) {
    cutoff_high = high;
}

std::pair<float, float> MyOpenGLWidget::getCutoff() const {
    return std::make_pair(cutoff_low, cutoff_high);
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

    if (!renderer) {
        return;
    }

    QMatrix4x4 rotate;
    rotate.rotate(rotation_y_angle, QVector3D(0.0f, 1.0f, 0.0f));
    rotate.rotate(rotation_x_angle, QVector3D(1.0f, 0.0f, 0.0f));

    QMatrix4x4 scale;
    if (correct_scale) {
        const auto md = static_cast<GLfloat>(std::max(data_texture.width(), std::max(data_texture.height(), data_texture.depth())));
        scale.scale(static_cast<GLfloat>(data_texture.width()) / md,
                    static_cast<GLfloat>(data_texture.height()) / md,
                    static_cast<GLfloat>(data_texture.depth()) / md);
    }

    if (update_renderer) {
        initRenderer();
        update_renderer = false;
    }

    renderer->setMVP(rotate * scale * model_matrix, view_matrix, projection_matrix);
    renderer->setCutoff(cutoff_low, cutoff_high);
    renderer->render(gl);
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

