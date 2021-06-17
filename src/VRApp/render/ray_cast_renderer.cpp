#include "ray_cast_renderer.h"
#include "objects/cube.h"

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <cmath>

void RayCastRenderer::init(QOpenGLFunctions *gl) {
    program = loadProgram("shaders/raycast.vert", "shaders/raycast.frag");

    cube = std::make_shared<Cube>();
    cube->attachVertices(program.get(), "vertex");

    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_CULL_FACE);
    gl->glDisable(GL_BLEND);
}

void RayCastRenderer::render(QOpenGLFunctions *gl) {
    if (!program || !data_texture || !color_texture || !opacity_texture) {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoff_low"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoff_high"), cutoff_high);
    program->setUniformValue(program->uniformLocation("cutoff_coeff"), 1.0f/(cutoff_high - cutoff_low));

    //const auto texture_inverse_matrix = (view_matrix * model_matrix * texture_matrix).inverted();

    //const auto proj_loc = program->uniformLocation("Proj");
    //const auto tex_inv_loc = program->uniformLocation("TexInv");
    const auto mvp_loc = program->uniformLocation("MVP");
    program->setUniformValue(mvp_loc, projection_matrix * view_matrix * model_matrix);

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

    cube->draw(gl);

    data_texture->release();
    color_texture->release();
    opacity_texture->release();

    program->release();
}
