#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

int main(int argc, char **argv) {

    std::string path = argv[1];
    size_t dim_x = std::stoi(argv[2]);
    size_t dim_y = std::stoi(argv[3]);
    size_t dim_z = std::stoi(argv[4]);
    std::string output_file = argv[5];

    std::ofstream out(output_file.c_str(), std::ios_base::out | std::ios_base::binary);

    out.write(reinterpret_cast<const char *>(&dim_x), sizeof(dim_x));
    out.write(reinterpret_cast<const char *>(&dim_y), sizeof(dim_y));
    out.write(reinterpret_cast<const char *>(&dim_z), sizeof(dim_z));

    // File should contain 2 bytes for each value.
    std::vector<char> block(2*dim_x*dim_y);

    for (size_t z = 1; z <= dim_z; z++) {
        auto input_file = path + std::to_string(z);
        std::ifstream in(input_file.c_str(), std::ios_base::in | std::ios_base::binary);
        in.read(block.data(), block.size());
        in.close();

        // Bytes are in Mac order. Swap bytes.
        for (size_t n = 0; n < dim_x*dim_y; n++) {
            std::swap(block[2*n], block[2*n + 1]);
        }

        out.write(block.data(), block.size());
    }

    out.close();

    return 0;
}
