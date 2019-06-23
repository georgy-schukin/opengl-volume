#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MyOpenGLWidget;

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
    void on_actionOp_x_4_triggered();
    void on_actionOp_x_8_triggered();    

    void on_actionPalRainbow_triggered();
    void on_actionPalMonochrome_triggered();

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    MyOpenGLWidget *gl_widget;
};

