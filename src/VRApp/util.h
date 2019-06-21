#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>
#include <functional>

Frame3D<GLfloat> makeRandomFrame(size_t dim_size);
Frame3D<GLfloat> makeSectorFrame(size_t dim_size);
Frame3D<GLfloat> makeSphereFrame(size_t dim_size);
Frame3D<GLfloat> makeAnalyticalSurfaceFrame(size_t dim_size, float cutoff,
                                            std::function<float(float,float)> surface_func);
Frame3D<GLfloat> makeQuadraticSurfaceFrame(size_t dim_size, float cutoff,
                                           std::function<float(float,float,float)> surface_func);
Frame3D<GLfloat> makeParaboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHelixFrame(size_t dim_size, float cutoff);

std::vector<QVector3D> makeRainbowPalette();

template <typename Func>
std::vector<GLfloat> makeOpacityPalette(size_t size, Func func) {
    std::vector<GLfloat> values;
    for (size_t i = 0; i <= size; i++) {
        values.push_back(func(GLfloat(i)/size));
    }
    return values;
}
