#pragma once

#include "renderer.h"

#include <memory>

class Cube;

class RayCastRenderer : public Renderer {
public:
    RayCastRenderer() = default;
    ~RayCastRenderer() override = default;

    void init(QOpenGLFunctions *gl) override;
    void render(QOpenGLFunctions *gl) override;

private:
    std::shared_ptr<Cube> cube;
};
