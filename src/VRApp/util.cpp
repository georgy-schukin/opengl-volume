#include "util.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>
#include <vector>
#include <ctime>
#include <fstream>

namespace  {
    template <typename T, typename U>
    T lerp(T start, T end, U u_start, U u_end, U u_value) {
        const T ind = T(u_value - u_start)/(u_end - u_start);
        return start*(T(1.0) - ind) + ind*end;
    }

    GLfloat hounsfield(int value) {
        const static std::vector<std::tuple<int, int, GLfloat, GLfloat>> ranges = {
            /*{13, 50, 0.5f, 0.6f},
            {50, 75, 0.6f, 0.7f},
            {100, 300, 0.4f, 0.5f},
            {300, 400, 0.9f, 1.0f},
            {1800, 1900, 0.9f, 1.0f},
            {1900, 4000, 0.2f, 0.3f},*/
            std::make_tuple(900, 1200, 0.8f, 1.0f),
            std::make_tuple(1200, 2000, 0.3f, 0.5f),
            std::make_tuple(2000, 4000, 0.2f, 0.3f),
            std::make_tuple(0, 1200, 0.01f, 0.1f)
        };
        for (const auto &t: ranges) {
            const auto rs = std::get<0>(t);
            const auto re = std::get<1>(t);
            const auto vs = std::get<2>(t);
            const auto ve = std::get<3>(t);
            if (value >= rs && value < re) {
                return lerp(vs, ve, rs, re, value);
            }
        }
        return 0.0f;
    }

    template <typename T>
    typename std::map<std::pair<int, int>, int> histogramm(const typename std::vector<T> &values, int num_of_buckets) {
        const auto max = *std::max_element(values.begin(), values.end());
        const auto buck_size = std::ceil(float(max)/num_of_buckets);

        typename std::map<std::pair<int, int>, int> histo;
        for (int i = 0; i < num_of_buckets; i++) {
            histo[std::make_pair(i*buck_size, (i + 1)*buck_size)] = 0;
        }

        for (const auto &v: values) {
            const auto b = std::floor(v/buck_size)*buck_size;
            auto buck = std::make_pair(b, b + buck_size);
            histo[buck]++;
        }
        return histo;
    }
}

Frame3D<GLfloat> makeRandomFrame(size_t dim_size) {
    //std::random_device rd;
    std::mt19937 mt(static_cast<unsigned int>(time(nullptr)));
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
        const auto x = float(i)/(frame.width() - 1);
        const auto y = float(j)/(frame.height() - 1);
        const auto z = float(k)/(frame.depth() - 1);
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

Frame3D<GLfloat> makeImplicitSurfaceFrame(size_t dim_size, float cutoff,
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
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return x*x + y*y - z;
    });
}

Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, float cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return 2.0f*x*x + 2.0f*y*y - 2.0f*z*z - 1.0f;
    });
}

Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, float cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return x*x - y*y - z;
    });
}

Frame3D<GLfloat> makeHelixFrame(size_t dim_size, float cutoff, float R, float r, float a) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [R, r, a](float x, float y, float z) {
        const auto xr = x - R*std::cos(z*a);
        const auto yr = y - R*std::sin(z*a);
        return xr*xr + yr*yr - r*r;
    });
}

Frame3D<GLfloat> makeHelicoidFrame(size_t dim_size, float cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](float x, float y, float z) {
        return x*std::sin(z*4.0f) - y*std::cos(z*4.0f);
    });
}

Frame3D<GLfloat> makeTorusFrame(size_t dim_size, float cutoff, float R, float r) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [R, r](float x, float y, float z) {
        const auto tmp = (x*x + y*y + z*z + R*R - r*r);
        return tmp*tmp - 4*R*R*(x*x + y*y);
    });
}

Frame3D<GLfloat> makeBubblesFrame(size_t dim_size, size_t num_of_bubbles, float min_rad, float max_rad) {
    //std::random_device rd;
    std::mt19937 mt(static_cast<unsigned int>(time(nullptr)));
    std::uniform_real_distribution<GLfloat> rad_distribution(min_rad, max_rad);
    std::uniform_real_distribution<GLfloat> pos_distribution(0.0f + max_rad, 1.0f - max_rad);

    std::vector<std::pair<QVector3D, float>> bubbles;
    for (size_t i = 0; i < num_of_bubbles; i++) {
        const auto pos = QVector3D(pos_distribution(mt), pos_distribution(mt), pos_distribution(mt));
        const auto radius = rad_distribution(mt);
        bubbles.push_back(std::make_pair(pos, radius));
    }

    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = float(i)/(dim_size - 1);
        const auto y = float(j)/(dim_size - 1);
        const auto z = float(k)/(dim_size - 1);
        const QVector3D point(x, y, z);
        GLfloat value = 0.0f;
        for (const auto &p: bubbles) {
            const auto &b_pos = p.first;
            const auto &rad = p.second;
            const auto dist = b_pos.distanceToPoint(point);
            if (dist <= rad) {
                value += 1.0f - dist/rad;
            }
        }
        return std::min(value, 1.0f);
    });
    return frame;
}

Frame3D<GLfloat> loadFrameFromFile(const std::string &filename) {
    std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    unsigned short w, h, d;
    in.read(reinterpret_cast<char *>(&w), sizeof(w));
    in.read(reinterpret_cast<char *>(&h), sizeof(h));
    in.read(reinterpret_cast<char *>(&d), sizeof(d));

    const auto width = static_cast<size_t>(w);
    const auto height = static_cast<size_t>(h);
    const auto depth = static_cast<size_t>(d);

    // We have 2 bytes per voxel's value.
    using VoxelType = short;
    std::vector<VoxelType> values(width*height*depth);
    in.read(reinterpret_cast<char *>(values.data()), static_cast<int>(values.size()*sizeof(VoxelType)));
    in.close();

    const auto max = *std::max_element(values.begin(), values.end());
    const auto min = *std::min_element(values.begin(), values.end());
    const auto avg = double(std::accumulate(values.begin(), values.end(), 0.0)) / values.size();

    Frame3D<GLfloat> frame(width, height, depth);
    // Slices are arranged by depth.
    #pragma omp parallel for
    for (size_t k = 0; k < depth; k++) {
        for (size_t i = 0; i < width; i++) {
            for (size_t j = 0; j < height; j++) {
                const auto index = frame.index(i, j, k);
                // Normalize data.
                frame.at(i, j, k) = GLfloat(values[index])/(max);
            }
        }
    }
    return frame;
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

std::vector<QVector3D> makeMonochromePalette() {
    return {
        QVector3D(0.0f, 0.0f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f)
    };
}


