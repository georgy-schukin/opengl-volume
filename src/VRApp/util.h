#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>

Frame3D<GLfloat> makeRandomFrame(size_t dim_size);
Frame3D<GLfloat> makeSectorFrame(size_t dim_size);
Frame3D<GLfloat> makeSphereFrame(size_t dim_size);

std::vector<QVector3D> makeRainbowPalette();

template <typename Func>
std::vector<GLfloat> makeOpacityPalette(size_t size, Func func) {
    std::vector<GLfloat> values;
    for (size_t i = 0; i <= size; i++) {
        values.push_back(func(GLfloat(i)/size));
    }
    return values;
}
