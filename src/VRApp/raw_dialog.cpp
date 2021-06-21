#include "raw_dialog.h"
#include "ui_raw_dialog.h"

#include <QFileDialog>
#include <QLineEdit>
#include <QRegularExpression>
#include <vector>
#include <algorithm>

RawDialog::RawDialog(QWidget *parent, QString frame_dir) :
    QDialog(parent),
    ui(new Ui::RawDialog),
    base_dir(frame_dir)
{
    ui->setupUi(this);

    type_items = {
        {ValueType::VT_INT8, QString("int8")},
        {ValueType::VT_UINT8, QString("uint8")},
        {ValueType::VT_INT16, QString("int16")},
        {ValueType::VT_UINT16, QString("uint16")},
        {ValueType::VT_INT32, QString("int32")},
        {ValueType::VT_UINT32, QString("uint32")},
        {ValueType::VT_FLOAT, QString("float32")}
    };

    for (const auto &p: type_items) {
        ui->typeComboBox->addItem(p.second);
    }
    ui->typeComboBox->setCurrentIndex(0);

    connect(ui->filenameEdit, &QLineEdit::textChanged, [this](const QString &filename) {
        detectParamsFromFilename(filename);
    });
}

RawDialog::~RawDialog()
{
    delete ui;
}

QString RawDialog::getFilename() const {
    return ui->filenameEdit->text();
}

size_t RawDialog::getWidth() const {
    return ui->widthSpinBox->value();
}

size_t RawDialog::getHeight() const {
    return ui->heightSpinBox->value();
}

size_t RawDialog::getDepth() const {
    return ui->depthSpinBox->value();
}

ValueType RawDialog::getValueType() const {
    return type_items.at(ui->typeComboBox->currentIndex()).first;
}

void RawDialog::on_filenameButton_clicked() {
    auto filename = QFileDialog::getOpenFileName(this, "Select file", base_dir, "All files(*.*)");
    if (filename.isNull()) {
        return;
    }
    ui->filenameEdit->setText(filename);
}

void RawDialog::detectParamsFromFilename(QString filename) {
    const auto basename = QFileInfo(filename).fileName().toLower();
    QRegularExpression re("^\\D*(\\d+)\\D+(\\d+)\\D+(\\d+)");
    auto match = re.match(basename, 0, QRegularExpression::PartialPreferCompleteMatch);
    if (match.hasMatch()) {
        ui->widthSpinBox->setValue(match.captured(1).toInt());
        ui->heightSpinBox->setValue(match.captured(2).toInt());
        ui->depthSpinBox->setValue(match.captured(3).toInt());
    }
    std::vector<std::pair<size_t, QString>> sorted;
    for (size_t i = 0; i < type_items.size(); i++) {
        sorted.push_back({i, type_items.at(i).second});
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto &p1, const auto &p2) {
        return p1.second > p2.second; // sort desc by string length, bigger string first
    });
    for (const auto &p: sorted) {
        if (basename.indexOf(p.second) >= 0) {
            ui->typeComboBox->setCurrentIndex(p.first);
            break;
        }
    }


}
