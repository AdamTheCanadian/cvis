#include "cvis/projection.h"
#include <math.h>

Mat4f visProjection_Perspective(float fovInDegrees,
                                float aspectRatio,
                                float nearPlane,
                                float farPlane) {
  Mat4f projection;
  Mat4f_SetZero(&projection);
  double top = nearPlane * tan(fovInDegrees * 0.5 * M_PI / 180.0);
  double right = top * aspectRatio;
  double left = -right;
  double bottom = -top;
  double znear = nearPlane;
  double zfar = farPlane;

  /* Right left distance */
  float rl = (float)(right - left);
  /* Top bottom distance */
  float tb = (float)(top - bottom);
  /* Far near distance */
  float fn = (float)(zfar - znear);

  projection.mat[0] = (float)(znear * 2.0f) / rl;
  projection.mat[5] = (float)(znear * 2.0f) / tb;
  projection.mat[8] = (float)(right + left) / rl;
  projection.mat[9] = (float)(top + bottom) / tb;
  projection.mat[10] = -(float)(zfar + znear) / fn;
  projection.mat[11] = -1.0f;
  projection.mat[14] = -(float)(zfar * znear * 2.0f) / fn;

  return projection;
}