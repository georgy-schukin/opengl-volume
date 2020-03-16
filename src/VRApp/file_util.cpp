#include "file_util.h"
#include "../common/types.h"

#include <fstream>

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
    default:
        throw std::runtime_error("Unknown value type: " + std::to_string(type));
    }
}
