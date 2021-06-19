#include "renderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <stdexcept>

std::shared_ptr<QOpenGLShaderProgram> Renderer::loadProgram(const char *vert_shader_file, const char *frag_shader_file) {
    auto prog = std::make_shared<QOpenGLShaderProgram>();
    if (!prog->addShaderFromSourceFile(QOpenGLShader::Vertex, QString(vert_shader_file))) {
        throw std::runtime_error(std::string("Failed to load vertex shaders from ") + vert_shader_file
                                 + ":\n" + prog->log().toStdString());
    }
    if (!prog->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(frag_shader_file))) {
        throw std::runtime_error(std::string("Failed to load fragment shaders from ") + frag_shader_file
                                 + ":\n" + prog->log().toStdString());
    }
    if (!prog->link()) {
        throw std::runtime_error(std::string("Failed to link program:\n") + prog->log().toStdString());
    }
    return prog;
}

void Renderer::render(QOpenGLFunctions *gl) {
    if (!program || !data_texture || !color_texture || !opacity_texture) {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoff_low"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoff_high"), cutoff_high);
    program->setUniformValue(program->uniformLocation("cutoff_coeff"), 1.0f/(cutoff_high - cutoff_low));

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

    doRender(gl);

    data_texture->release();
    color_texture->release();
    opacity_texture->release();

    program->release();
}

