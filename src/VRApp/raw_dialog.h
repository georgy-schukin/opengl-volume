#pragma once

#include "../common/types.h"

#include <QDialog>
#include <cstddef>
#include <vector>

namespace Ui {
class RawDialog;
}

class RawDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RawDialog(QWidget *parent = nullptr, QString frame_dir = "");
    ~RawDialog() override;

    QString getFilename() const;
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getDepth() const;
    ValueType getValueType() const;

private slots:
    void on_filenameButton_clicked();

private:
    void detectParamsFromFilename(QString filename);

private:
    Ui::RawDialog *ui;
    QString base_dir;
    std::vector<std::pair<ValueType, QString>> type_items;
};
