#pragma once

#include <string>
#include <vector>
#include <array>

namespace cube {

using Vector = std::array<double, 3>;

std::istream& operator>> (std::istream &in, Vector &v);
std::ostream& operator<< (std::ostream &out, const Vector &v);

class AtomData {
public:
    int n = 0; // number of an element in PTE
    double unk = 0.0; // unknown item
    Vector rc = {}; // atresian coordinates in Angstrom [x..z]
};

class CubeData {
public:
    std::string title[2];
    bool is_in_angstrom = true; // true when in Angstrom, false when in Bhores
    size_t dim[3] = {0, 0, 0}; // cube dimensions
    Vector origin = {}; // cube origin
    Vector dvec[3] = {}; // elementary vectors [dimensions: v1, v2, v3][coordinates of vi: x, y, z]
    bool bord_period[3] = {true, true, true}; // [v1, v2, v3] borders periodicity
    std::vector<AtomData> atoms;
    std::vector<double> data;
    //double valmin = 0.0, valmax = 0.0, valave = 0.0; // statistics on data values
};

CubeData readCubeFile(const std::string &filename);
void writeCubeFile(const CubeData &data, const std::string &filename);

}
