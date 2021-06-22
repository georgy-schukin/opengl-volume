#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QTimer>
#include <memory>

#include "frame3d.h"
#include "render/renderer.h"

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=nullptr);

    void setFrame(const Frame3D<GLfloat> &data);
    void setColorPalette(const std::vector<QVector3D> &colors);
    void setOpacityPalette(const std::vector<GLfloat> &values);
    void setRenderer(std::shared_ptr<Renderer> rend);
    void enableLighting(bool enabled);

    size_t getFrameSize() const {
        return frame_size;
    }

    void setBackgroundColor(QColor color);
    QColor getBackgroundColor() const;

    void setCutoff(float low, float high);
    void setCutoffLow(float low);
    void setCutoffHigh(float high);
    std::pair<float, float> getCutoff() const;

signals:
    void initialized();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:    
    void initView();    
    void initRenderer();

    void onTimer();

private:    
    QOpenGLTexture data_texture, color_texture, opacity_texture;

    QMatrix4x4 model_matrix, view_matrix, projection_matrix;

    std::shared_ptr<Renderer> renderer;

    float rotation_y_angle {0.0f}, rotation_x_angle {0.0f};

    QTimer timer;
    int timer_interval {60};

    QPoint mouse_pos {0, 0};

    size_t frame_size {256};

    QColor background_color {0, 0, 25};

    float cutoff_low {0.0f}, cutoff_high {1.0f};

    bool update_renderer = false;
    bool lighting_enabled = false;
};

