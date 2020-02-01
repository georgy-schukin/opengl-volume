#ifndef RAW_DIALOG_H
#define RAW_DIALOG_H

#include <QDialog>

namespace Ui {
class RawDialog;
}

class RawDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RawDialog(QWidget *parent = nullptr);
    ~RawDialog();

private:
    Ui::RawDialog *ui;
};

#endif // RAW_DIALOG_H
