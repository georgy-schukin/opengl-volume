#include "cutoff_dialog.h"
#include "ui_cutoff_dialog.h"

CutoffDialog::CutoffDialog(float low, float high, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutoffDialog),
    low(low), high(high)
{
    ui->setupUi(this);

    ui->cutoff_low->setValue(static_cast<double>(low));
    ui->cutoff_high->setValue(static_cast<double>(high));
}

CutoffDialog::~CutoffDialog() {
    delete ui;
}

void CutoffDialog::on_cutoff_low_valueChanged(double arg1) {
    low = static_cast<float>(arg1);
}

void CutoffDialog::on_cutoff_high_valueChanged(double arg1) {
    high = static_cast<float>(arg1);
}
