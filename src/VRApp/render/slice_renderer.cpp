#include "slice_renderer.h"
#include "objects/plane.h"

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <cmath>

void SliceRenderer::doInit(QOpenGLFunctions *gl) {
    program = loadProgram("shaders/slice.vert", "shaders/slice.frag");

    plane = std::make_shared<Plane>();
    plane->attachVertices(program.get(), "vertex");

    texture_matrix.setToIdentity();
    texture_matrix.scale(2.0f);
    texture_matrix.translate(-0.5f, -0.5f, -0.5f);

    gl->glDisable(GL_DEPTH_TEST);
    gl->glDisable(GL_CULL_FACE);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SliceRenderer::doRender(QOpenGLFunctions *gl) {
    const auto proj_loc = program->uniformLocation("Proj");
    const auto tex_inv_loc = program->uniformLocation("TexInv");

    static const float cube_half_size = 1.0f; // cube vertices' coords are +1/-1
    static const float cube_extent_radius = cube_half_size * std::sqrt(3.0f); // radius of a sphere around cube

    // Data cube is located in [0,0,0] in world coordinates and has side length of 2.
    const auto view_distance = (view_matrix * QVector4D(0, 0, 0, 1)).length(); // distance from the camera to the origin
    const auto max_dim = std::max(data_texture->width(), std::max(data_texture->height(), data_texture->depth()));
    const auto step = 1.0f / static_cast<GLfloat>(max_dim * step_multiplier);
    const auto num_of_steps = static_cast<int>(2.0f * cube_extent_radius / step);
    program->setUniformValue(program->uniformLocation("step"), step);
    program->setUniformValue(program->uniformLocation("stepMultCoeff"), 1.0f / static_cast<GLfloat>(step_multiplier));

    const auto texture_inverse_matrix = (view_matrix * model_matrix * texture_matrix).inverted();
    QMatrix4x4 plane_model_matrix;
    plane_model_matrix.scale(cube_extent_radius, cube_extent_radius, 1); // scale plane to fit over cube
    plane_model_matrix.translate(0, 0, -view_distance - cube_extent_radius); // plane is in view space

    for (int i = 0; i <= num_of_steps; i++) {
        program->setUniformValue(proj_loc, projection_matrix * plane_model_matrix);
        program->setUniformValue(tex_inv_loc, texture_inverse_matrix * plane_model_matrix);
        plane->draw(gl);
        plane_model_matrix.translate(0, 0, step);
    }
}
