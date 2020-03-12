#include "cube_util.h"
#include "cube_data.h"

#include <cassert>

namespace cube {

Frame3D<GLfloat> cubeToframe(const CubeData &data) {
    const auto dx = data.dim[0];
    const auto dy = data.dim[1];
    const auto dz = data.dim[2];
    // Cube(dx, dy, dz) data layout - x,y,z: dx slices of dy*dz arrays.
    // Frame(dx, dy, dz) data layout - z,x,y: dz slices of dx*dy arrays.
    Frame3D<GLfloat> frame(dx, dy, dz);
    for (size_t i = 0; i < dx; i++) {
        for (size_t j = 0; j < dy; j++) {
            for (size_t k = 0; k < dz; k++) {
                const auto index = i*dy*dz + j*dz + k; // frame(i, j, k) = cube(i, j, k)
                assert (index < data.data.size());
                frame.at(i, j, k) = static_cast<GLfloat>(data.data[index]);
            }
        }
    }
    frame.normalize();
    return frame;
}

Frame3D<GLfloat> loadCube(const std::string &filename) {
    const auto data = readCubeFile(filename);
    return cubeToframe(data);
}

}
