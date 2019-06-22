#pragma once

#include <vector>
#include <cstddef>
#include <algorithm>
#include <omp.h>

template <class T>
class Frame3D {
public:
    Frame3D(size_t width, size_t height, size_t depth=1) :
        _width(width), _height(height), _depth(depth)
    {
        _data.resize(_width*_height*_depth);
    }

    T* data() {
        return &_data[0];
    }

    const T* data() const {
        return &_data[0];
    }

    size_t size() const {
        return _width*_height*_depth;
    }

    size_t width() const {
        return _width;
    }

    size_t height() const {
        return _height;
    }

    size_t depth() const {
        return _depth;
    }

    template <typename Func>
    void fill(Func func) {
        #pragma omp parallel for
        for (size_t i = 0; i < _width; i++) {
            for (size_t j = 0; j < _height; j++) {
                for (size_t k = 0; k < _depth; k++) {
                    at(i, j, k) = func(i, j, k);
                }
            }
        }
    }

    void fillBy(const T &value) {
        std::fill(_data.begin(), _data.end(), value);
    }

    void normalize() {
        const auto max = *std::max_element(_data.begin(), _data.end());
        for (auto &el: _data) {
            el /= max;
        }
    }

    T& at(size_t x, size_t y, size_t z) {
        return _data[x*_height*_depth + y*_depth + z];
    }

private:
    size_t _width, _height, _depth;
    typename std::vector<T> _data;
};
