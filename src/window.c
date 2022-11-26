#include "cvis/window.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cvis/projection.h"
#include <math.h>

/* Main window object */
static GLFWwindow *window_ = NULL;
static float field_of_view_deg_ = 45;
static int window_width_ = 0;
static int window_height_ = 0;
static bool mouse_left_pushed_ = false;
static double prev_mouse_x_ = 0.0;
static double prev_mouse_y_ = 0.0;

Mat4f projection_;

static void WindowMouseCallback(GLFWwindow* window,
                                 double xpos,
                                 double ypos);

static void WindowKeyboardCallback(GLFWwindow* window,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods);

static void WindowScrollCallback(GLFWwindow* window,
                                  double xoffset,
                                  double yoffset);

static void WindowMouseButtonCallback(GLFWwindow* window,
                                       int button,
                                       int action,
                                       int mods);

static void WindowResizeCallback(GLFWwindow *window,
                                 int width,
                                 int height);

static void UpdateProjectionMatrix();

bool visWindow_Initialize(const char *windowName,
                          int width,
                          int height) {
  #if defined(__APPLE__)
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
  #endif
  if (!glfwInit()) {
    printf("GLFW ERROR: Failed to initialize\n");
    return false;
  }
  
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  /* OSX Requires fordward compatibility */
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
  window_width_ = width;
  window_height_ = height;
  UpdateProjectionMatrix();

  window_ = glfwCreateWindow(window_width_, window_height_, windowName, NULL, NULL);
  if (window_ == NULL) {
    printf("GLFW ERROR: Failed to create window\n");
    return false;
  }
  glfwSetFramebufferSizeCallback(window_, WindowResizeCallback);
  glfwSetCursorPosCallback(window_, WindowMouseCallback);
  glfwSetKeyCallback(window_, WindowKeyboardCallback);
  glfwSetScrollCallback(window_, WindowScrollCallback);
  glfwSetMouseButtonCallback(window_, WindowMouseButtonCallback);
  
  glfwMakeContextCurrent(window_);
  /* Enable vsync, when this is disabled was getting high cpu usage */
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("GLAD ERROR: Failed to initialize\n");
    return false;
  }
  igCreateContext(NULL);
  const char* glsl_version = "#version 330 core";
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  igStyleColorsDark(NULL);

  return true;
}

bool visWindow_ShouldClose() {
  return glfwWindowShouldClose(window_);
}

void visWindow_NewFrame() {
  glfwPollEvents();
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  igNewFrame();
}

void visWindow_EndFrame() {
  igRender();
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
  glfwSwapBuffers(window_);
}

void visWindow_ChangeFieldOfView(float fovInDegrees) {
  field_of_view_deg_ = fovInDegrees;
  if (field_of_view_deg_ < 1.0f) {
    field_of_view_deg_ = 1.0f;
  }
  if (field_of_view_deg_ > 100.0f) {
    field_of_view_deg_ = 100.0f;
  }

  UpdateProjectionMatrix();
}

const Mat4f* visWindow_GetProjectionMatrix() {
  return &projection_;
}

static void WindowResizeCallback(GLFWwindow *window,
                                 int width,
                                 int height) {
  glViewport(0, 0, width, height);
  window_width_ = width;
  window_height_ = height;
  UpdateProjectionMatrix();
}

void WindowMouseCallback(GLFWwindow* win,
                         double xpos,
                         double ypos) {
  // static const double angle_scale = 0.2;
  // struct ImGuiIO* io = igGetIO();
  // if (mouse_left_pushed && !io->WantCaptureMouse) {
  //   double delta_x = (xpos - prev_mouse_x) * angle_scale;
  //   double delta_y = (ypos - prev_mouse_y) * angle_scale;
  //   prev_mouse_x = xpos;
  //   prev_mouse_y = ypos;
  //   /* As the mouse moves up down on the screen (which is a change of y position) we want to change
  //    * the pitch angle. Thats a rotation around the X axis in OpenGL.
  //    * Likewise, if the moves left/right (change of X position) we want to change the yaw angle. And thats
  //    * a rotation around Y axis in OpenGL */
  //   Cam3_RotateIncrement(&cam, delta_y, 0.0, delta_x);
  // }
}

void WindowKeyboardCallback(GLFWwindow* win,
                                    int key,
                                    int scancode,
                                    int action,
                                    int mods) {
  // if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {  
  //   glfwSetWindowShouldClose(window_, 1);
  // }                                  
  // if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
  //   model.mat[13] += 5;
  //   target_position.y += 5;
  //   Cam3_SetTargetPosition(&cam, &target_position);
  //   // cam.target.z += 5.0f;
  //   // Cam3_SetTargetPosition(&cam, &cam.target);
  // }
  // if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
  //   model.mat[13] -= 5.0f;
  //   target_position.y -= 5;
  //   Cam3_SetTargetPosition(&cam, &target_position);
  //   // cam.target.z -= 5.0f;
  //   // Cam3_SetTargetPosition(&cam, &cam.target);
  // }
  // if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
  //   model.mat[12] -= 5.0f;
  //   target_position.x -= 5;
  //   Cam3_SetTargetPosition(&cam, &target_position);
  //   // cam.target.x -= 5.0f;
  //   // Cam3_SetTargetPosition(&cam, &cam.target);
  // }
  // if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
  //   model.mat[12] += 5.0f;
  //   target_position.x += 5;
  //   Cam3_SetTargetPosition(&cam, &target_position);
  //   // Cam3_SetTargetPosition(&cam, &cam.target);
  // }
  // if (key == GLFW_KEY_R && action == GLFW_PRESS) {
  //   target_position.x = 0;
  //   target_position.y = 0;
  //   target_position.z = 0;
  //   model.mat[12] = 0;
  //   model.mat[13] = 0;
  //   Cam3_SetTargetPosition(&cam, &target_position);
  //   Cam3_SetCameraPosition(&cam, &(Vec3f){.x = 0, .y = 0, .z = 10});
  // }
}

void WindowScrollCallback(GLFWwindow* win,
                                  double xoffset,
                                  double yoffset) {
  field_of_view_deg_ -= yoffset;
  visWindow_ChangeFieldOfView(field_of_view_deg_);
}

void WindowMouseButtonCallback(GLFWwindow* win,
                               int button,
                               int action,
                               int mods) {
  if (!igGetIO()->WantCaptureMouse) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      if (action == GLFW_PRESS) {
        /* Get the mouse position from when the left button is clicked */
        glfwGetCursorPos(win, &prev_mouse_x_, &prev_mouse_y_);
        mouse_left_pushed_ = true;
      } else {
        mouse_left_pushed_ = false;
      }
    }
  }
}

void UpdateProjectionMatrix() {
  projection_ = visProjection_Perspective(field_of_view_deg_,
                                          visProjection_AspectRatio(window_width_, window_height_),
                                          0.01f,
                                          1000.0f);
}