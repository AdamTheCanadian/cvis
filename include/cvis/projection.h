#ifndef CVIS_INCLUDE_CVIS_PROJECTION_H_
#define CVIS_INCLUDE_CVIS_PROJECTION_H_

#include "cmat/mat4f.h"

/**
 * Simple helper to compute aspect ratio from a width and height
 * \return aspect ratio (unitless)
 */
static inline float visProjection_AspectRatio(int width,
                                              int height) {
  return (float)width / (float)height;
}

/**
 * Compute a perspective matrix
 * \param fovInDegrees
 * \param aspectRatio
 * \param nearPlane
 * \param farPlane
 * \return
 */
Mat4f visProjection_Perspective(float fovInDegrees,
                                float aspectRatio,
                                float nearPlane,
                                float farPlane);

#endif
