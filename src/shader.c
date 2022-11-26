#include "cvis/shader.h"
#include "glad/glad.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static char* LoadShaderFile(const char *file) {

  FILE* shader_file = fopen(file, "r");
  char *shader_code = NULL;
  if (!shader_file) {
    printf("ERROR (Shader): Could not open file: %s\n", file);
    return NULL; 
  }
  // We are going to get the size of the file and allocate the shader source string,
  // then read all the contents into the allocated buffer.
  // Did this instead of doing the std:: stringstream approach which can be a bit slower for larger files
  fseek(shader_file, 0L, SEEK_END);
  size_t file_size = ftell(shader_file);
  fseek(shader_file, 0L, SEEK_SET);

  shader_code = (char *)malloc((file_size + 1)  * sizeof(char));
  fread(shader_code, sizeof(char), file_size, shader_file);
  shader_code[file_size] = '\0';

  return shader_code;
}

visShader visShader_LoadShaderFromFiles(const char *vertexSourceFile,
                                       const char *fragmentShaderFile) {
  int32_t shader_success = 0;
  visShader shader = 0;
  char shader_error_log[512];

  uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char *shader_source = LoadShaderFile(vertexSourceFile);
  if (!shader_source) {
    return shader;
  }
  /* OpenGL requires pointer to const char * so need to set that up */
  const char *vs = shader_source;
  glShaderSource(vertex_shader, 1, &vs, NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_success);
  if (!shader_success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, shader_error_log);
    printf("VERTEX SHADER ERROR: %s\n", shader_error_log);
    return 0;
  }

  uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  shader_source = LoadShaderFile(fragmentShaderFile);
  if (!shader_source) {
    return shader;
  }
  /* OpenGL requires pointer to const char * so need to set that up */
  const char *fs = shader_source;
  glShaderSource(fragment_shader, 1, &fs, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_success);
  if (!shader_success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, shader_error_log);
    printf("FRAGMENT SHADER ERROR: %s\n", shader_error_log);
    return 0;
  }

  shader = glCreateProgram();
  glAttachShader(shader, vertex_shader);
  glAttachShader(shader, fragment_shader);
  glLinkProgram(shader);
  glGetProgramiv(shader, GL_LINK_STATUS, &shader_success);
  if (!shader_success) {
    glGetProgramInfoLog(shader, 512, NULL, shader_error_log);
    printf("SHADER PROGRAM ERROR: %s\n", shader_error_log);
    return 0;
  }
  /* Once linked we can delete the vertex/fragment shaders */
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader;
}