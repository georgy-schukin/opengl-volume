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

void RayCastRenderer::doRender(QOpenGLFunctions *gl) {
    const auto mv = view_matrix * model_matrix;
    const auto mvp = projection_matrix * mv;
    const auto mvInv = mv.inverted();
    const auto eye = mvInv * QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    const auto light = mvInv * QVector4D(-5.0f, -5.0f, -5.0f, 1.0f);
    program->setUniformValue(program->uniformLocation("MVP"), mvp);
    program->setUniformValue(program->uniformLocation("eyePosition"), QVector3D(eye.x(), eye.y(), eye.z()));
    program->setUniformValue(program->uniformLocation("lightPosition"), QVector3D(light.x(), light.y(), light.z()));
    program->setUniformValue(program->uniformLocation("lightingEnabled"), lightingEnabled);

    const auto max_dim = std::max(data_texture->width(), std::max(data_texture->height(), data_texture->depth()));
    const auto step = std::sqrt(3.0f) / static_cast<GLfloat>(max_dim * step_multiplier);
    program->setUniformValue(program->uniformLocation("step"), step);
    program->setUniformValue(program->uniformLocation("numSteps"), static_cast<int>(2.0f * std::sqrt(3.0f) / step));

    cube->draw(gl);
}
