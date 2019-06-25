/*
 * Utility for converting Stanford volume data sets into .frame file format.
 * Frame format is (binary):
 * - Header: width height depth (unsigned short each),
 * - Data: slices in depth-order, width*height unsigned shorts each.
*/

#include "../common/types.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    if (argc <= 5) {
        std::cerr << "Usage: " << argv[0] << " <path> <width> <height> <depth> <output_file> <swap_bytes>" << std::endl;
        return -1;
    }

    const std::string path = argv[1];
    const size_t width = static_cast<size_t>(std::stoi(argv[2]));
    const size_t height = static_cast<size_t>(std::stoi(argv[3]));
    const size_t depth = static_cast<size_t>(std::stoi(argv[4]));
    const std::string output_file = argv[5];
    const int swap_bytes = (argc > 6 ? std::stoi(argv[6]) : 0);

    std::ofstream out(output_file.c_str(), std::ios_base::out | std::ios_base::binary);

    const unsigned char type = static_cast<unsigned char>(ValueType::VT_INT16);
    const unsigned short w = static_cast<unsigned short>(width);
    const unsigned short h = static_cast<unsigned short>(height);
    const unsigned short d = static_cast<unsigned short>(depth);
    out.write(reinterpret_cast<const char *>(&type), sizeof(type));
    out.write(reinterpret_cast<const char *>(&w), sizeof(w));
    out.write(reinterpret_cast<const char *>(&h), sizeof(h));
    out.write(reinterpret_cast<const char *>(&d), sizeof(d));

    // File should contain 2 bytes for each value.
    std::vector<char> block(2*width*height);

    for (size_t z = 1; z <= depth; z++) {
        auto input_file = path + std::to_string(z);
        std::ifstream in(input_file.c_str(), std::ios_base::in | std::ios_base::binary);
        if (!in.is_open()) {
            std::cerr << "Error: cannot open " << input_file << std::endl;
            continue;
        }
        in.read(block.data(), static_cast<int>(block.size()));
        in.close();

        // Bytes are in DEC Vax byte order. Swap bytes(?).
        if (swap_bytes) {
            for (size_t n = 0; n < width*height; n++) {
                std::swap(block[2*n], block[2*n + 1]);
            }
        }

        out.write(block.data(), static_cast<int>(block.size()));
    }

    out.close();

    std::cout << "Done " << output_file << std::endl;

    return 0;
}
