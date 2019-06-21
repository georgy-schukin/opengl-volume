#pragma once

#include "triangulated_shape.h"

class HemiSphere : public TriangulatedShape {
public:
    HemiSphere(float radius, int num_of_sectors, int num_of_rings);
};
