#include "cvis/camera3d.h"
#include <cmath>

using namespace vis;

Camera3d::Camera3d() :
    position_(0, 0, 5),
    target_(0, 0, 0),
    up_(0, 1, 0){

  view_ = Eigen::Matrix4d::Identity();
}

void Camera3d::SetCameraPosition(const Eigen::Vector3d &pos) {
  position_ = pos;
  UpdateView();
}

void Camera3d::SetTargetPosition(const Eigen::Vector3d &target) {
  target_ = target;

  Eigen::Vector3d forward(-view_(0, 2),
                          -view_(1, 2),
                          -view_(2, 2));

  double distance_to_target = target_.dot(position_);
  // We moved the target position but want to maintain the cameras relative
  // view to the target so we need to shift the camera position along the forward vector
  // by the new distance
  position_ = Eigen::Vector3d(target_.x() - distance_to_target * forward.x(),
                              target_.y() - distance_to_target * forward.y(),
                              target_.z() - distance_to_target * forward.z());

  visCamera_ComputeMatrix(cam);
}

void visCamera_RotateIncrement(visCamera* cam,
                               double x,
                               double y,
                               double z) {
  /* Get the current rotation/orientation angles from the view matrix,
   * then will update the angles, and recompute the matrix */
  Vec3f angle = cam->angle;
  /* Increment the current orientation */
  angle.x += (float)x;
  angle.y += (float)y;
  angle.z += (float)z;

  visCamera_SetOrientationAngle(cam, &angle);
}

void visCamera_UpdateLookAtMatrix(visCamera* cam) {

  Mat4f_SetZero(&cam->view);

  /* If the camera is located at the target we only need to translate, no rotation is needed */
  if (Vec3f_IsEqual(&cam->position, &cam->target)) {
    Mat4f_SetIdentity(&cam->view);
    Mat4f_SetIdentity(&cam->rotation);
    Vec3f translation = (Vec3f) {
        .x = -cam->position.x,
        .y = -cam->position.y,
        .z = -cam->position.z};
    cam->view.mat[12] = translation.x;
    cam->view.mat[13] = translation.y;
    cam->view.mat[14] = translation.z;
    return;
  }

  Vec3f forward, right, up;
  Vec3f_SetZero(&forward);
  Vec3f_SetZero(&right);
  Vec3f_SetZero(&up);

  /* Calculate the camera coordinate system Z axis (the direction vector) */
  forward = Vec3f_SubtractVec3f(&cam->position, &cam->target);
  cam->distance_to_target = (double) Vec3f_Length(&forward);
  Vec3f_Normalize(&forward);

  /* By default assume Y axis is up, will correct this later as we determine the right
   * and forward axes */
  up.y = 1.0f;

  /* If we are looking down/up on the Y axis (90 degrees) we need to change the up vector */
  if (fabsf(forward.x) < EPSILON_F && fabsf(forward.z) < EPSILON_F) {
    if (forward.y > 0) {
      /* Foward vector is pointing along the +Y axis */
      up = (Vec3f){.x = 0.0f, .y = 0.0f, .z = -1.0f};
    }
    else {
      /* Foward vector is pointing along the -Y axis */
      up = (Vec3f){.x = 0.0f, .y = 0.0f, .z = 1.0f};
    }
  }

  /* Calculate the cameras right axis. */
  right = Vec3f_Cross(&up, &forward);

  /* In theory up and direction should already be normalized, meaning the cross
   * product is normalized but just to be safe going to normalize */
  Vec3f_Normalize(&right);

  /* Calculate the new up vector */
  up = Vec3f_Cross(&forward, &right);

  Mat4f_SetIdentity(&cam->rotation);
  cam->rotation.mat[0] = right.x;
  cam->rotation.mat[4] = right.y;
  cam->rotation.mat[8] = right.z;

  cam->rotation.mat[1] = up.x;
  cam->rotation.mat[5] = up.y;
  cam->rotation.mat[9] = up.z;

  cam->rotation.mat[2] = forward.x;
  cam->rotation.mat[6] = forward.y;
  cam->rotation.mat[10] = forward.z;

  Mat4f_SetIdentity(&cam->view);
  cam->view.mat[0] = right.x;
  cam->view.mat[4] = right.y;
  cam->view.mat[8] = right.z;

  cam->view.mat[1] = up.x;
  cam->view.mat[5] = up.y;
  cam->view.mat[9] = up.z;

  cam->view.mat[2] = forward.x;
  cam->view.mat[6] = forward.y;
  cam->view.mat[10] = forward.z;

  /* Translational components (note there is a negative sign) */
  Vec3f translation = (Vec3f) {
      .x = -Vec3f_DotProduct(&right, &cam->position),
      .y = -Vec3f_DotProduct(&up, &cam->position),
      .z = -Vec3f_DotProduct(&forward, &cam->position)
  };
  cam->view.mat[12] = translation.x;
  cam->view.mat[13] = translation.y;
  cam->view.mat[14] = translation.z;
  visCamera_GetViewOrientation(cam, &cam->angle);
}

void visCamera_GetViewOrientation(const visCamera* cam,
                                  Vec3f* angleXYZ) {
  float yaw = 0.0f;
  float pitch = 0.0f;
  float roll = 0.0f;

  yaw = RAD_TO_DEG_F * asinf(cam->rotation.mat[8]);
  if (cam->rotation.mat[10] < 0) {
    if (yaw >= 0) {
      yaw = 180.0f - yaw;
    }
    else {
      yaw = -180.0f - yaw;
    }
  }

  if (cam->rotation.mat[0] > -EPSILON_F && cam->rotation.mat[0] < EPSILON_F) {
    roll = 0.0f;
    pitch = RAD_TO_DEG_F * atan2f(cam->rotation.mat[1], cam->rotation.mat[5]);
  }
  else {
    roll = RAD_TO_DEG_F * atan2f(-cam->rotation.mat[4], cam->rotation.mat[0]);
    pitch = RAD_TO_DEG_F * atan2f(-cam->rotation.mat[9], cam->rotation.mat[10]);
  }
  /* Remember in open gl Y is up, so a yaw rotation is rotating around Y */
  angleXYZ->x = pitch;
  angleXYZ->y = -yaw;
  angleXYZ->z = roll;
}

void visCamera_SetOrientationAngle(visCamera* cam,
                                   const Vec3f* angleXYZ) {
  Vec3f_Copy(angleXYZ, &cam->angle);
  Mat4f_SetIdentity(&cam->rotation);

  static const float DEG_TO_RAD_F = (float)(M_PI / 180.0);

  /* Convert euler to rotation matrix */
  const float sx = sinf(angleXYZ->x * DEG_TO_RAD_F);
  const float cx = cosf(angleXYZ->x * DEG_TO_RAD_F);
  const float sy = sinf(-angleXYZ->y * DEG_TO_RAD_F);
  const float cy = cosf(-angleXYZ->y * DEG_TO_RAD_F);
  const float sz = sinf(angleXYZ->z * DEG_TO_RAD_F);
  const float cz = cosf(angleXYZ->z * DEG_TO_RAD_F);

  cam->rotation.mat[0] = cy * cz;
  cam->rotation.mat[1] = sx * sy * cz + cx * sz;
  cam->rotation.mat[2] = -cx * sy * cz + sx * sz;

  cam->rotation.mat[4] = -cy * sz;
  cam->rotation.mat[5] = -sx * sy * sz + cx * cz;
  cam->rotation.mat[6] = cx * sy * sz + sx * cz;

  cam->rotation.mat[8] = sy;
  cam->rotation.mat[9] = -sx * cy;
  cam->rotation.mat[10] = cx * cy;

  visCamera_ComputeMatrix(cam);
}

void visCamera_ComputeMatrix(visCamera* cam) {
  /* Extract the previous rotation matrix (orientation of the camera) */
  Vec3f right = (Vec3f) {
      .x = cam->rotation.mat[0],
      .y = cam->rotation.mat[1],
      .z = cam->rotation.mat[2]};
  Vec3f up = (Vec3f) {
      .x = cam->rotation.mat[4],
      .y = cam->rotation.mat[5],
      .z = cam->rotation.mat[6]};
  Vec3f forward = (Vec3f) {
      .x = cam->rotation.mat[8],
      .y = cam->rotation.mat[9],
      .z = cam->rotation.mat[10]};

  Vec3f translation = (Vec3f) {
      .x = right.x * -cam->target.x + up.x * -cam->target.y + forward.x * -cam->target.z,
      .y = right.y * -cam->target.x + up.y * -cam->target.y + forward.y * -cam->target.z,
      .z = right.z * -cam->target.x + up.z * -cam->target.y + forward.z * -cam->target.z - (float)cam->distance_to_target};

  Mat4f_SetIdentity(&cam->view);

  cam->view.mat[0] = right.x;
  cam->view.mat[1] = right.y;
  cam->view.mat[2] = right.z;

  cam->view.mat[4] = up.x;
  cam->view.mat[5] = up.y;
  cam->view.mat[6] = up.z;

  cam->view.mat[8] = forward.x;
  cam->view.mat[9] = forward.y;
  cam->view.mat[10] = forward.z;

  cam->view.mat[12] = translation.x;
  cam->view.mat[13] = translation.y;
  cam->view.mat[14] = translation.z;

  /* Recompute the camera position */
  forward = (Vec3f) {
      .x = -cam->view.mat[2],
      .y = -cam->view.mat[6],
      .z = -cam->view.mat[10]};
  cam->position = (Vec3f){
      .x = cam->target.x - (float)cam->distance_to_target * forward.x,
      .y = cam->target.y - (float)cam->distance_to_target * forward.y,
      .z = cam->target.z - (float)cam->distance_to_target * forward.z};
}

void visCamera_UpdateFromUserInput(visCamera* cam) {
  static const double angle_scale = 0.2;
  const struct ImGuiIO* io = igGetIO();
  if (!io->WantCaptureMouse) {
    if (io->MouseDown[0]) {
      ImVec2 mouse_delta = io->MouseDelta;
      /* As the mouse moves up down on the screen (which is a change of y position) we want to change
       * the pitch angle. Thats a rotation around the X axis in OpenGL.
       * Likewise, if the moves left/right (change of X position) we want to change the yaw angle. And thats
       * a rotation around Y axis in OpenGL */
      visCamera_RotateIncrement(cam, mouse_delta.y * angle_scale, 0.0, mouse_delta.x * angle_scale);
    }
  }
}