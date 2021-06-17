#pragma once

#include <memory>
#include <QMatrix4x4>

class QOpenGLTexture;
class QOpenGLContext;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

class Renderer {
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void init(QOpenGLFunctions *gl) = 0;
    virtual void render(QOpenGLFunctions *gl) = 0;

    void setDataTexture(QOpenGLTexture *tex) {
        data_texture = tex;
    }

    void setColorTexture(QOpenGLTexture *tex) {
        color_texture = tex;
    }

    void setOpacityTexture(QOpenGLTexture *tex) {
        opacity_texture = tex;
    }

    void setMVP(const QMatrix4x4 &model, const QMatrix4x4 &view, const QMatrix4x4 &proj) {
        model_matrix = model;
        view_matrix = view;
        projection_matrix = proj;
    }

    void setCutoff(float low, float high) {
        cutoff_low = low;
        cutoff_high = high;
    }

protected:
    static std::shared_ptr<QOpenGLShaderProgram> loadProgram(const char *vert_shader_file, const char *frag_shader_file);

protected:
    QMatrix4x4 model_matrix;
    QMatrix4x4 view_matrix;
    QMatrix4x4 projection_matrix;

    QOpenGLTexture *data_texture = nullptr;
    QOpenGLTexture *color_texture = nullptr;
    QOpenGLTexture *opacity_texture = nullptr;

    std::shared_ptr<QOpenGLShaderProgram> program;

    float cutoff_low {0.0f}, cutoff_high {1.0f};
};
