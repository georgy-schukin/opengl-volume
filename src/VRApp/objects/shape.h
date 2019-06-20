#pragma once

class QOpenGLFunctions;

class Shape {
public:
    virtual ~Shape() = default;

    virtual void draw(QOpenGLFunctions*) = 0;
};
