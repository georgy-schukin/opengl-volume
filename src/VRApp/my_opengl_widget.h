#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QTimer>
#include <memory>

class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=nullptr);

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void initProgram();
    void initView();
    void initTextures();
    void onTimer();

private:
    std::shared_ptr<QOpenGLShaderProgram> program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertex_buffer, color_buffer, tex_coord_buffer, index_buffer;
    QMatrix4x4 model, view, projection;
    QOpenGLTexture texture_3d, palette;

    int num_of_indices {0};

    float rotation_y_angle {0.0f}, rotation_z_angle {0.0f};

    QTimer timer;
    int timer_interval {60};

    QPoint mouse_pos {0, 0};
};

