#include "renderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <stdexcept>
#include <vector>
#include <random>

namespace {

std::vector<GLfloat> randomJitter2D(int size) {
    std::vector<GLfloat> jitter;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<GLfloat> dist(0.0f, 1.0f);
    for (int i = 0; i < size * size; i++) {
        jitter.push_back(dist(mt));
    }
    return jitter;
}

}

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

void Renderer::init(QOpenGLFunctions *gl) {
    jitter_texture = std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    initJitter(jitter_texture.get());

    doInit(gl);
}

void Renderer::initJitter(QOpenGLTexture *jitter) {
    jitter->setSize(jitter_size, jitter_size);
    jitter->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
    jitter->setWrapMode(QOpenGLTexture::Repeat);
    jitter->setFormat(QOpenGLTexture::R32F);
    jitter->allocateStorage();
    const auto jitter_data = randomJitter2D(jitter_size);
    jitter->setData(QOpenGLTexture::Red, QOpenGLTexture::Float32, jitter_data.data());
}

void Renderer::render(QOpenGLFunctions *gl) {
    if (!program || !data_texture || !color_texture || !opacity_texture) {
        return;
    }

    program->bind();

    program->setUniformValue(program->uniformLocation("cutoffLow"), cutoff_low);
    program->setUniformValue(program->uniformLocation("cutoffHigh"), cutoff_high);
    const auto cutoff_coeff = (std::abs(cutoff_low - cutoff_high) > 1e-8f ? 1.0f / (cutoff_high - cutoff_low) : 1.0f);
    program->setUniformValue(program->uniformLocation("cutoffCoeff"), cutoff_coeff);

    gl->glActiveTexture(GL_TEXTURE0);
    program->setUniformValue(program->uniformLocation("texture3d"), 0);
    data_texture->bind();

    gl->glActiveTexture(GL_TEXTURE1);
    program->setUniformValue(program->uniformLocation("palette"), 1);
    color_texture->bind();

    gl->glActiveTexture(GL_TEXTURE2);
    program->setUniformValue(program->uniformLocation("opacity"), 2);
    opacity_texture->bind();

    gl->glActiveTexture(GL_TEXTURE3);
    program->setUniformValue(program->uniformLocation("jitter"), 3);
    jitter_texture->bind();

    program->setUniformValue(program->uniformLocation("jitterSize"), jitter_size);
    program->setUniformValue(program->uniformLocation("jitterEnabled"), jitter_enabled);

    const auto mvInv = (view_matrix * model_matrix).inverted();
    const auto eye = mvInv * QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
    const auto light = mvInv * QVector4D(-5.0f, -5.0f, -5.0f, 1.0f);
    program->setUniformValue(program->uniformLocation("eyePosition"), QVector3D(eye.x(), eye.y(), eye.z()));
    program->setUniformValue(program->uniformLocation("lightPosition"), QVector3D(light.x(), light.y(), light.z()));
    program->setUniformValue(program->uniformLocation("lightingEnabled"), lighting_enabled);

    doRender(gl);

    data_texture->release();
    color_texture->release();
    opacity_texture->release();

    program->release();
}

