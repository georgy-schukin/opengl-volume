#pragma once

#include <QDialog>

namespace Ui {
class CutoffDialog;
}

class CutoffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutoffDialog(float low, float high, QWidget *parent = nullptr);
    ~CutoffDialog();

    float getLow() const {
        return low;
    }

    float getHigh() const {
        return high;
    }

private slots:
    void on_cutoff_low_valueChanged(double arg1);

    void on_cutoff_high_valueChanged(double arg1);

private:
    Ui::CutoffDialog *ui;
    float low, high;
};

