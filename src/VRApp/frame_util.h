#pragma once

#include "frame3d.h"

#include <QOpenGLFunctions>
#include <QVector3D>

#include <vector>
#include <string>
#include <functional>

Frame3D<GLfloat> makeRandomFrame(size_t dim_size);
Frame3D<GLfloat> makeSectorFrame(size_t dim_size);
Frame3D<GLfloat> makeSphereFrame(size_t dim_size);

Frame3D<GLfloat> makeAnalyticalSurfaceFrame(size_t dim_size, double cutoff,
                                            std::function<double(double,double)> surface_func);
Frame3D<GLfloat> makeImplicitSurfaceFrame(size_t dim_size, double cutoff,
                                           std::function<double(double,double,double)> surface_func);

Frame3D<GLfloat> makeParaboloidFrame(size_t dim_size, double cutoff);
Frame3D<GLfloat> makeHyperboloidFrame(size_t dim_size, double cutoff);
Frame3D<GLfloat> makeHyperbolicParaboloidFrame(size_t dim_size, double cutoff);
Frame3D<GLfloat> makeHelixFrame(size_t dim_size, double cutoff, double R, double r, double a);
Frame3D<GLfloat> makeHelicoidFrame(size_t dim_size, double cutoff);
Frame3D<GLfloat> makeTorusFrame(size_t dim_size, double cutoff, double R, double r);

Frame3D<GLfloat> makeBubblesFrame(size_t dim_size, size_t num_of_bubbles, double min_rad, double max_rad);

Frame3D<GLfloat> makePerlinNoiseFrame(size_t dim_size, double freq = 1.0);
Frame3D<GLfloat> makePerlinNoiseOctavesFrame(size_t dim_size, int steps, double start_freq = 1.0, double start_ampl = 1.0);
