#include "frame_loader.h"

#include <fstream>
#include <sstream>

namespace {

template <ValueType Type>
Frame3D<GLfloat> readFrame(std::ifstream &in, size_t width, size_t height, size_t depth) {
    using InputType = typename ValueTypeSelect<Type>::type;
    const auto size = width * height * depth;
    if (size == 0) {
        std::ostringstream out;
        out << "Bad data size: " << width << " x " << height << " x " << depth;
        throw std::runtime_error(out.str());
    }
    std::vector<InputType> values(size);
    in.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        in.read(reinterpret_cast<char *>(values.data()), static_cast<std::streamsize>(values.size() * sizeof(InputType)));
    }
    catch (const std::ifstream::failure &e) {
        throw std::runtime_error(std::string("Failed to read data: ") + e.what());
    }

    Frame3D<GLfloat> frame(width, height, depth);
    frame.fill(values.begin(), values.end()); // load data 'as is' into frame
    frame.normalize();
    return frame;
}

}

Frame3D<GLfloat> FrameLoader::load(const std::string &filename) {
    std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }
    unsigned char type;
    unsigned short width, height, depth;
    in.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        in.read(reinterpret_cast<char *>(&type), sizeof(type));
        in.read(reinterpret_cast<char *>(&width), sizeof(width));
        in.read(reinterpret_cast<char *>(&height), sizeof(height));
        in.read(reinterpret_cast<char *>(&depth), sizeof(depth));
    }
    catch (const std::ifstream::failure &e) {
        throw std::runtime_error(std::string("Failed to read data: ") + e.what());
    }
    return loadBinary(in, width, height, depth, static_cast<ValueType>(type));
}

Frame3D<GLfloat> FrameLoader::loadRaw(const std::string &filename, size_t width, size_t height, size_t depth, ValueType type) {
    std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }
    return loadBinary(in, width, height, depth, type);
}

Frame3D<GLfloat> FrameLoader::loadBinary(std::ifstream &in, size_t width, size_t height, size_t depth, ValueType type) {
    switch (type) {
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
        throw std::runtime_error("Unknown data type: " + std::to_string((int)type));
    }
}
