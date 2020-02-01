#include "raw_dialog.h"
#include "ui_raw_dialog.h"

RawDialog::RawDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RawDialog)
{
    ui->setupUi(this);
}

RawDialog::~RawDialog()
{
    delete ui;
}
