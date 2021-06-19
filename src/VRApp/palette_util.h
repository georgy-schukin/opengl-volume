#pragma once

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>
#include <string>
#include <functional>

std::vector<QVector3D> makeRainbowPalette();
std::vector<QVector3D> makeRainbowWithBlackPalette();
std::vector<QVector3D> makeMonochromePalette();

template <typename Func>
std::vector<GLfloat> makeOpacityPalette(size_t size, Func func) {
    std::vector<GLfloat> values;
    for (size_t i = 0; i <= size; i++) {
        // Compute values of the func for x from [0, 1] range.
        // Values of the func should lie in [0, 1] range too.
        values.push_back(func(GLfloat(i)/size));
    }
    return values;
}
