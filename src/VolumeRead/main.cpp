#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv) {

    const std::string path = argv[1];
    const size_t width = std::stoi(argv[2]);
    const size_t height = std::stoi(argv[3]);
    const size_t depth = std::stoi(argv[4]);
    const std::string output_file = argv[5];
    const int swap = (argc > 6 ? std::stoi(argv[6]) : 0);

    std::ofstream out(output_file.c_str(), std::ios_base::out | std::ios_base::binary);

    out.write(reinterpret_cast<const char *>(&width), sizeof(width));
    out.write(reinterpret_cast<const char *>(&height), sizeof(height));
    out.write(reinterpret_cast<const char *>(&depth), sizeof(depth));

    // File should contain 2 bytes for each value.
    std::vector<char> block(2*width*height);

    for (size_t z = 1; z <= depth; z++) {
        auto input_file = path + std::to_string(z);
        std::ifstream in(input_file.c_str(), std::ios_base::in | std::ios_base::binary);
        in.read(block.data(), block.size());
        in.close();

        // Bytes are in DEC Vax byte order. Swap bytes(?).
        if (swap) {
            for (size_t n = 0; n < width*height; n++) {
                std::swap(block[2*n], block[2*n + 1]);
            }
        }

        out.write(block.data(), block.size());
    }

    out.close();

    std::cout << "Done " << output_file << std::endl;

    return 0;
}
