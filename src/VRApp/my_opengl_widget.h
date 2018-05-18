#pragma once

#include <QOpenGLWidget>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <memory>

class QOpenGLShaderProgram;

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=0);
    ~MyOpenGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;

private:
    void initProgram();
    void initView();

private:
    std::shared_ptr<QOpenGLShaderProgram> program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vertex_buffer, color_buffer, index_buffer;
    QMatrix4x4 model, view, projection;
};

