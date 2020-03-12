#include "cube_data.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>

namespace cube {

const double ANGSTROMS_IN_BOHR = 0.529177249;

std::istream& operator>> (std::istream &in, Vector &v) {
    return in >> v[0] >> v[1] >> v[2];
}

std::ostream& operator<< (std::ostream &out, const Vector &v) {
    return out << v[0] << v[1] << v[2];
}

namespace {
    //Vector toBohr
}

CubeData readCubeFile(const std::string &filename) {
    std::ifstream cubfile(filename.c_str());

    if (!cubfile.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }

    CubeData data;

    std::getline(cubfile, data.title[0]);
    std::getline(cubfile, data.title[1]);

    int n_atoms = 0;
    cubfile >> n_atoms;

    data.is_in_angstrom = (n_atoms < 0);
    const size_t num_of_atoms = static_cast<size_t>(std::abs(n_atoms));

    cubfile >> data.origin;
    for (size_t i = 0; i < 3; i++) {
        cubfile >> data.dim[i] >> data.axis[i];
    }

    if (!data.is_in_angstrom) {
        for (size_t i = 0; i < 3; i++) {
            data.origin[i] *= ANGSTROMS_IN_BOHR;
            data.axis[i][0] *= ANGSTROMS_IN_BOHR;
            data.axis[i][1] *= ANGSTROMS_IN_BOHR;
            data.axis[i][2] *= ANGSTROMS_IN_BOHR;
        }
    }

    data.atoms.resize(num_of_atoms);
    for (size_t i = 0; i < data.atoms.size(); i ++) {
        cubfile >> data.atoms[i].number
                >> data.atoms[i].charge
                >> data.atoms[i].center;
        if (!data.is_in_angstrom) {
            data.atoms[i].center[0] *= ANGSTROMS_IN_BOHR;
            data.atoms[i].center[1] *= ANGSTROMS_IN_BOHR;
            data.atoms[i].center[2] *= ANGSTROMS_IN_BOHR;
        }
    }

    const size_t data_size = data.dim[0] * data.dim[1] * data.dim[2];
    data.data.resize(data_size);
    for (size_t i = 0; i < data.data.size(); i++) {
        cubfile >> data.data[i];
    }

    cubfile.close();

    return data;
}

void writeCubeFile(const CubeData &data, const std::string &filename) {
    std::ofstream cubfile(filename.c_str(), std::ios_base::out | std::ios_base::trunc);

    if (!cubfile.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }

    cubfile << data.title[0] << std::endl;
    cubfile << data.title[1] << std::endl;

    cubfile.close();
}

}
