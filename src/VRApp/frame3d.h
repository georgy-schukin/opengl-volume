#pragma once

#include <vector>
#include <cstddef>

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
        for (size_t i = 0; i < _width; i++)
            for (size_t j = 0; j < _height; j++)
                for (size_t k = 0; k < _depth; k++) {
                    _data[i*_height*_depth + j*_depth + k] = func(i, j, k);
                }
    }

private:
    size_t _width, _height, _depth;
    typename std::vector<T> _data;
};
