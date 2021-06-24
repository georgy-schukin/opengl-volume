#include "main_window.h"
#include "ui_main_window.h"
#include "cutoff_dialog.h"
#include "raw_dialog.h"
#include "frame_util.h"
#include "frame_loader.h"
#include "palette_util.h"
#include "cube/cube_util.h"
#include "render/slice_renderer.h"
#include "render/ray_cast_renderer.h"

#include <QStatusBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QSettings>
#include <QSlider>

#include <cmath>

namespace {

std::vector<GLfloat> defaultOpacityPalette() {
    return {0.0f, 0.001f, 0.002f, 0.003f, 0.01f, 0.02f, 0.05f, 1.0f};
}

std::vector<GLfloat> powOpacityPalette(int n) {
    return makeOpacityPalette(1024, [n](const GLfloat &x) {return std::pow(x, n);});
}

std::vector<GLfloat> logOpacityPalette(float step = 1.0f) {
    return makeOpacityPalette(1024, [step](const GLfloat &x) {return std::log(x + step) / std::log(1.0f + step);});
}

QColor getColorSetting(QString key, QColor default_color) {
    QSettings settings;
    auto value = settings.value(key, static_cast<unsigned int>(default_color.rgb()));
    return QColor(QRgb(value.toUInt()));
}

void setColorSetting(QString key, QColor color) {
    QSettings settings;
    settings.setValue(key, static_cast<unsigned int>(color.rgb()));
}

template <class T>
void setSetting(QString key, T value) {
    QSettings settings;
    settings.setValue(key, value);
}

template <class T>
QVariant getSetting(QString key, T default_value) {
    QSettings settings;
    return settings.value(key, default_value);
}

const static QString BG_COLOR_KEY = "background-color";
const static QString SINGLE_COLOR_KEY = "single-color";
const static QString FRAME_DIR_KEY = "frame-dir";
const static QString FRAME_FILTER_KEY = "frame-filter";
const static QString SHOW_TOOLBAR_KEY = "show-toolbar";
const static QString SHOW_STATUSBAR_KEY = "show-statusbar";
const static QString ENABLE_LIGHTING_KEY = "enable-lighting";
const static QString ENABLE_JITTER_KEY = "enable-jitter";
const static QString ENABLE_CORRECT_SCALE_KEY = "enable-correct-scale";
const static QString CUTOFF_LOW_KEY = "cutoff-low";
const static QString CUTOFF_HIGH_KEY = "cutoff-high";
const static QString STEP_MULTIPLIER_KEY = "step-multiplier";

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gl_widget = ui->openGLWidget;
    connect(gl_widget, &MyOpenGLWidget::initialized, this, &MainWindow::initGlWidget);

    initStatusbar();
    initToolbar();

    default_title = windowTitle();
    setWindowIcon(QIcon(":/resources/cube.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatusbar() {
    size_label = new QLabel(this);

    cutoff_label = new QLabel(this);
    connect(this, &MainWindow::cutoffChanged, [this](float low, float high) {
       cutoff_label->setText(QString("Cutoff: [%0, %1]").arg(low).arg(high));
    });

    ui->statusBar->addWidget(size_label);
    ui->statusBar->addWidget(cutoff_label);

    const auto show = getSetting(SHOW_STATUSBAR_KEY, true).toBool();
    ui->actionShow_hide_Statusbar->setChecked(show);
    ui->statusBar->setHidden(!show);
}

void MainWindow::initToolbar() {
    slider_low = new QSlider(Qt::Horizontal, this);
    slider_low->setMinimum(0);
    slider_low->setMaximum(100);
    slider_low->setValue(slider_low->minimum());
    slider_low->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(slider_low, &QSlider::valueChanged, [this](int value) {
        setCutoff(float(value) / slider_low->maximum(), getCutoff().second);
    });

    slider_high = new QSlider(Qt::Horizontal, this);
    slider_high->setMinimum(0);
    slider_high->setMaximum(100);
    slider_high->setValue(slider_high->maximum());
    slider_high->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(slider_high, &QSlider::valueChanged, [this](int value) {
        setCutoff(getCutoff().first, float(value) / slider_high->maximum());
    });

    connect(this, &MainWindow::cutoffChanged, [this](float low, float high) {
        const auto s_low = static_cast<int>(low * slider_low->maximum());
        const auto s_high = static_cast<int>(high * slider_high->maximum());
        if (s_low != slider_low->value()) {
            slider_low->setValue(s_low);
        }
        if (s_high != slider_high->value()) {
            slider_high->setValue(s_high);
        }
    });

    step_mult_box = new QSpinBox(this);
    step_mult_box->setMinimum(1);
    step_mult_box->setMaximum(1000);
    step_mult_box->setValue(getSetting(STEP_MULTIPLIER_KEY, 1).toInt());
    step_mult_box->setFocusPolicy(Qt::TabFocus);
    connect(step_mult_box, qOverload<int>(&QSpinBox::valueChanged), [this](int value) {
        setStepMultiplier(value);
    });

    ui->mainToolBar->addWidget(new QLabel("Low: ", this));
    ui->mainToolBar->addWidget(slider_low);
    ui->mainToolBar->addWidget(new QLabel("High: ", this));
    ui->mainToolBar->addWidget(slider_high);
    ui->mainToolBar->addWidget(new QLabel("Step mult: ", this));
    ui->mainToolBar->addWidget(step_mult_box);

    const auto show = getSetting(SHOW_TOOLBAR_KEY, true).toBool();
    ui->actionShow_hide_Toolbar->setChecked(show);
    ui->mainToolBar->setHidden(!show);
}

void MainWindow::initGlWidget() {
    auto bgColor = getColorSetting(BG_COLOR_KEY, Qt::white);
    if (bgColor.isValid()) {
        gl_widget->setBackgroundColor(bgColor);
    }

    setFrame(makeSectorFrame(gl_widget->getFrameSize()), "Sector");
    setColorPalette(makeRainbowWithBlackPalette());
    setOpacityPalette(powOpacityPalette(1));
    setRenderer(std::make_shared<RayCastRenderer>());

    const auto c_low = getSetting(CUTOFF_LOW_KEY, 0.0).toFloat();
    const auto c_high = getSetting(CUTOFF_HIGH_KEY, 1.0).toFloat();
    setCutoff(c_low, c_high);
    setStepMultiplier(getSetting(STEP_MULTIPLIER_KEY, 1).toInt());

    const auto lighting_enabled = getSetting(ENABLE_LIGHTING_KEY, false).toBool();
    const auto jitter_enabled = getSetting(ENABLE_JITTER_KEY, false).toBool();
    const auto correct_scale_enabled = getSetting(ENABLE_CORRECT_SCALE_KEY, false).toBool();
    ui->actionUse_Lighting->setChecked(lighting_enabled);
    ui->actionEnable_Jitter->setChecked(jitter_enabled);
    ui->actionCorrect_Scale->setChecked(correct_scale_enabled);
    enableLighting(lighting_enabled);
    enableJitter(jitter_enabled);
    enableCorrectScale(correct_scale_enabled);
}

void MainWindow::setFrame(const Frame3D<GLfloat> &frame, const QString &title) {
    setWindowTitle(default_title + (!title.isEmpty() ? ": " + title : ""));
    size_label->setText(QString("Size: %0 x %1 x %2").arg(frame.width()).arg(frame.height()).arg(frame.depth()));
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

void MainWindow::setRenderer(std::shared_ptr<Renderer> renderer) {
    gl_widget->setRenderer(renderer);
    gl_widget->update();
}

void MainWindow::setCutoff(float low, float high) {
    gl_widget->setCutoff(low, high);
    gl_widget->update();
    setSetting(CUTOFF_LOW_KEY, low);
    setSetting(CUTOFF_HIGH_KEY, high);
    emit cutoffChanged(low, high);
}

void MainWindow::enableLighting(bool enabled) {
    gl_widget->enableLighting(enabled);
    gl_widget->update();
    setSetting(ENABLE_LIGHTING_KEY, enabled);
}

void MainWindow::enableJitter(bool enabled) {
    gl_widget->enableJitter(enabled);
    gl_widget->update();
    setSetting(ENABLE_JITTER_KEY, enabled);
}

void MainWindow::enableCorrectScale(bool enabled) {
    gl_widget->enableCorrectScale(enabled);
    gl_widget->update();
    setSetting(ENABLE_CORRECT_SCALE_KEY, enabled);
}

void MainWindow::setStepMultiplier(int multiplier) {
    gl_widget->setStepMultiplier(multiplier);
    gl_widget->update();
    setSetting(STEP_MULTIPLIER_KEY, multiplier);
}

std::pair<float, float> MainWindow::getCutoff() const {
    return gl_widget->getCutoff();
}

void MainWindow::on_actionSector_triggered() {
    setFrame(makeSectorFrame(gl_widget->getFrameSize()), "Sector");
}

void MainWindow::on_actionRandom_triggered() {
    setFrame(makeRandomFrame(gl_widget->getFrameSize()), "Random");
}

void MainWindow::on_actionSphere_triggered() {
    setFrame(makeSphereFrame(gl_widget->getFrameSize()), "Sphere");
}

void MainWindow::on_actionParabololoid_triggered() {
    setFrame(makeParaboloidFrame(gl_widget->getFrameSize(), 0.2), "Paraboloid");
}

void MainWindow::on_actionHyperboloid_triggered() {
    setFrame(makeHyperboloidFrame(gl_widget->getFrameSize(), 0.2), "Hyperboloid");
}

void MainWindow::on_actionHyperbolic_Paraboloid_triggered() {
    setFrame(makeHyperbolicParaboloidFrame(gl_widget->getFrameSize(), 0.2), "Hyperbolic Paraboloid");
}

void MainWindow::on_actionHelix_triggered() {
    setFrame(makeHelixFrame(gl_widget->getFrameSize(), 0.2, 0.6, 0.1, 8.0), "Helix");
}

void MainWindow::on_actionHelicoid_triggered() {
    setFrame(makeHelicoidFrame(gl_widget->getFrameSize(), 0.2), "Helicoid");
}

void MainWindow::on_actionTorus_triggered() {
    setFrame(makeTorusFrame(gl_widget->getFrameSize(), 0.2, 0.7, 0.2), "Torus");
}

void MainWindow::on_actionBubbles_triggered() {
    setFrame(makeBubblesFrame(gl_widget->getFrameSize(), 20, 0.05, 0.25), "Bubbles");
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

void MainWindow::on_actionOpLog_triggered() {
    setOpacityPalette(logOpacityPalette());
}

void MainWindow::on_actionOpFull_triggered() {
    setOpacityPalette(std::vector<GLfloat> {1.0f});
}

void MainWindow::on_actionPalRainbow_triggered() {
    setColorPalette(makeRainbowPalette());
}

void MainWindow::on_actionPalRainbow_w_black_triggered() {
    setColorPalette(makeRainbowWithBlackPalette());
}

void MainWindow::on_actionPalMonochrome_triggered() {
    setColorPalette(makeMonochromePalette());
}

void MainWindow::on_actionPalSingle_color_triggered() {
    QColor color = QColorDialog::getColor(getColorSetting(SINGLE_COLOR_KEY, Qt::red), this,
                                          "Choose color",
                                          QColorDialog::DontUseNativeDialog);
    if (color.isValid()) {
        setColorPalette({QVector3D(color.redF(), color.greenF(), color.blueF())});
        setColorSetting(SINGLE_COLOR_KEY, color);
    }
}

void MainWindow::on_actionOpen_triggered() {
    QSettings settings;
    QString selectedFilter = settings.value(FRAME_FILTER_KEY).toString();
    auto filename = QFileDialog::getOpenFileName(this,
                                                 "Open frame file",
                                                 settings.value(FRAME_DIR_KEY).toString(),
                                                 "Frame files (*.frame);;Cube files (*.cube);;All files(*.*)",
                                                 &selectedFilter);
    if (filename.isNull()) {
        return;
    }
    try {
        if (filename.endsWith(".cube")) {
            setFrame(cube::loadCube(filename.toStdString()), QFileInfo(filename).fileName());
        } else {
            setFrame(FrameLoader::load(filename.toStdString()), QFileInfo(filename).fileName());
        }
        settings.setValue(FRAME_DIR_KEY, QFileInfo(filename).dir().absolutePath());
        settings.setValue(FRAME_FILTER_KEY, selectedFilter);
    }
    catch (const std::exception &e) {
        showError(e.what());
    }
}

void MainWindow::on_actionOpen_Raw_triggered() {
    try {
        QSettings settings;
        RawDialog dlg(this, settings.value(FRAME_DIR_KEY).toString());
        if (dlg.exec() == QDialog::Accepted) {
            const auto filename = dlg.getFilename();
            setFrame(FrameLoader::loadRaw(filename.toStdString(),
                                          dlg.getWidth(), dlg.getHeight(), dlg.getDepth(),
                                          dlg.getValueType()), QFileInfo(filename).fileName());
            settings.setValue(FRAME_DIR_KEY, QFileInfo(filename).dir().absolutePath());
        }
    }
    catch (const std::exception &e) {
        showError(e.what());
    }
}

void MainWindow::on_actionExit_triggered() {
    qApp->exit();
}

void MainWindow::on_actionAbout_triggered() {
    QString about =
            QString("Volume Rendering v1.0<br>") +
            QString("Programmed by Georgy Schukin<br>") +
            QString("<a href='mailto:georgy.schukin@gmail.com'>georgy.schukin@gmail.com</a>");
    QMessageBox::information(this, "About", about);
}

void MainWindow::on_actionBackground_Color_triggered() {
    QColor color = QColorDialog::getColor(gl_widget->getBackgroundColor(), this,
                                          "Choose background color",
                                          QColorDialog::DontUseNativeDialog);
    if (color.isValid()) {
        setColorSetting(BG_COLOR_KEY, color);
        gl_widget->setBackgroundColor(color);
        gl_widget->update();
    }
}

void MainWindow::on_actionCutoff_triggered() {
    const auto cutoff = getCutoff();
    CutoffDialog dlg(cutoff.first, cutoff.second, this);
    if (dlg.exec() == QDialog::Accepted) {
        setCutoff(dlg.getLow(), dlg.getHigh());
    }
}

void MainWindow::on_actionRenderSlices_triggered() {
    setRenderer(std::make_shared<SliceRenderer>());
}

void MainWindow::on_actionRenderRay_Casting_triggered() {
    setRenderer(std::make_shared<RayCastRenderer>());
}

void MainWindow::on_actionShow_hide_Toolbar_triggered() {
    const auto show = ui->mainToolBar->isHidden();
    ui->mainToolBar->setHidden(!show);
    setSetting(SHOW_TOOLBAR_KEY, show);
}

void MainWindow::on_actionShow_hide_Statusbar_triggered() {
    const auto show = ui->statusBar->isHidden();
    ui->statusBar->setHidden(!show);
    setSetting(SHOW_STATUSBAR_KEY, show);
}

void MainWindow::on_actionUse_Lighting_triggered() {
    enableLighting(ui->actionUse_Lighting->isChecked());
}

void MainWindow::showError(QString message) {
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::on_actionCorrect_Scale_triggered() {
    enableCorrectScale(ui->actionCorrect_Scale->isChecked());
}

void MainWindow::on_actionEnable_Jitter_triggered() {
    enableJitter(ui->actionEnable_Jitter->isChecked());
}
