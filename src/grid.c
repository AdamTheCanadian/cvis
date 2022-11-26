#include "cvis/grid.h"
#include "cvis/shader.h"
#include "glad/glad.h"
#include <stdio.h>

static GLuint grid_shader_ = 0;
static GLuint grid_vao_ = 0;
static GLuint grid_vbo_ = 0;

/* The max number of grid vertices. There are 6 vertices per grid line, so
 * this (9600) sets a max number of lines to be 1600 */
#define GRID_MAX_NUM_VERTICES 9600
static float grid_vertices[GRID_MAX_NUM_VERTICES] = {0};

static uint32_t num_grid_vertices_ = 0;
static float grid_x_min_ = -10.0f;
static float grid_x_max_ = 10.0f;
static float grid_y_min_ = -10.0f;
static float grid_y_max_ = 10.0f;

static void SetupGridLines(float stepSize) {

  /* Setup the horizontal spanning lines */
  float curr_val = grid_y_min_;
  while (curr_val <= grid_y_max_ && num_grid_vertices_ < GRID_MAX_NUM_VERTICES) {

    grid_vertices[num_grid_vertices_] = grid_x_min_;
    grid_vertices[num_grid_vertices_ + 1] = curr_val;

    grid_vertices[num_grid_vertices_ + 3] = grid_x_max_;
    grid_vertices[num_grid_vertices_ + 4] = curr_val;

    curr_val += stepSize;
    num_grid_vertices_ += 6;
  }

  /* Setup the vertical spanning lines */
  curr_val = grid_x_min_;
  while (curr_val <= grid_x_max_ && num_grid_vertices_ < GRID_MAX_NUM_VERTICES) {

    grid_vertices[num_grid_vertices_] = curr_val;
    grid_vertices[num_grid_vertices_ + 1] = grid_y_min_;

    grid_vertices[num_grid_vertices_ + 3] = curr_val;
    grid_vertices[num_grid_vertices_ + 4] = grid_y_max_;

    curr_val += stepSize;
    num_grid_vertices_ += 6;
  }

  // TODO: Check for duplicate entries
//  for (int i = 0; i < num_grid_vertices_; i += 6) {
//    printf("[%d] %.2f %.2f %.2f %.2f %.2f %.2f\n",
//           i,
//           grid_vertices[i],
//           grid_vertices[i + 1],
//           grid_vertices[i + 2],
//           grid_vertices[i + 3],
//           grid_vertices[i + 4],
//           grid_vertices[i + 5]);
//  }
}

static void GridShaderInit() {
  grid_shader_ = visShader_LoadShaderFromFiles("/Users/adamclare/projects/quimby/spoc/vis/shaders/grid_shader.vs",
                                               "/Users/adamclare/projects/quimby/spoc/vis/shaders/grid_shader.fs");

  glGenVertexArrays(1, &grid_vao_);
  glGenBuffers(1, &grid_vbo_);
  glBindVertexArray(grid_vao_);

  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_grid_vertices_, grid_vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void visGrid_InitDefault() {
  SetupGridLines(1.0f);
  GridShaderInit();
}

void visGrid_InitWithSpacing(float spacing) {
  SetupGridLines(spacing);
  GridShaderInit();
}

void visGrid_Draw(const Mat4f *view,
                  const Mat4f *projection) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(grid_shader_);
  // Grid default color is black
  float grid_color[4] = {0, 0, 0, 1};
  glUniform4f(glGetUniformLocation(grid_shader_, "color"),
              grid_color[0],
              grid_color[1],
              grid_color[2],
              grid_color[3]);
  glUniformMatrix4fv(glGetUniformLocation(grid_shader_, "view"), 1, GL_FALSE, &view->mat[0]);
  glUniformMatrix4fv(glGetUniformLocation(grid_shader_, "projection"), 1, GL_FALSE, &projection->mat[0]);

  glBindVertexArray(grid_vao_);
  glDrawArrays(GL_LINES, 0, (GLsizei)(num_grid_vertices_ * sizeof(float)) / 6);
}
