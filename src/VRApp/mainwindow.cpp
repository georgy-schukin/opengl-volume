#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStatusBar>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->mainToolBar->hide();
    ui->statusBar->hide();        
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSector_triggered() {
    ui->openGLWidget->setSectorFrame();
}

void MainWindow::on_actionRandom_triggered() {
    ui->openGLWidget->setRandomFrame();
}

void MainWindow::on_actionSphere_triggered() {
    ui->openGLWidget->setSphereFrame();
}
