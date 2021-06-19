/*
 * Utility for converting raw volume data into .frame file format.
 * Frame format is (binary):
 * - Header: type (uchar) width height depth (ushort),
 * - Data: slices in depth-order, each slice is width*height elements.
*/

#include "../common/types.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

size_t getFileSize(const std::string &filename) {
    std::ifstream in(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }
    in.seekg(0, std::ios::end);
    return static_cast<size_t>(in.tellg());
}

ValueType getType(const std::string &type) {
    static const std::map<std::string, ValueType> types = {
        {"int8", ValueType::VT_INT8},
        {"uint8", ValueType::VT_UINT8},
        {"int16", ValueType::VT_INT16},
        {"uint16", ValueType::VT_UINT16},
        {"int32", ValueType::VT_INT32},
        {"uint32", ValueType::VT_UINT32},
        {"float32", ValueType::VT_FLOAT}
    };
    auto it = types.find(type);
    if (it != types.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Unknown type: " + type);
    }
}

int main(int argc, char **argv) {
    if (argc <= 6) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <input_type> <width> <height> <depth> <output_file>" << std::endl;
        return -1;
    }

    try {
        const std::string input_file = argv[1];
        const std::string type = argv[2];
        const size_t width = static_cast<size_t>(std::stoi(argv[3]));
        const size_t height = static_cast<size_t>(std::stoi(argv[4]));
        const size_t depth = static_cast<size_t>(std::stoi(argv[5]));
        const std::string output_file = argv[6];

        const auto size = getFileSize(input_file);

        std::ifstream in(input_file.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!in.is_open()) {
            throw std::runtime_error("Cannot open " + input_file);
        }

        std::vector<char> data(size);
        in.read(data.data(), static_cast<int>(data.size()));
        in.close();

        std::ofstream out(output_file.c_str(), std::ios_base::out | std::ios_base::binary);

        const unsigned char tp = static_cast<unsigned char>(getType(type));
        const unsigned short w = static_cast<unsigned short>(width);
        const unsigned short h = static_cast<unsigned short>(height);
        const unsigned short d = static_cast<unsigned short>(depth);
        out.write(reinterpret_cast<const char *>(&tp), sizeof(tp));
        out.write(reinterpret_cast<const char *>(&w), sizeof(w));
        out.write(reinterpret_cast<const char *>(&h), sizeof(h));
        out.write(reinterpret_cast<const char *>(&d), sizeof(d));
        out.write(data.data(), data.size());
        out.close();

        std::cout << "Done " << output_file << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
