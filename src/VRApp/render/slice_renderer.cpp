#include "slice_renderer.h"
#include "objects/plane.h"

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <cmath>

void SliceRenderer::init(QOpenGLFunctions *gl) {
    program = loadProgram("shaders/plane.vert", "shaders/plane.frag");

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

void SliceRenderer::render(QOpenGLFunctions *gl) {
    if (!program || !data_texture || !color_texture || !opacity_texture) {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoff_low"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoff_high"), cutoff_high);
    program->setUniformValue(program->uniformLocation("cutoff_coeff"), 1.0f/(cutoff_high - cutoff_low));

    const auto texture_inverse_matrix = (view_matrix * model_matrix * texture_matrix).inverted();

    const auto proj_loc = program->uniformLocation("Proj");
    const auto tex_inv_loc = program->uniformLocation("TexInv");

    const int tex3d_loc = program->uniformLocation("texture3d");
    const int palette_loc = program->uniformLocation("palette");
    const int opacity_loc = program->uniformLocation("opacity");

    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(tex3d_loc, 0);
    data_texture->bind();

    gl->glActiveTexture(GL_TEXTURE1);
    program->setUniformValue(palette_loc, 1);
    color_texture->bind();

    gl->glActiveTexture(GL_TEXTURE2);
    program->setUniformValue(opacity_loc, 2);
    opacity_texture->bind();

    static const float cube_half_size = 1.0f; // cube vertices' coords are +1/-1
    static const float cube_extent_radius = cube_half_size * std::sqrt(3.0f); // radius of a sphere around cube
    static const float step_coeff = cube_half_size * std::sqrt(2.0f);

    const auto max_dim = std::max(data_texture->width(), std::max(data_texture->height(), data_texture->depth()));
    // Data cube is located in [0,0,0] in world coordinates and has side length of 2.
    const float view_distance = (view_matrix * QVector4D(0, 0, 0, 1)).length(); // distance from the camera to the origin
    const float step = step_coeff / static_cast<float>(max_dim);
    const int num_of_steps = static_cast<int>(2.0f * cube_extent_radius / step);

    QMatrix4x4 plane_model_matrix;
    plane_model_matrix.scale(cube_extent_radius, cube_extent_radius, 1); // scale plane to fit over cube
    plane_model_matrix.translate(0, 0, -view_distance - cube_extent_radius); // plane is in view space
    for (int i = 0; i <= num_of_steps; i++) {
        program->setUniformValue(proj_loc, projection_matrix * plane_model_matrix);
        program->setUniformValue(tex_inv_loc, texture_inverse_matrix * plane_model_matrix);
        plane->draw(gl);
        plane_model_matrix.translate(0, 0, step);
    }

    data_texture->release();
    color_texture->release();
    opacity_texture->release();

    program->release();
}
