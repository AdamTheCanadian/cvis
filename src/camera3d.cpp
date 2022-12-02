#include "cvis/camera3d.h"
#include <cmath>


static constexpr float EPS = 1.0e-5f;

vis::Camera3D::Camera3D() : position_(0, 0, 0),
                            target_(0, 0, 0),
                            up_direction_(0, 1, 0),
                            distance_to_target_(0),
                            field_of_view_(45) {

  // Initialize to identity in case forget to set any of the parameters we dont get
  // a zero matrix (no drawing/rendering will no happen)
  view_.setIdentity();
  rotation_.setIdentity();
}

void vis::Camera3D::SetPosition(const Eigen::Vector3f &newPosition) {
  position_ = newPosition;
}

void vis::Camera3D::SetPosition(float x,
                                float y,
                                float z) {
  SetPosition(Eigen::Vector3f(x, y, z));
}

void vis::Camera3D::SetTargetPosition(const Eigen::Vector3f &newTarget) {
  target_ = newTarget;

  // When we change the target position we want to matain the same distance to target and
  // orientation of the camera so we need to move the camera positions in the direction
  // of new current forward/direction vector
  Eigen::Vector3f forward(-view_(2), -view_(6), -view_(10));
  position_.x() = target_.x() - distance_to_target_ * forward.x();
  position_.y() = target_.y() - distance_to_target_ * forward.y();
  position_.z() = target_.z() - distance_to_target_ * forward.z();

  ComputeMatrix();
}

void vis::Camera3D::SetTargetPosition(float x,
                                      float y,
                                      float z) {
  SetTargetPosition(Eigen::Vector3f(x, y, z));
}

void vis::Camera3D::RotateByIncrement(float x,
                                      float y,
                                      float z) {


  Eigen::Vector3f current_angle = orientation_angles_;
  // Increment the current orientation
  current_angle.x() += (float)x;
  current_angle.y() += (float)y;
  current_angle.z() += (float)z;

  SetOrientationAngles(current_angle);
}

void vis::Camera3D::SetOrientationAngles(const Eigen::Vector3f &newAngles) {
  orientation_angles_ = newAngles;
  rotation_.setIdentity();

  /* Convert euler to rotation matrix */
  const float sx = sinf(newAngles.x() * M_PI / 180.0f);
  const float cx = cosf(newAngles.x() * M_PI / 180.0f);
  const float sy = sinf(-newAngles.y() * M_PI / 180.0f);
  const float cy = cosf(-newAngles.y() * M_PI / 180.0f);
  const float sz = sinf(newAngles.z() * M_PI / 180.0f);
  const float cz = cosf(newAngles.z() * M_PI / 180.0f);

  rotation_(0) = cy * cz;
  rotation_(1) = sx * sy * cz + cx * sz;
  rotation_(2) = -cx * sy * cz + sx * sz;

  rotation_(4) = -cy * sz;
  rotation_(5) = -sx * sy * sz + cx * cz;
  rotation_(6) = cx * sy * sz + sx * cz;

  rotation_(8) = sy;
  rotation_(9) = -sx * cy;
  rotation_(10) = cx * cy;

  ComputeMatrix();
}

void vis::Camera3D::SetOrientationAngles(float x,
                                         float y,
                                         float z) {
  SetOrientationAngles(Eigen::Vector3f(x, y, z));
}

const Eigen::Matrix4f vis::Camera3D::GetViewMatrix() const {
  return view_;
}

void vis::Camera3D::UpdateViewMatrix() {
  // Make sure we reset any entries, dont want to carry forward any off
  // diagonal elements for example
  view_.setZero();

  // If the camera is located at the target position, or close enough, we only
  // need to apply a translation component to the view matrix
  if (abs((position_ - target_).norm()) < 1.0e-3f) {
    view_.setIdentity();
    rotation_.setIdentity();
    view_(12) = -position_.x();
    view_(13) = -position_.y();
    view_(14) = -position_.z();
    return;
  }

  // We need to calculate the three axes of the camera system.
  // In OpenGL these are usually called forward, right, and up vectors
  Eigen::Vector3f forward(0, 0, 0);
  Eigen::Vector3f right(0, 0, 0);
  Eigen::Vector3f up(0, 0, 0);

  forward = position_ - target_;
  // Can update the distance to the target now that we have the vector between the camera and target
  distance_to_target_ = forward.norm();
  // Need to keep the vector as a unit/direction vector
  forward.normalize();

  // We start with assuming the up vector is +y, and will correct for it later
  up.y() = 1;
  // If we are looking down/up on the Y axis (90 degrees) we need to change the up vector
  if (abs(forward.x()) < EPS && abs(forward.z()) < EPS) {
    if (forward.y() > 0) {
      /* Foward vector is pointing along the +Y axis */
      up = Eigen::Vector3f(0, 0, -1);
    }
    else {
      /* Foward vector is pointing along the -Y axis */
      up = Eigen::Vector3f(0, 0, 1);
    }
  }

  // Take the cross product of the two axes vectors to determine the third axes
  right.x() = up.y() * forward.z() - up.z() * forward.y();
  right.y() = up.z() * forward.x() - up.x() * forward.z();
  right.z() = up.x() * forward.y() - up.y() * forward.x();

  // Make sure its a unit vector
  right.normalize();

  // Calculate the new up vector since we assumed its direction in the previous stesp
  up.x() = forward.y() * right.z() - forward.z() * right.y();
  up.y() = forward.z() * right.x() - forward.x() * right.z();
  up.z() = forward.x() * right.y() - forward.y() * right.x();

  rotation_.setIdentity();
  rotation_(0) = right.x();
  rotation_(4) = right.y();
  rotation_(8) = right.z();

  rotation_(1) = up.x();
  rotation_(5) = up.y();
  rotation_(9) = up.z();

  rotation_(2) = forward.x();
  rotation_(6) = forward.y();
  rotation_(10) = forward.z();

  view_.setIdentity();
  view_(0) = right.x();
  view_(4) = right.y();
  view_(8) = right.z();
  view_(1) = up.x();
  view_(5) = up.y();
  view_(9) = up.z();
  view_(2) = forward.x();
  view_(6) = forward.y();
  view_(10) = forward.z();

  Eigen::Vector3f translation;
  translation.x() = -right.dot(position_);
  translation.y() = -up.dot(position_);
  translation.z() = -forward.dot(position_);

  view_(12) = translation.x();
  view_(13) = translation.y();
  view_(14) = translation.z();

  UpdateOrientationAngles();
}

void vis::Camera3D::UpdateOrientationAngles() {
  float yaw = 0.0f;
  float pitch = 0.0f;
  float roll = 0.0f;

  yaw = 180.0f / M_PI * asinf(rotation_(8));
  if (rotation_(10) < 0) {
    if (yaw >= 0) {
      yaw = 180.0f - yaw;
    }
    else {
      yaw = -180.0f - yaw;
    }
  }

  if (rotation_(0) > -EPS && rotation_(0) < EPS) {
    roll = 0.0f;
    pitch = 180.0f / M_PI * atan2f(rotation_(1), rotation_(5));
  }
  else {
    roll = 180.0f / M_PI * atan2f(-rotation_(4), rotation_(0));
    pitch = 180.0f / M_PI * atan2f(-rotation_(9), rotation_(10));
  }

  /* Remember in open gl Y is up, so a yaw rotation is rotating around Y axis */
  orientation_angles_ = Eigen::Vector3f(pitch, -yaw, roll);
}

void vis::Camera3D::ComputeMatrix() {
  Eigen::Vector3f right(rotation_(0), rotation_(1), rotation_(2));
  Eigen::Vector3f up(rotation_(4), rotation_(5), rotation_(6));
  Eigen::Vector3f forward(rotation_(8), rotation_(9), rotation_(10));

  Eigen::Vector3f translation;
  translation.x() = right.x() * -target_.x() + up.x() * -target_.y() + forward.x() * -target_.z();
  translation.y() = right.y() * -target_.x() + up.y() * -target_.y() + forward.y() * -target_.z();
  translation.z() = right.z() * -target_.x() + up.z() * -target_.y() + forward.z() * -target_.z() - (float)distance_to_target_;

  view_.setIdentity();

  view_(0) = right.x();
  view_(1) = right.y();
  view_(2) = right.z();

  view_(4) = up.x();
  view_(5) = up.y();
  view_(6) = up.z();

  view_(8) = forward.x();
  view_(9) = forward.y();
  view_(10) = forward.z();

  view_(12) = translation.x();
  view_(13) = translation.y();
  view_(14) = translation.z();

  /* Recompute the camera position */
  forward.x() = -view_(2);
  forward.y() = -view_(6);
  forward.z() = -view_(10);

  position_.x() = target_.x() - distance_to_target_ * forward.x();
  position_.y() = target_.y() - distance_to_target_ * forward.y();
  position_.z() = target_.z() - distance_to_target_ * forward.z();
}