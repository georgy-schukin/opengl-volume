#pragma once

#include <QMainWindow>
#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>

namespace Ui {
class MainWindow;
}

class MyOpenGLWidget;

template <typename T>
class Frame3D;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void initGlWidget();

    void on_actionSector_triggered();
    void on_actionRandom_triggered();
    void on_actionSphere_triggered();
    void on_actionParabololoid_triggered();
    void on_actionHyperboloid_triggered();
    void on_actionHyperbolic_Paraboloid_triggered();
    void on_actionHelix_triggered();
    void on_actionHelicoid_triggered();
    void on_actionTorus_triggered();
    void on_actionBubbles_triggered();

    void on_actionOpDefault_triggered();
    void on_actionOp_x_triggered();
    void on_actionOp_x_2_triggered();
    void on_actionOp_x_3_triggered();
    void on_actionOp_x_4_triggered();
    void on_actionOp_x_5_triggered();
    void on_actionOp_x_6_triggered();
    void on_actionOp_x_7_triggered();
    void on_actionOp_x_8_triggered();
    void on_actionOp_x_9_triggered();

    void on_actionPalRainbow_triggered();
    void on_actionPalMonochrome_triggered();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionBackground_Color_triggered();

private:
    void setFrame(const Frame3D<GLfloat> &frame);
    void setColorPalette(const std::vector<QVector3D> &palette);
    void setOpacityPalette(const std::vector<GLfloat> &palette);

private:
    Ui::MainWindow *ui;
    MyOpenGLWidget *gl_widget;
};

