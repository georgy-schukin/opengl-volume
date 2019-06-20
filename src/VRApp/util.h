#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>

Frame3D<GLfloat> makeRandomFrame(size_t dim_size);
Frame3D<GLfloat> makeSectorFrame(size_t dim_size);
Frame3D<GLfloat> makeSphereFrame(size_t dim_size);

std::vector<QVector3D> makeRainbowPalette();
