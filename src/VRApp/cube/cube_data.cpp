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

CubeData readCubeFile(const std::string &filename) {
    std::ifstream cubfile(filename.c_str());

    if (!cubfile.is_open()) {
        throw std::runtime_error("Cannot open " + filename);
    }

    CubeData data;

    std::getline(cubfile, data.title[0]);
    std::getline(cubfile, data.title[1]);

    int num_of_atoms = 0;
    cubfile >> num_of_atoms;

    data.is_in_angstrom = (num_of_atoms < 0);
    num_of_atoms = std::abs(num_of_atoms);

    cubfile >> data.origin;
    for (size_t i = 0; i < 3; i++) {
        cubfile >> data.dim[i] >> data.dvec[i];
    }

    if (!data.is_in_angstrom) {
        for (size_t i = 0; i < 3; i++) {
            data.origin[i] *= ANGSTROMS_IN_BOHR;
            data.dvec[i][0] *= ANGSTROMS_IN_BOHR;
            data.dvec[i][1] *= ANGSTROMS_IN_BOHR;
            data.dvec[i][2] *= ANGSTROMS_IN_BOHR;
        }
    }
    /*if (verbosity > 0)
    {
        cout << data.nat << " atoms, origin @ (" <<
                            data.orig[0] << ", " <<
                            data.orig[1] << ", " <<
                            data.orig[2] << "), Angstrom" << endl;
        cout <<" Dimensions " << data.dim[0] << " x " <<
                                 data.dim[1] << " x " <<
                                 data.dim[2] << "; " << endl;
        cout <<" V1 = (" << data.dvec[0][0] << ", " <<
                            data.dvec[0][1] << ", " <<
                            data.dvec[0][2] << "), Angstrom" << endl;
        cout <<" V2 = (" << data.dvec[1][0] << ", " <<
                            data.dvec[1][1] << ", " <<
                            data.dvec[1][2] << "), Angstrom" << endl;
        cout <<" V3 = (" << data.dvec[2][0] << ", " <<
                            data.dvec[2][1] << ", " <<
                            data.dvec[2][2] << "), Angstrom" << endl;
    }*/

    data.atoms.resize(static_cast<size_t>(num_of_atoms));
    for (size_t i = 0; i < data.atoms.size(); i ++) {
        cubfile >> data.atoms[i].n
                >> data.atoms[i].unk
                >> data.atoms[i].rc;

        if (!data.is_in_angstrom) {
            data.atoms[i].rc[0] *= ANGSTROMS_IN_BOHR;
            data.atoms[i].rc[1] *= ANGSTROMS_IN_BOHR;
            data.atoms[i].rc[2] *= ANGSTROMS_IN_BOHR;
        }
        /*if (verbosity > 0) cout << "atom #" << i+1 << ": " << data.atoms[i].n << " "
                                                           << data.atoms[i].unk << " "
                                                           << data.atoms[i].rc[0] << ", "
                                                           << data.atoms[i].rc[1] << ", "
                                                           << data.atoms[i].rc[2] << ", (A)" << endl;*/
    }

    const size_t datanum = static_cast<size_t>(data.dim[0] * data.dim[1] * data.dim[2]);
    data.data.resize(datanum);

    //cout << "Reading " << datanum/1000 << "k data block... (* = " << STAR_NUM/1000 << "k values,  " <<
     //       datanum/STARLINE_NUM << "+ * lines)" << endl;

    for (size_t i = 0; i < data.data.size(); i++) {
        cubfile >> data.data[i];
    }

    cubfile.close();

    return data;
}

void writeCubeFile(const CubeData &data, const std::string &filename) {

}

}
