#include "cvis/vis.h"
#include "glad/glad.h"
#include "cmat/mat4f.h"
#include <math.h>

static double robot_width_ = 1.0;
static double robot_height_ = 1.0;
static double robot_length_ = 1.0;

static visShader robot_shader_ = 0;

/* TODO: Use an EBO here? */
/* This creates a cube of 1x1x1 and we will scale it accordingly from the robot parameters */
static float robot_default_vertices_[] = {
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    0.5f,-0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f,-0.5f,
    0.5f,-0.5f,-0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f,-0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f,-0.5f, 0.5f
};

static uint32_t robot_vao_, robot_vbo_;

static Mat4f robot_model_;

/* Update the local (cpu side) model matrix and send the model matrix to the gpu/shader.
 * Must be called after shader is created.
 * Will disable shader after sending matrix*/
static void UpdateModelMatrix(double length,
                              double width,
                              double height) {
  /* TOOD: Assert here if < 0 */
  robot_length_ = length;
  robot_width_ = width;
  robot_height_ = height;

  Mat4f_SetIdentity(&robot_model_);
  /* Since the default vertices are for a 1x1x1 cube, the robot l/w/h parameters can be used as
   * the scaling values (which are the diagonals of the model matrix) */
  Mat4f_Set(&robot_model_, 0, 0, (float)robot_width_);
  Mat4f_Set(&robot_model_, 1, 1, (float)robot_length_);
  Mat4f_Set(&robot_model_, 2, 2, (float)robot_height_);

  /* We want the robot to appear at a height (Z) of 0, so its on top of the ground plane. To do this
   * we need to shift the robot up 1/2 of the height. If we dont do this, half of the robot will be below
   * the ground, and the other half above.
   * This is because the default vertices, which are centered on 0, go from -0.5 to 0.5,
   * so by default half of the vertices are below 0 (all the -0.5) */
  Mat4f_Set(&robot_model_, 3, 2, 0.5f * (float)robot_height_);

  glUseProgram(robot_shader_);
  GLint model_loc = glGetUniformLocation(robot_shader_, "model");
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, &robot_model_.mat[0]);
}

void visRobot_Init(double length,
                   double width,
                   double height) {

  /* Setup the shader and vertex buffers */
  robot_shader_ = visShader_LoadShaderFromFiles("/Users/adamclare/projects/quimby/spoc/vis/shaders/simple_shader.vs",
                                                "/Users/adamclare/projects/quimby/spoc/vis/shaders/simple_shader.fs");

  glGenVertexArrays(1, &robot_vao_);
  glGenBuffers(1, &robot_vbo_);
  glBindVertexArray(robot_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, robot_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(robot_default_vertices_), robot_default_vertices_, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glUseProgram(0);

  UpdateModelMatrix(length, width, height);
}

void visRobot_Draw() {
  glUseProgram(robot_shader_);

  // Default color to silver
//  float robot_color[4] = {192.0f/255, 192.0f/255, 192.0f/255, 1};
  float robot_color[4] = {1, 0, 0, 1};
  glUniform4f(glGetUniformLocation(robot_shader_, "color"),
              robot_color[0],
              robot_color[1],
              robot_color[2],
              robot_color[3]);

  GLint view_loc = glGetUniformLocation(robot_shader_, "view");
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, &vis_GetCurrentView()->mat[0]);
  GLint projection_loc = glGetUniformLocation(robot_shader_, "projection");
  glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &vis_GetCurrentProjection()->mat[0]);

  glBindVertexArray(robot_vao_);
  glDrawArrays(GL_TRIANGLES, 0, 12*3);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glEnableVertexAttribArray(0);
}

void visRobot_ChangeSize(double length,
                         double width,
                         double height) {

  UpdateModelMatrix(length, width, height);
}

void visRobot_UpdatePosition(Vec3f pose) {

  Mat4f translation;
  Mat4f_SetIdentity(&translation);
  translation.mat[12] = pose.x;
  translation.mat[13] = pose.y;
  translation.mat[14] = 0.5f * (float)robot_height_;

  printf("Translation\n");
  for (int i = 0; i < 16; i += 4) {
    printf("%.3f %.3f %.3f %.3f\n",
           translation.mat[i],
           translation.mat[i + 1],
           translation.mat[i + 2],
           translation.mat[i + 3]);
  }

  Mat4f rotation;
  Mat4f_SetIdentity(&rotation);
  // 90 - angle since the kinematic equations use theta as angle from x axis but
  // I want the angle to be from Y axis to be alligned with navigation heading (w.r.t North)
  rotation.mat[0] = cosf(M_PI_2 - pose.z);
  rotation.mat[1] = sinf(M_PI_2 - pose.z);
  rotation.mat[4] = -sinf(M_PI_2 - pose.z);
  rotation.mat[5] = cosf(M_PI_2 - pose.z);

  printf("Rotation\n");
  for (int i = 0; i < 16; i += 4) {
    printf("%.3f %.3f %.3f %.3f\n",
           rotation.mat[i],
           rotation.mat[i + 1],
           rotation.mat[i + 2],
           rotation.mat[i + 3]);
  }
  Mat4f scale;
  Mat4f_SetIdentity(&scale);
  Mat4f_Set(&scale, 0, 0, (float)robot_width_);
  Mat4f_Set(&scale, 1, 1, (float)robot_length_);
  Mat4f_Set(&scale, 2, 2, (float)robot_height_);

  printf("Scale\n");
  for (int i = 0; i < 16; i += 4) {
    printf("%.3f %.3f %.3f %.3f\n",
           scale.mat[i],
           scale.mat[i + 1],
           scale.mat[i + 2],
           scale.mat[i + 3]);
  }
  Mat4f temp =Mat4f_MultiplyMat4f(&rotation, &scale);

  robot_model_ = Mat4f_MultiplyMat4f(&translation, &temp);

  printf("Model\n");
  for (int i = 0; i < 16; i += 4) {
    printf("%.3f %.3f %.3f %.3f\n",
           robot_model_.mat[i],
           robot_model_.mat[i + 1],
           robot_model_.mat[i + 2],
           robot_model_.mat[i + 3]);
  }
  glUseProgram(robot_shader_);
  GLint model_loc = glGetUniformLocation(robot_shader_, "model");
  glUniformMatrix4fv(model_loc, 1, GL_FALSE, &robot_model_.mat[0]);
  glUseProgram(0);
}