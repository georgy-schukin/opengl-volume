#pragma once

#include "frame3d.h"
#include "../common/types.h"

#include <QOpenGLFunctions>
#include <string>
#include <cstddef>
#include <fstream>

class FrameLoader {
public:
    static Frame3D<GLfloat> load(const std::string &filename);
    static Frame3D<GLfloat> loadRaw(const std::string &filename, size_t width, size_t height, size_t depth, ValueType type);

private:
    static Frame3D<GLfloat> loadBinary(std::ifstream &in, size_t width, size_t height, size_t depth, ValueType type);
};
