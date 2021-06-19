#include "cube.h"

Cube::Cube() {
    // Vertex coordinates.
    const std::vector<QVector3D> vertices = {
        // front
        QVector3D(-1.0f, -1.0f,  -1.0f),
        QVector3D(1.0f, -1.0f,  -1.0f),
        QVector3D(1.0f,  1.0f,  -1.0f),
        QVector3D(-1.0f,  1.0f,  -1.0f),
        // back
        QVector3D(-1.0f, -1.0f, 1.0f),
        QVector3D(1.0f, -1.0f, 1.0f),
        QVector3D(1.0f,  1.0f, 1.0f),
        QVector3D(-1.0f,  1.0f, 1.0f)
    };

    // Vertex texture coordinates.
    const std::vector<QVector3D> tex_coords = {
        // front
        QVector3D(0.0f, 0.0f,  0.0f),
        QVector3D(1.0f, 0.0f,  0.0f),
        QVector3D(1.0f, 1.0f,  0.0f),
        QVector3D(0.0f, 1.0f,  0.0f),
        // back
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        QVector3D(0.0f, 1.0f, 1.0f)
    };

    // Vertex colors.
    const std::vector<QVector3D> colors = {
        // front colors
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 1.0f),
        // back colors
        QVector3D(1.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 1.0f),
        QVector3D(1.0f, 0.0f, 1.0f),
        QVector3D(0.0f, 0.0f, 0.0f)
    };

    const std::vector<GLuint> indices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
    };

    setVertices(vertices, GL_FLOAT, 3);
    setTextureCoords(tex_coords, GL_FLOAT, 3);
    setColors(colors, GL_FLOAT, 3);
    setIndices(indices, GL_UNSIGNED_INT);
}
