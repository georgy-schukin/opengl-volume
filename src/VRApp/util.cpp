#include "util.h"

#include <fstream>

namespace util {

std::string readFile(const std::string &filename) {
    std::ifstream f_in(filename.c_str());

    if (!f_in.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }

    return std::string((std::istreambuf_iterator<char>(f_in)),
                        std::istreambuf_iterator<char>());
}

}
