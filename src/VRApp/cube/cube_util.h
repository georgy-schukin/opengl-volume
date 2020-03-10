#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>

namespace cube {

class CubeData;

Frame3D<GLfloat> cubeToframe(const CubeData &data);

Frame3D<GLfloat> loadCube(const std::string &filename);

}
