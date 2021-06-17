#include "renderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
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
