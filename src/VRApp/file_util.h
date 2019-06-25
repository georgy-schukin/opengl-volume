#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <string>

Frame3D<GLfloat> loadFrameFromFile(const std::string &filename);
