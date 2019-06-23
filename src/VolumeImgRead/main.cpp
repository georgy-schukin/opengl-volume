#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <QApplication>
#include <QImage>
#include <QLabel>

using namespace std;

int main(int argc, char **argv) {
    QString path = argv[1];
    size_t width = std::stoi(argv[2]);
    size_t height = std::stoi(argv[3]);
    size_t depth = std::stoi(argv[4]);
    std::string output_file = argv[5];

    std::ofstream out(output_file.c_str(), std::ios_base::out | std::ios_base::binary);

    out.write(reinterpret_cast<const char *>(&width), sizeof(width));
    out.write(reinterpret_cast<const char *>(&height), sizeof(height));
    out.write(reinterpret_cast<const char *>(&depth), sizeof(depth));

    for (size_t z = 1; z <= depth; z++) {
        auto num = QString::number(z).rightJustified(3, '0');
        auto input_file = path.replace("@", num);
        QImage img(input_file);
        QImage converted = img.convertToFormat(QImage::Format_Grayscale16, Qt::MonoOnly);

        out.write(reinterpret_cast<const char *>(converted.bits()), converted.sizeInBytes());
    }

    out.close();

    std::cout << "Done " << output_file << std::endl;

    return 0;
}
