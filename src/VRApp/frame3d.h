#pragma once

#include <vector>

template <class T>
class Frame3D {
public:
    Frame3D(int width, int height, int depth=1) :
        width(width), height(height), depth(depth)
    {
        data.resize(width*height*depth);
    }

    ~Frame3D() {}

    T* data() {
        return &data[0];
    }

    const T* data() const {
        return &data[0];
    }

    int size() const {
        return width*height*depth;
    }

private:
    int width, height, depth;
    typename std::vector<T> data;
};
