#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>
#include <algorithm>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    a.setApplicationName("OpenGL Volume Rendering");

    MainWindow w;

    const auto geom = qApp->desktop()->availableGeometry();
    const auto screen_size = geom.size();
    const auto dim = static_cast<int>(std::min(screen_size.width()*0.7f, screen_size.height()*0.8f));
    QSize new_size(dim, dim);
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, new_size, geom));

    w.show();

    return a.exec();
}
