#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QTimer>
#include <memory>

#include "objects/cube.h"

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
    std::shared_ptr<QOpenGLShaderProgram> initProgram(QString vertex_shader_file, QString fragment_shader_file);
    void initView();
    void initObjects();
    void initTextures();
    void onTimer();

private:
    std::shared_ptr<QOpenGLShaderProgram> program;
    QMatrix4x4 model_matrix, view_matrix, projection_matrix, texture_matrix;
    QOpenGLTexture texture_3d, palette;

    std::shared_ptr<Cube> cube;

    float rotation_y_angle {0.0f}, rotation_z_angle {0.0f};

    QTimer timer;
    int timer_interval {60};

    QPoint mouse_pos {0, 0};
};

