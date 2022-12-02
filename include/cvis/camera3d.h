#ifndef CVIS_INCLUDE_CVIS_CAMERA3D_H_
#define CVIS_INCLUDE_CVIS_CAMERA3D_H_

#include "Eigen/Core"

namespace vis {

/**
 * @brief Object for managing a 3D camera, which is essentially the "user" (or player) eyes in
 * a 3D world. This is how we determine what to view/visualize/draw on the screen
 *
 *
 * An note about the internal implemenentation. OpenGL assumes column major ordering for its
 * matrices. Convientently Eigen is also column order by default. So we can get away with
 * single indexing (use index (3) instead of (0, 2)).
 * If the matrix library changes then this would need to be updated.
 */
class Camera3D {
 public:

  enum class ProjectionTypes {
    PERSPECTIVE,
    ORTHOGRAPHIC
  };

  Camera3D();

  /**
   * @brief Set the cameras position in the world
   *
   * @param newPosition metres, world coordinates
   */
  void SetPosition(const Eigen::Vector3f &newPosition);

  /**
   * @brief Set the  cameras position in the world
   *
   * @param x metres, world coordinates
   * @param y metres, world coordinates
   * @param z metres, world coordinates
   */
  void SetPosition(float x,
                   float y,
                   float z);

  /**
   * @brief Set the target position for the camera to look at
   *
   * @param newTarget metres, world coordinates
   */
  void SetTargetPosition(const Eigen::Vector3f &newTarget);

  /**
   * @brief Set the target position for the camera to look at
   *
   * @param x metres, world coordinates
   * @param y metres, world coordinates
   * @param z metres, world coordinates
   */
  void SetTargetPosition(float x,
                         float y,
                         float z);

  /**
   * @brief Rotate the cameras orientation by incrmeent values.
   * These are not absolute angles. A value of 10 rotates by 10 degrees, does not
   * set the orientation to 10
   *
   * @param x degrees, increment value, typically pitch
   * @param y degrees, increment value, typically yaw
   * @param z degrees, increment value, typically roll
   */
  void RotateByIncrement(float x,
                         float y,
                         float z);

  /**
   * @brief Set the orientation of the camera in the world
   *
   * @param newAngles degrees
   */
  void SetOrientationAngles(const Eigen::Vector3f &newAngles);

  /**
   * @brief Set the orientation of the camera in the world
   *
   * @param x degrees, typically pitch
   * @param y degrees, typically yaw
   * @param z degrees, typically roll
   */
  void SetOrientationAngles(float x,
                            float y,
                            float z);

  const Eigen::Matrix4f GetViewMatrix() const;
 private:
  /**
   * @brief Update the internal view matrix based on the cameras internal members
   */
  void UpdateViewMatrix();

  void UpdateOrientationAngles();

  void ComputeMatrix();

 private:
  // Where is the camera located in real world coordinates (metres)
  Eigen::Vector3f position_;
  // The position the camera is looking at in real world coordinates (metres)
  Eigen::Vector3f target_;
  // The up/vertical axis of the camera, this is used to define the coordinate system of the camera
  // Typically +y axis is up (0, 1, 0) since in OpenGL Y axis is up/down screen (x to the right, and
  // +z screen is out from the screen)
  Eigen::Vector3f up_direction_;
  // The orientation angles of the camera in radians
  Eigen::Vector3f orientation_angles_;
  // The "view" matrix, which is created from the camera parameters.
  // Also called lookAt matrix in some OpenGL contexts
  // It is a homogenous coordinate transformation matrix, rotation and translation
  Eigen::Matrix4f view_;
  // How far the camera position is from the target. This could be calculated from the
  // position_ and target_ vectors, but typically zooming in and out changes distance.
  // So on zoom we change distance, then back calculate the new position_
  double distance_to_target_;

  // The vertical angle field of view (FOV) in degrees
  float field_of_view_;

  // This is a helper member to keep track of just the rotation matrix. This is used
  // as a place holder for some calculations
  Eigen::Matrix4f rotation_;
};

}

#endif