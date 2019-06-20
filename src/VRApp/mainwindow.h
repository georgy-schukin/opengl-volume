#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionSector_triggered();

    void on_actionRandom_triggered();

    void on_actionSphere_triggered();

private:
    Ui::MainWindow *ui;
};

