#pragma once

#include "renderer.h"

#include <memory>

class Cube;

class RayCastRenderer : public Renderer {
public:
    RayCastRenderer() = default;
    ~RayCastRenderer() override = default;    

protected:
    void doInit(QOpenGLFunctions *gl) override;
    void doRender(QOpenGLFunctions *gl) override;

private:
    std::shared_ptr<Cube> cube;    
};
