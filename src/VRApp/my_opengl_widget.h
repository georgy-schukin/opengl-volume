#pragma once

#include <QOpenGLWidget>

class MyOpenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit MyOpenGLWidget(QWidget *parent=0);
    ~MyOpenGLWidget();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width, int height) override;
    virtual void paintGL() override;
};

