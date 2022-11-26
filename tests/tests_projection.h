#ifndef SPOC_TESTS_PROJECTION_H_
#define SPOC_TESTS_PROJECTION_H_

#include "ctest/unit_test.h"
#include "cvis/projection.h"

void test_projection_perspective() {
  Mat4f projection = visProjection_Perspective(45, 4.0f/3.0f, 0.01f, 1000.0f);
  Mat4f true_projection = {.mat = {
      1.810660, 0.000000, 0.000000, 0.000000,
      0.000000, 2.414213, 0.000000, 0.000000,
      0.000000, 0.000000, -1.000020, -1.000000,
      0.000000, 0.000000, -0.020000, 0.000000
  }};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", projection.mat, true_projection.mat, 16, 1.0e-5f);

  // Different FOV
  projection = visProjection_Perspective(100, 4.0f/3.0f, 0.01f, 1000.0f);
  true_projection = (Mat4f){.mat = {
      0.629325, 0.000000, 0.000000, 0.000000,
      0.000000, 0.839100, 0.000000, 0.000000,
      0.000000, 0.000000, -1.000020, -1.000000,
      0.000000, 0.000000, -0.020000, 0.000000
  }};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", projection.mat, true_projection.mat, 16, 1.0e-5f);

  // Different Aspect ratio
  projection = visProjection_Perspective(100, 16.0f/9.0f, 0.01f, 1000.0f);
  true_projection = (Mat4f){.mat = {
      0.471994, 0.000000, 0.000000, 0.000000,
      0.000000, 0.839100, 0.000000, 0.000000,
      0.000000, 0.000000, -1.000020, -1.000000,
      0.000000, 0.000000, -0.020000, 0.000000
  }};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", projection.mat, true_projection.mat, 16, 1.0e-5f);


  // Different near and far plane
  projection = visProjection_Perspective(100, 16.0f/9.0f, 1.0f, 10.0f);
  true_projection = (Mat4f){.mat = {
      0.471994, 0.000000, 0.000000, 0.000000,
      0.000000, 0.839100, 0.000000, 0.000000,
      0.000000, 0.000000, -1.222222, -1.000000,
      0.000000, 0.000000, -2.222222, 0.000000
  }};
  UNIT_TEST_EXPECT_EQ_ARRAY_F("", projection.mat, true_projection.mat, 16, 1.0e-5f);
}

void tests_projection_run() {
  UNIT_TEST_SETUP("Projection");
  UNIT_TEST_RUN_TEST("Perspective", test_projection_perspective);
  UNIT_TEST_FINISH("Projection");
}

#endif
