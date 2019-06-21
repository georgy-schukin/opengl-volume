#include "hemisphere.h"

#include <cmath>

namespace {
    const float PI = std::acos(-1.0f);
    const float PI_HALF = PI*0.5f;

    QVector3D point(float radius, float phi, float theta) {
        const auto x = radius*std::cos(phi)*std::cos(theta);
        const auto y = radius*std::cos(phi)*std::sin(theta);
        const auto z = radius*std::sin(phi);
        return QVector3D(x, y, radius - z);
    }
}

HemiSphere::HemiSphere(float radius, int num_of_segments, int num_of_rings) {
    std::vector<QVector3D> vertices;
    std::vector<GLuint> indices;

    // Pole point.
    vertices.push_back(QVector3D(0.0f, 0.0f, 0.0f));

    const float phi_step = PI_HALF/num_of_rings;
    const float theta_step = 2.0f*PI/num_of_segments;

    // Triangle fan around the pole (the first ring).
    for (int i = 0; i < num_of_segments; i++) {
        const auto phi = PI_HALF - phi_step;
        const auto theta = i*theta_step;
        vertices.push_back(point(radius, phi, theta));
    }

    for (int i = 0; i < num_of_segments; i++) {
        indices.push_back(GLuint(i) + 1);
        indices.push_back(GLuint((i + 1) % num_of_segments) + 1);
        indices.push_back(0);
    }

    // Another rings.
    for (int r = 2; r <= num_of_rings; r++) {
        const auto phi = PI_HALF - r*phi_step;
        const auto vertices_prev_size = GLuint(vertices.size());
        for (int i = 0; i < num_of_segments; i++) {
            const auto theta = i*theta_step;
            vertices.push_back(point(radius, phi, theta));
        }

        for (int i = 0; i < num_of_segments; i++) {
            const auto ind = vertices_prev_size + GLuint(i);
            const auto ind_next = vertices_prev_size + GLuint((i + 1) % num_of_segments);
            const auto ind_prev = ind - num_of_segments;
            const auto ind_prev_next = ind_next - num_of_segments;

            indices.push_back(ind);
            indices.push_back(ind_next);
            indices.push_back(ind_prev);

            indices.push_back(ind_next);
            indices.push_back(ind_prev_next);
            indices.push_back(ind_prev);
        }
    }

    setVertices(vertices, GL_FLOAT, 3);
    setIndices(indices, GL_UNSIGNED_INT);
}
