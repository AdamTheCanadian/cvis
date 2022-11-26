#include "ctest/unit_test.h"
#include "cvis/camera3d.h"

void test_camera3_init() {
  visCamera cam;
  visCamera_Init(&cam);
  UNIT_TEST_EXPECT_EQ_ARRAY_F_TO_VALUE("", cam.position.vec, 0.0f, 3, 1.0e-6f);
  UNIT_TEST_EXPECT_EQ_ARRAY_F_TO_VALUE("", cam.target.vec, 0.0f, 3, 1.0e-6f);
  // Up vector should be (0, 1, 0) on init
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_ARRAY_F_TO_VALUE("", cam.angle.vec, 0.0f, 3, 1.0e-6f);
  UNIT_TEST_EXPECT_EQ_DOUBLE("", cam.distance_to_target, 0.0);

  Mat4f mat;
  Mat4f_SetIdentity(&mat);

  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, mat.mat, 16, 1.0e-5f);
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, mat.mat, 16, 1.0e-5f);
}

void test_camera3_set_position() {
  visCamera cam;
  visCamera_Init(&cam);

  /* First test moving along Z axis, this should induce 0 rotation affects */
  visCamera_SetCameraPosition(&cam, &(Vec3f){.x = 0, .y = 0, .z = 10});

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[2], 10.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_DOUBLE("", cam.distance_to_target, 10.0);

  Mat4f true_mat = {.mat =
      {1, 0, 0, 0,
       0, 1, 0, 0,
       0, 0, 1, 0,
       0, 0, -10, 1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, true_mat.mat, 16, 1.0e-5f);

  Mat4f_SetIdentity(&true_mat);
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, true_mat.mat, 16, 1.0e-5f);

  /* The true values below were taken from an online Windows application and the precision of their
   * output was only 3 digits on some values so some tests are only comparing to 1.0e-3f
   * instead of 1.0e-5f*/

  /* Now test moving along Y axis, this should induce some rotation since we want to maintain tracking on target */
  visCamera_SetCameraPosition(&cam, &(Vec3f){.x = 0, .y = -4, .z = 10});

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[1], -4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[2], 10.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT_EPS("", cam.angle.vec[0], -21.801f, 1.0e-3f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_DOUBLE_EPS("", cam.distance_to_target, 10.7703296142690, 1.0e-6f);

  true_mat = (Mat4f){.mat =
      {1,     0,        0,          0,
       0,     0.928f,   -0.371f,    0,
       0,     0.371f,   0.928f,     0,
       0,     0,        -10.7703f,  1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, true_mat.mat, 16, 1.0e-3f);

  true_mat = (Mat4f){.mat =
      {1,     0,        0,          0,
       0,     0.928f,   -0.371f,    0,
       0,     0.371f,   0.928f,     0,
       0,     0,        0,          1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, true_mat.mat, 16, 1.0e-3f);

  /* Now test moving along X axis, this should induce some rotation since we want to maintain tracking on target */
  visCamera_SetCameraPosition(&cam, &(Vec3f){.x = 8, .y = -4, .z = 10});

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[0], 8.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[1], -4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[2], 10.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT_EPS("", cam.angle.vec[0], -17.346f, 1.0e-3f);
  UNIT_TEST_EXPECT_EQ_FLOAT_EPS("", cam.angle.vec[1], 38.659f, 1.0e-3f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_DOUBLE_EPS("", cam.distance_to_target, 13.4164078649987, 1.0e-6f);

  true_mat = (Mat4f){.mat =
      {0.781f,  0.186f, 0.596f,     0.0f,
       0,       0.955f, -0.298f,    0,
       -0.625f, 0.233f, 0.745f,     0,
       0,       0,      -13.416f,  1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, true_mat.mat, 16, 1.0e-3f);

  true_mat = (Mat4f){.mat =
      {0.781f,  0.186f, 0.596f,     0.0f,
       0,       0.955f, -0.298f,    0,
       -0.625f, 0.233f, 0.745f,     0,
       0,       0,      0,          1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, true_mat.mat, 16, 1.0e-3f);
}

void test_camera3_set_target_position() {
  visCamera cam;
  visCamera_Init(&cam);

  visCamera_SetCameraPosition(&cam, &(Vec3f){.x = 0, .y = 0, .z = 10});

  visCamera_SetTargetPosition(&cam, &(Vec3f){.x = 4, .y = 0, .z = 0});
  /* Setting the target position moves the camera as well so need to make sure to test that */
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[0], 4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[2], 10.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[0], 4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_DOUBLE("", cam.distance_to_target, 10.0);

  Mat4f true_mat = {.mat =
      {1, 0, 0, 0,
       0, 1, 0, 0,
       0, 0, 1, 0,
       -4, 0, -10, 1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, true_mat.mat, 16, 1.0e-5f);

  Mat4f_SetIdentity(&true_mat);
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, true_mat.mat, 16, 1.0e-5f);

  /* Moving Target in Y and Z direction */
  visCamera_SetTargetPosition(&cam, &(Vec3f){.x = 4, .y = -4, .z = 8});
  /* Setting the target position moves the camera as well so need to make sure to test that */
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[0], 4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[1], -4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.position.vec[2], 18.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[1], 1.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.up.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[0], 4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[1], -4.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.target.vec[2], 8.0f);

  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[0], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[1], 0.0f);
  UNIT_TEST_EXPECT_EQ_FLOAT("", cam.angle.vec[2], 0.0f);

  UNIT_TEST_EXPECT_EQ_DOUBLE("", cam.distance_to_target, 10.0);

  true_mat = (Mat4f){.mat =
      {1, 0, 0, 0,
       0, 1, 0, 0,
       0, 0, 1, 0,
       -4, 4, -18, 1}};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.view.mat, true_mat.mat, 16, 1.0e-5f);

  Mat4f_SetIdentity(&true_mat);
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", cam.rotation.mat, true_mat.mat, 16, 1.0e-5f);
}

void test_camera3_run() {
  UNIT_TEST_SETUP("Camera3");
  UNIT_TEST_RUN_TEST("Init", test_camera3_init);
  UNIT_TEST_RUN_TEST("Set Camera Position", test_camera3_set_position);
  UNIT_TEST_RUN_TEST("Set Target Position", test_camera3_set_target_position);
  UNIT_TEST_FINISH("Camera3");
}