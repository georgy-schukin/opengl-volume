#include "triangulated_shape.h"

#include <QOpenGLFunctions>

TriangulatedShape::TriangulatedShape() :
    vertex_buffer(QOpenGLBuffer::VertexBuffer),
    color_buffer(QOpenGLBuffer::VertexBuffer),
    tex_coord_buffer(QOpenGLBuffer::VertexBuffer),
    index_buffer(QOpenGLBuffer::IndexBuffer)
{
    vao.create();
}

void TriangulatedShape::attachVertices(QOpenGLShaderProgram *program, QString var) {
    attachBuffer(program, var, vertex_buffer);
}

void TriangulatedShape::attachTextureCoords(QOpenGLShaderProgram *program, QString var) {
    attachBuffer(program, var, tex_coord_buffer);
}

void TriangulatedShape::attachColors(QOpenGLShaderProgram *program, QString var) {
    attachBuffer(program, var, color_buffer);
}

void TriangulatedShape::attachBuffer(QOpenGLShaderProgram *program, QString var, Buffer &buf) {
    const int loc = program->attributeLocation(var);
    vao.bind();
    buf.bind();
    program->enableAttributeArray(loc);
    program->setAttributeBuffer(loc, buf.elemType(), 0, buf.elemSize());
    vao.release();
}

void TriangulatedShape::draw(QOpenGLFunctions *gl) {
    vao.bind();
    gl->glDrawElements(GL_TRIANGLES, index_buffer.size(), index_buffer.elemType(), 0);
    vao.release();
}
