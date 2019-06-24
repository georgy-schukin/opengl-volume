#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cutoff_dialog.h"
#include "util.h"

#include <QStatusBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QSettings>

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
    setFrame(makeSectorFrame(gl_widget->getFrameSize()));
    setColorPalette(makeRainbowPalette());
    setOpacityPalette(defaultOpacityPalette());
}

void MainWindow::on_actionSector_triggered() {
    setFrame(makeSectorFrame(gl_widget->getFrameSize()));
}

void MainWindow::on_actionRandom_triggered() {
    setFrame(makeRandomFrame(gl_widget->getFrameSize()));
}

void MainWindow::on_actionSphere_triggered() {
    setFrame(makeSphereFrame(gl_widget->getFrameSize()));
}

void MainWindow::on_actionParabololoid_triggered() {
    setFrame(makeParaboloidFrame(gl_widget->getFrameSize(), 0.2f));
}

void MainWindow::on_actionHyperboloid_triggered() {
    setFrame(makeHyperboloidFrame(gl_widget->getFrameSize(), 0.2f));
}

void MainWindow::on_actionHyperbolic_Paraboloid_triggered() {
    setFrame(makeHyperbolicParaboloidFrame(gl_widget->getFrameSize(), 0.2f));
}

void MainWindow::on_actionHelix_triggered() {
    setFrame(makeHelixFrame(gl_widget->getFrameSize(), 0.2f, 0.6f, 0.1f, 8.0f));
}

void MainWindow::on_actionHelicoid_triggered() {
    setFrame(makeHelicoidFrame(gl_widget->getFrameSize(), 0.2f));
}

void MainWindow::on_actionTorus_triggered() {
    setFrame(makeTorusFrame(gl_widget->getFrameSize(), 0.2f, 0.7f, 0.2f));
}

void MainWindow::on_actionBubbles_triggered() {
    setFrame(makeBubblesFrame(gl_widget->getFrameSize(), 20, 0.05f, 0.25f));
}

void MainWindow::on_actionOpDefault_triggered() {
    setOpacityPalette(defaultOpacityPalette());
}

void MainWindow::on_actionOp_x_triggered() {
    setOpacityPalette(powOpacityPalette(1));
}

void MainWindow::on_actionOp_x_2_triggered() {
    setOpacityPalette(powOpacityPalette(2));
}

void MainWindow::on_actionOp_x_3_triggered() {
    setOpacityPalette(powOpacityPalette(3));
}

void MainWindow::on_actionOp_x_4_triggered() {
    setOpacityPalette(powOpacityPalette(4));
}

void MainWindow::on_actionOp_x_5_triggered() {
    setOpacityPalette(powOpacityPalette(5));
}

void MainWindow::on_actionOp_x_6_triggered() {
    setOpacityPalette(powOpacityPalette(6));
}

void MainWindow::on_actionOp_x_7_triggered() {
    setOpacityPalette(powOpacityPalette(7));
}

void MainWindow::on_actionOp_x_8_triggered() {
    setOpacityPalette(powOpacityPalette(8));
}

void MainWindow::on_actionOp_x_9_triggered() {
    setOpacityPalette(powOpacityPalette(9));
}

void MainWindow::on_actionPalRainbow_triggered() {
    setColorPalette(makeRainbowPalette());
}

void MainWindow::on_actionPalMonochrome_triggered() {
    setColorPalette(makeMonochromePalette());
}

void MainWindow::on_actionOpen_triggered() {
    const static QString FRAME_DIR = "frame_dir";
    QSettings settings;
    auto filename = QFileDialog::getOpenFileName(this,
                                                 "Open frame file",
                                                 settings.value(FRAME_DIR).toString(),
                                                 "Frame files (*.frame);;All files(*.*)");
    if (filename.isNull()) {
        return;
    }
    setFrame(loadFrameFromFile(filename.toStdString()));
    settings.setValue(FRAME_DIR, QFileInfo(filename).dir().absolutePath());
}

void MainWindow::on_actionExit_triggered() {
    qApp->exit();
}

void MainWindow::on_actionAbout_triggered() {
    QString about =
            QString("Volume Rendering v1.0<br>") +
            QString("Programmed by Georgy Schukin<br>") +
            QString("<a href='mailto:schukin@ssd.sscc.ru'>schukin@ssd.sscc.ru</a>");
    QMessageBox::information(this, "About", about);
}

void MainWindow::setFrame(const Frame3D<GLfloat> &frame) {
    gl_widget->setFrame(frame);
    gl_widget->update();
}

void MainWindow::setColorPalette(const std::vector<QVector3D> &palette) {
    gl_widget->setColorPalette(palette);
    gl_widget->update();
}

void MainWindow::setOpacityPalette(const std::vector<GLfloat> &palette) {
    gl_widget->setOpacityPalette(palette);
    gl_widget->update();
}

void MainWindow::on_actionBackground_Color_triggered() {
    QColor color = QColorDialog::getColor(gl_widget->getBackgroundColor(), this,
                                          "Choose background color",
                                          QColorDialog::DontUseNativeDialog);
    if (color.isValid()) {
        gl_widget->setBackgroundColor(color);
        gl_widget->update();
    }
}

void MainWindow::on_actionCutoff_triggered() {
    const auto cutoff = gl_widget->getCutoff();
    CutoffDialog dlg(cutoff.first, cutoff.second, this);
    if (dlg.exec() == QDialog::Accepted) {
        gl_widget->setCutoff(dlg.getLow(), dlg.getHigh());
        gl_widget->update();
    }
}
