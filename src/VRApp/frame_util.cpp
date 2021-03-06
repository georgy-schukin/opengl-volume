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
    T lerp2(T start, T end, U u_start, U u_end, U u_value) {
        const T ind = T(u_value - u_start)/(u_end - u_start);
        return start*(T(1.0) - ind) + ind*end;
    }

    bool lessOrEqual(double a, double b) {
        return (a < b) || (std::abs(a - b) < 1e-8);
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
                return lerp2(vs, ve, rs, re, value);
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

    double fade(double t) {
        return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
    }

    double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    double grad(size_t hash, double x, double y, double z) {
      const auto h = hash & 15;
      double u = h < 8 ? x : y,
             v = h < 4 ? y : h == 12||h == 14 ? x : z;
      return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

    double noise(double x, double y, double z, const std::vector<size_t> &perm) {
        const auto X = static_cast<size_t>(floor(x)) % perm.size();
        const auto Y = static_cast<size_t>(floor(y)) % perm.size();
        const auto Z = static_cast<size_t>(floor(z)) % perm.size();

        x -= floor(x);
        y -= floor(y);
        z -= floor(z);

        const auto u = fade(x);
        const auto v = fade(y);
        const auto w = fade(z);

        const auto A =  (perm[X] + Y) % perm.size();
        const auto AA = (perm[A] + Z) % perm.size();
        const auto AB = (perm[(A + 1) % perm.size()] + Z) % perm.size();
        const auto B =  (perm[(X + 1) % perm.size()] + Y) % perm.size();
        const auto BA = (perm[B] + Z) % perm.size();
        const auto BB = (perm[(B + 1) % perm.size()] + Z) % perm.size();

        return lerp(w, lerp(v, lerp(u, grad(perm[AA], x, y, z),
                                       grad(perm[BA], x - 1, y, z)),
                               lerp(u, grad(perm[AB], x, y - 1, z),
                                       grad(perm[BB], x - 1, y - 1, z))),
                       lerp(v, lerp(u, grad(perm[(AA + 1) % perm.size()], x, y, z - 1),
                                       grad(perm[(BA + 1) % perm.size()], x - 1, y, z - 1)),
                               lerp(u, grad(perm[(AB + 1) % perm.size()], x, y - 1, z - 1),
                                       grad(perm[(BB + 1) % perm.size()], x - 1, y - 1, z - 1))));
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
        const auto x = double(i) / (frame.width() - 1);
        const auto y = double(j) / (frame.height() - 1);
        const auto z = double(k) / (frame.depth() - 1);
        return std::sqrt(x*x + y*y + z*z) / std::sqrt(3.0);
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
            return 0.0;
        }
        const auto dist = std::sqrt(x*x + y*y + z*z);
        return lessOrEqual(dist, 1.0) ? 1.0 - dist : 0.0;
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
        return lessOrEqual(diff, cutoff) ? 1.0 - diff/cutoff : 0.0;
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
        return lessOrEqual(diff, cutoff) ? 1.0 - diff/cutoff : 0.0;
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
        return std::min(value, 1.0);
    });
    return frame;
}

Frame3D<GLfloat> makePerlinNoiseFrame(size_t dim_size, double freq) {
    std::vector<size_t> permutations;
    for (size_t i = 0; i < 256; i++) {
        permutations.push_back(i);
    }
    std::random_shuffle(permutations.begin(), permutations.end());

    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        const auto x = double(i) * freq;
        const auto y = double(j) * freq;
        const auto z = double(k) * freq;
        return noise(x, y, z, permutations);
    });
    return frame;
}

Frame3D<GLfloat> makePerlinNoiseOctavesFrame(size_t dim_size, int steps, double start_freq, double start_ampl) {
    std::vector<size_t> permutations;
    for (size_t i = 0; i < 256; i++) {
        permutations.push_back(i);
    }
    std::random_shuffle(permutations.begin(), permutations.end());

    Frame3D<GLfloat> frame(dim_size, dim_size, dim_size);
    frame.fill([&](size_t i, size_t j, size_t k) -> auto {
        double value = 0.0;
        const auto x = double(i) * start_freq;
        const auto y = double(j) * start_freq;
        const auto z = double(k) * start_freq;
        for (int n = 0; n < steps; n++) {
            const auto coeff = n + 1;
            value += start_ampl * noise(x * coeff, y * coeff, z * coeff, permutations) / coeff;
        }
        return std::min(1.0, value);
    });
    return frame;
}
