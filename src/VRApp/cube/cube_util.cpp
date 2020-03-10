#include "cube_util.h"
#include "cube_data.h"

#include <cassert>

namespace cube {

Frame3D<GLfloat> cubeToframe(const CubeData &data) {
    const auto dx = data.dim[0];
    const auto dy = data.dim[1];
    const auto dz = data.dim[2];
    Frame3D<GLfloat> frame(dx, dy, dz);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto index = i*dx*dy + j*dx + k;
        assert (index < data.data.size());
        return static_cast<GLfloat>(data.data[index]);
    });
    frame.normalize();
    return frame;
}

Frame3D<GLfloat> loadCube(const std::string &filename) {
    const auto data = readCubeFile(filename);
    return cubeToframe(data);
}

}
