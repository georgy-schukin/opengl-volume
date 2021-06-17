#pragma once

#include "renderer.h"

#include <memory>

class Plane;

class SliceRenderer : public Renderer {
public:
    SliceRenderer() = default;
    ~SliceRenderer() override = default;

    void init(QOpenGLFunctions *gl) override;
    void render(QOpenGLFunctions *gl) override;

private:
    QMatrix4x4 texture_matrix;
    std::shared_ptr<Plane> plane;
};
