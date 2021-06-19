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
    cube->attachTextureCoords(program.get(), "texCoord");
    cube->attachColors(program.get(), "color");

    gl->glDisable(GL_DEPTH_TEST);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
    gl->glFrontFace(GL_CW);
    gl->glEnable(GL_BLEND);
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RayCastRenderer::render(QOpenGLFunctions *gl) {
    if (!program || !data_texture || !color_texture || !opacity_texture) {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoff_low"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoff_high"), cutoff_high);
    program->setUniformValue(program->uniformLocation("cutoff_coeff"), 1.0f/(cutoff_high - cutoff_low));

    const auto mv = view_matrix * model_matrix;
    const auto mvp = projection_matrix * mv;
    const auto eye = mv.inverted() * QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    program->setUniformValue(program->uniformLocation("MVP"), mvp);
    program->setUniformValue(program->uniformLocation("eye"), eye);

    const auto max_dim = std::max(data_texture->width(), std::max(data_texture->height(), data_texture->depth()));
    const auto step = std::sqrt(3.0f) / max_dim;
    program->setUniformValue(program->uniformLocation("step"), step);
    program->setUniformValue(program->uniformLocation("numSteps"), static_cast<int>(2.0f * std::sqrt(3.0f) / step));

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
