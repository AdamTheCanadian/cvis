#ifndef CVIS_INCLUDE_CVIS_CAMERA3D_H_
#define CVIS_INCLUDE_CVIS_CAMERA3D_H_

#include "cmat/mat4f.h"
#include "cmat/vec3f.h"

/**
 * A camera for viewing in a 3D world
 */
typedef struct {
  Mat4f view;
  Mat4f rotation;

  Vec3f position;
  Vec3f target;
  Vec3f up;

  Vec3f angle;
  double distance_to_target;
} visCamera;

/**
 * @brief Initialize a camera object
 */
void visCamera_Init(visCamera* cam);

/**
 * @brief Set the XYZ position of the camera
 */
void visCamera_SetCameraPosition(visCamera* cam,
                                 const Vec3f* pos);

/**
 * @brief Set the XYZ position of the target
 */
void visCamera_SetTargetPosition(visCamera* cam,
                                 const Vec3f* target);

void visCamera_RotateIncrement(visCamera* cam,
                               double x,
                               double y,
                               double z);

void visCamera_GetViewOrientation(const visCamera* cam,
                                  Vec3f* angleXYZ);

void visCamera_SetOrientationAngle(visCamera* cam,
                                   const Vec3f* angleXYZ);

void visCamera_ComputeMatrix(visCamera* cam);

/* Update the look at matrix based on the cameras position, target, and up vectors */
void visCamera_UpdateLookAtMatrix(visCamera* cam);

void visCamera_UpdateFromUserInput(visCamera* cam);
#endif