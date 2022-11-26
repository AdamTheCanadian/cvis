#include "cvis/vis.h"
#include "cmat/vec3f.h"
#include "glad/glad.h"
#include <assert.h>

#define MAX_WAYPOINTS 1000
static Vec3f waypoints[MAX_WAYPOINTS] = {0};

static int number_of_waypoints_ = 0;

static visShader waypoints_shader_ = 0;
static uint32_t waypoints_vao_, waypoints_vbo_;

void visWaypoints_Init() {
  waypoints_shader_ = visShader_LoadShaderFromFiles("/Users/adamclare/projects/quimby/spoc/vis/shaders/grid_shader.vs",
                                                   "/Users/adamclare/projects/quimby/spoc/vis/shaders/grid_shader.fs");

  glGenVertexArrays(1, &waypoints_vao_);
  glGenBuffers(1, &waypoints_vbo_);
  glBindVertexArray(waypoints_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, waypoints_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(waypoints), waypoints, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void visWaypoints_Add(float x,
                      float y,
                      float z) {
  assert(number_of_waypoints_ < MAX_WAYPOINTS);
  waypoints[number_of_waypoints_].x = x;
  waypoints[number_of_waypoints_].y = y;
  waypoints[number_of_waypoints_].z = z;
  number_of_waypoints_ += 1;

  glBindBuffer(GL_ARRAY_BUFFER, waypoints_vbo_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_waypoints_ * sizeof(float) * 3, waypoints);
}

void visWaypoints_Draw() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(waypoints_shader_);
  glPointSize(5);
  // Default color to blue
  float wp_color[4] = {0, 0, 1, 1};
  glUniform4f(glGetUniformLocation(waypoints_shader_, "color"),
              wp_color[0],
              wp_color[1],
              wp_color[2],
              wp_color[3]);
  glUniformMatrix4fv(glGetUniformLocation(waypoints_shader_, "view"), 1, GL_FALSE, &vis_GetCurrentView()->mat[0]);
  glUniformMatrix4fv(glGetUniformLocation(waypoints_shader_, "projection"), 1, GL_FALSE, &vis_GetCurrentProjection()->mat[0]);

  glBindVertexArray(waypoints_vao_);
  glDrawArrays(GL_POINTS, 0, (GLsizei)number_of_waypoints_);
  glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)number_of_waypoints_);
}