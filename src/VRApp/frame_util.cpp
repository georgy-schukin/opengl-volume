#include "frame_util.h"
#include "../common/types.h"

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

    bool lessOrEqual(float a, float b) {
        return (a < b) || (std::abs(a - b) < 1e-6f);
    }

    bool lessOrEqual(double a, double b) {
        return (a < b) || (std::abs(a - b) < 1e-6);
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

    template <ValueType Type>
    Frame3D<GLfloat> readFrame(std::ifstream &in, size_t width, size_t height, size_t depth) {
        using InputType = typename ValueTypeSelect<Type>::type;
        std::vector<InputType> values(width*height*depth);
        in.read(reinterpret_cast<char *>(values.data()), static_cast<int>(values.size()*sizeof(InputType)));

        Frame3D<GLfloat> frame(width, height, depth);
        frame.fill(values.begin(), values.end()); // load data 'as is' into frame
        frame.normalize();
        return frame;
    }
}

Frame3D<GLfloat> loadFrameFromFile(const std::string &filename) {
    std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    unsigned char type;
    unsigned short width, height, depth;
    in.read(reinterpret_cast<char *>(&type), sizeof(type));
    in.read(reinterpret_cast<char *>(&width), sizeof(width));
    in.read(reinterpret_cast<char *>(&height), sizeof(height));
    in.read(reinterpret_cast<char *>(&depth), sizeof(depth));

    const auto value_type = static_cast<ValueType>(type);

    switch (value_type) {
    case ValueType::VT_INT8:
        return readFrame<ValueType::VT_INT8>(in, width, height, depth);
    case ValueType::VT_UINT8:
        return readFrame<ValueType::VT_UINT8>(in, width, height, depth);
    case ValueType::VT_INT16:
        return readFrame<ValueType::VT_INT16>(in, width, height, depth);
    case ValueType::VT_UINT16:
        return readFrame<ValueType::VT_UINT16>(in, width, height, depth);
    case ValueType::VT_INT32:
        return readFrame<ValueType::VT_UINT32>(in, width, height, depth);
    case ValueType::VT_UINT32:
        return readFrame<ValueType::VT_UINT32>(in, width, height, depth);
    case ValueType::VT_FLOAT:
        return readFrame<ValueType::VT_FLOAT>(in, width, height, depth);
    default:
        throw std::runtime_error("Unknown value type: " + std::to_string(type));
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
        const auto x = float(i)/float(frame.width() - 1);
        const auto y = float(j)/float(frame.height() - 1);
        const auto z = float(k)/float(frame.depth() - 1);
        return std::sqrt(x*x + y*y + z*z)/std::sqrt(3.0f);
    });
    return frame;
}

Frame3D<GLfloat> makeSphereFrame(size_t dim_size) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    const auto coeff = 2.0 / (dim_size - 1);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0 - double(i) * coeff;
        const auto y = 1.0 - double(j) * coeff;
        const auto z = 1.0 - double(k) * coeff;
        if (z < 0.0) {
            return 0.0f;
        }
        const auto dist = std::sqrt(x*x + y*y + z*z);
        return lessOrEqual(dist, 1.0) ? float(1.0 - dist) : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeAnalyticalSurfaceFrame(size_t dim_size, double cutoff,
                                            std::function<double (double, double)> surface_func) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    const auto coeff = 2.0 / (dim_size - 1);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0 - double(i) * coeff;
        const auto y = 1.0 - double(j) * coeff;
        const auto z = 1.0 - double(k) * coeff;
        const auto func_value = surface_func(x, y);
        const auto diff = std::abs(z - func_value);
        return lessOrEqual(diff, cutoff) ? float(1.0 - diff/cutoff) : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeImplicitSurfaceFrame(size_t dim_size, double cutoff,
                                           std::function<double (double, double, double)> surface_func) {
    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    const auto coeff = 2.0/(dim_size - 1);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = 1.0 - double(i) * coeff;
        const auto y = 1.0 - double(j) * coeff;
        const auto z = 1.0 - double(k) * coeff;
        const auto func_value = surface_func(x, y, z);
        // Diff should be zero if the point lies on the surface.
        const auto diff = std::abs(func_value);
        return lessOrEqual(diff, cutoff) ? float(1.0 - diff/cutoff) : 0.0f;
    });
    return frame;
}

Frame3D<GLfloat> makeParaboloidFrame(size_t dim_size, double cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](auto x, auto y, auto z) {
        return x*x + y*y - z;
    });
}

Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, double cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](auto x, auto y, auto z) {
        return 2.0*x*x + 2.0*y*y - 2.0*z*z - 1.0;
    });
}

Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, double cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](auto x, auto y, auto z) {
        return x*x - y*y - z;
    });
}

Frame3D<GLfloat> makeHelixFrame(size_t dim_size, double cutoff, double R, double r, double a) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [R, r, a](auto x, auto y, auto z) {
        const auto xr = x - R * std::cos(z*a);
        const auto yr = y - R * std::sin(z*a);
        return xr*xr + yr*yr - r*r;
    });
}

Frame3D<GLfloat> makeHelicoidFrame(size_t dim_size, double cutoff) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [](auto x, auto y, auto z) {
        return x*std::sin(z*4.0) - y*std::cos(z*4.0);
    });
}

Frame3D<GLfloat> makeTorusFrame(size_t dim_size, double cutoff, double R, double r) {
    return makeImplicitSurfaceFrame(dim_size, cutoff, [R, r](auto x, auto y, auto z) {
        const auto tmp = (x*x + y*y + z*z + R*R - r*r);
        return tmp*tmp - 4*R*R*(x*x + y*y);
    });
}

Frame3D<GLfloat> makeBubblesFrame(size_t dim_size, size_t num_of_bubbles, double min_rad, double max_rad) {
    //std::random_device rd;
    std::mt19937 mt(static_cast<unsigned int>(time(nullptr)));
    std::uniform_real_distribution<double> rad_distribution(min_rad, max_rad);
    std::uniform_real_distribution<double> pos_distribution(0.0f + max_rad, 1.0f - max_rad);

    std::vector<std::pair<QVector3D, double>> bubbles;
    for (size_t i = 0; i < num_of_bubbles; i++) {
        const auto pos = QVector3D(pos_distribution(mt), pos_distribution(mt), pos_distribution(mt));
        const auto radius = rad_distribution(mt);
        bubbles.push_back(std::make_pair(pos, radius));
    }

    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    const auto coeff = 1.0 / (dim_size - 1);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = double(i) * coeff;
        const auto y = double(j) * coeff;
        const auto z = double(k) * coeff;
        const QVector3D point(x, y, z);
        double value = 0.0;
        for (const auto &p: bubbles) {
            const auto &b_pos = p.first;
            const auto &rad = p.second;
            const auto dist = (double)b_pos.distanceToPoint(point);
            if (lessOrEqual(dist, rad)) {
                value += 1.0 - dist/rad;
            }
        }
        return (float)std::min(value, 1.0);
    });
    return frame;
}



