#pragma once

#include <memory>
#include <QMatrix4x4>

class QOpenGLContext;
class QOpenGLShaderProgram;
class QOpenGLFunctions;
class QOpenGLTexture;

class Renderer {
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    void init(QOpenGLFunctions *gl);
    void render(QOpenGLFunctions *gl);

    void enableLighting(bool enabled) {
        lighting_enabled = enabled;
    }

    void enableJitter(bool enabled) {
        jitter_enabled = enabled;
    }

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

    void setStepMultiplier(int multipl) {
        step_multiplier = multipl;
    }

protected:
    static std::shared_ptr<QOpenGLShaderProgram> loadProgram(const char *vert_shader_file, const char *frag_shader_file);

    virtual void doInit(QOpenGLFunctions *gl) = 0;
    virtual void doRender(QOpenGLFunctions *gl) = 0;

private:
    void initJitter(QOpenGLTexture *jitter);

protected:
    QMatrix4x4 model_matrix;
    QMatrix4x4 view_matrix;
    QMatrix4x4 projection_matrix;

    QOpenGLTexture *data_texture = nullptr;
    QOpenGLTexture *color_texture = nullptr;
    QOpenGLTexture *opacity_texture = nullptr;
    std::unique_ptr<QOpenGLTexture> jitter_texture;

    std::shared_ptr<QOpenGLShaderProgram> program;

    float cutoff_low {0.0f}, cutoff_high {1.0f};

    int step_multiplier = 1;
    int jitter_size = 64;
    bool lighting_enabled = false;
    bool jitter_enabled = false;
};
