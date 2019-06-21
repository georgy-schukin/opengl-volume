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

    void on_actionOpDefault_triggered();
    void on_actionOp_x_triggered();
    void on_actionOp_x_2_triggered();
    void on_actionOp_x_4_triggered();
    void on_actionOp_x_8_triggered();

private:
    Ui::MainWindow *ui;
    MyOpenGLWidget *gl_widget;
};

