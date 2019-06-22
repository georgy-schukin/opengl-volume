#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"

#include <QStatusBar>
#include <QToolBar>

#include <cmath>

namespace {

std::vector<GLfloat> defaultOpacityPalette() {
    return {0.0f, 0.001f, 0.002f, 0.003f, 0.01f, 0.02f, 0.05f, 1.0f};
}

std::vector<GLfloat> powOpacityPalette(int n) {
    return makeOpacityPalette(1024, [n](const GLfloat &x) {return std::pow(x, n);});
}

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mainToolBar->hide();
    ui->statusBar->hide();

    gl_widget = ui->openGLWidget;

    connect(gl_widget, &MyOpenGLWidget::initialized, this, &MainWindow::initGlWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGlWidget() {
    gl_widget->setFrame(makeSectorFrame(gl_widget->getFrameSize()));    
    gl_widget->setColorPalette(makeRainbowPalette());
    gl_widget->setOpacityPalette(defaultOpacityPalette());
    gl_widget->update();
}

void MainWindow::on_actionSector_triggered() {
    gl_widget->setFrame(makeSectorFrame(gl_widget->getFrameSize()));
    gl_widget->update();
}

void MainWindow::on_actionRandom_triggered() {
    gl_widget->setFrame(makeRandomFrame(gl_widget->getFrameSize()));
    gl_widget->update();
}

void MainWindow::on_actionSphere_triggered() {
    gl_widget->setFrame(makeSphereFrame(gl_widget->getFrameSize()));
    gl_widget->update();
}

void MainWindow::on_actionParabololoid_triggered() {
    gl_widget->setFrame(makeParaboloidFrame(gl_widget->getFrameSize(), 0.2f));
    gl_widget->update();
}

void MainWindow::on_actionHyperboloid_triggered() {
    gl_widget->setFrame(makeHyperboloidFrame(gl_widget->getFrameSize(), 0.2f));
    gl_widget->update();
}

void MainWindow::on_actionHyperbolic_Paraboloid_triggered() {
    gl_widget->setFrame(makeHyperbolicParaboloidFrame(gl_widget->getFrameSize(), 0.2f));
    gl_widget->update();
}

void MainWindow::on_actionHelix_triggered() {
    gl_widget->setFrame(makeHelixFrame(gl_widget->getFrameSize(), 0.2f, 0.7f, 0.1f, 8.0f));
    gl_widget->update();
}

void MainWindow::on_actionHelicoid_triggered() {
    gl_widget->setFrame(makeHelicoidFrame(gl_widget->getFrameSize(), 0.2f));
    gl_widget->update();
}

void MainWindow::on_actionTorus_triggered() {
    gl_widget->setFrame(makeTorusFrame(gl_widget->getFrameSize(), 0.2f, 0.7f, 0.2f));
    gl_widget->update();
}

void MainWindow::on_actionBubbles_triggered() {
    gl_widget->setFrame(makeBubblesFrame(gl_widget->getFrameSize(), 20, 0.05f, 0.25f));
    gl_widget->update();
}

void MainWindow::on_actionOpDefault_triggered() {
    gl_widget->setOpacityPalette(defaultOpacityPalette());
    gl_widget->update();
}

void MainWindow::on_actionOp_x_triggered() {
    gl_widget->setOpacityPalette(powOpacityPalette(1));
    gl_widget->update();
}

void MainWindow::on_actionOp_x_2_triggered() {
    gl_widget->setOpacityPalette(powOpacityPalette(2));
    gl_widget->update();
}

void MainWindow::on_actionOp_x_4_triggered() {
    gl_widget->setOpacityPalette(powOpacityPalette(4));
    gl_widget->update();
}

void MainWindow::on_actionOp_x_8_triggered() {
    gl_widget->setOpacityPalette(powOpacityPalette(8));
    gl_widget->update();
}
