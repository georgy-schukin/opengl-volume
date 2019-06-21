#include "util.h"

#include <algorithm>
#include <cmath>
#include <random>

Frame3D<GLfloat> makeRandomFrame(size_t dim_size) {
    std::default_random_engine generator;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<GLfloat> distribution(0.0f, 1.0f);
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t, size_t, size_t) -> auto {
        return distribution(mt);
    });
    return frame;
}

Frame3D<GLfloat> makeSectorFrame(size_t dim_size) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = float(i)/(dim_size - 1);
        const auto y = float(j)/(dim_size - 1);
        const auto z = float(k)/(dim_size - 1);
        return std::sqrt(x*x + y*y + z*z)/std::sqrt(3.0f);
    });
    return frame;
}

Frame3D<GLfloat> makeSphereFrame(size_t dim_size) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0f - 2.0f*float(i)/(dim_size - 1);
        const auto y = 1.0f - 2.0f*float(j)/(dim_size - 1);
        const auto z = 1.0f - 2.0f*float(k)/(dim_size - 1);
        if (z < 0.0f) {
            return 0.0f;
        }
        const auto dist = std::sqrt(x*x + y*y + z*z);
        return dist <= 1.0f ? 1.0f - dist : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeAnalyticalSurfaceFrame(size_t dim_size, float cutoff,
                                            std::function<float(float,float)> surface_func) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0f - 2.0f*float(i)/(dim_size - 1);
        const auto y = 1.0f - 2.0f*float(j)/(dim_size - 1);
        const auto z = 1.0f - 2.0f*float(k)/(dim_size - 1);
        const auto func_value = surface_func(x, y);
        const auto diff = std::abs(z - func_value);
        return diff <= cutoff ? 1.0f - diff/cutoff : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeQuadraticSurfaceFrame(size_t dim_size, float cutoff,
                                           std::function<float(float,float,float)> surface_func) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0f - 2.0f*float(i)/(dim_size - 1);
        const auto y = 1.0f - 2.0f*float(j)/(dim_size - 1);
        const auto z = 1.0f - 2.0f*float(k)/(dim_size - 1);
        const auto func_value = surface_func(x, y, z);
        // Diff should be zero if the point lies on the surface.
        const auto diff = std::abs(func_value);
        return diff <= cutoff ? 1.0f - diff/cutoff : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeParaboloidFrame(size_t dim_size, float cutoff) {
    return makeQuadraticSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return x*x + y*y - z;
    });
}

Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, float cutoff) {
    return makeQuadraticSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return 2.0f*x*x + 2.0f*y*y - 2.0f*z*z - 1.0f;
    });
}

Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, float cutoff) {
    return makeQuadraticSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return x*x - y*y - z;
    });
}

Frame3D<GLfloat> makeHelixFrame(size_t dim_size, float cutoff) {
    return makeQuadraticSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        const auto xr = x - 0.5f*std::cos(z*4.0f);
        const auto yr = y - 0.5f*std::sin(z*4.0f);
        return xr*xr + yr*yr - 0.2f*0.2f;
    });
}

std::vector<QVector3D> makeRainbowPalette() {
    const std::vector<QVector3D> rainbow = {
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(0.0f, 1.0f, 1.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(1.0f, 1.0f, 0.0f),
        QVector3D(1.0f, 0.0f, 0.0f)
    };
    /*std::vector<QVector3D> palette;
    for (size_t i = 0; i < rainbow.size() - 1; i++) {
        const auto c1 = rainbow[i];
        const auto c2 = rainbow[i + 1];
        for (int j = 0; j < 10; j++) {
            const float t = float(j)/10;
            palette.push_back(c1*(1 - t) + c2*t);
        }
    }
    return palette;*/
    return rainbow;
}
