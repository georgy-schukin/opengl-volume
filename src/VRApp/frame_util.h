#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>
#include <string>
#include <functional>

Frame3D<GLfloat> loadFrameFromFile(const std::string &filename);

Frame3D<GLfloat> makeRandomFrame(size_t dim_size);
Frame3D<GLfloat> makeSectorFrame(size_t dim_size);
Frame3D<GLfloat> makeSphereFrame(size_t dim_size);

Frame3D<GLfloat> makeAnalyticalSurfaceFrame(size_t dim_size, float cutoff,
                                            std::function<float(float,float)> surface_func);
Frame3D<GLfloat> makeImplicitSurfaceFrame(size_t dim_size, float cutoff,
                                           std::function<float(float,float,float)> surface_func);

Frame3D<GLfloat> makeParaboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeHelixFrame(size_t dim_size, float cutoff, float R, float r, float a);
Frame3D<GLfloat> makeHelicoidFrame(size_t dim_size, float cutoff);
Frame3D<GLfloat> makeTorusFrame(size_t dim_size, float cutoff, float R, float r);

Frame3D<GLfloat> makeBubblesFrame(size_t dim_size, size_t num_of_bubbles, float min_rad, float max_rad);
