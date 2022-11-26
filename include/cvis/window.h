#ifndef CVIS_INCLUDE_CVIS_WINDOW_H_
#define CVIS_INCLUDE_CVIS_WINDOW_H_

#include <stdbool.h>
#include "cmat/mat4f.h"

/* Include ImGui here so any users of window.h automatically get imgui function */
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"

bool visWindow_Initialize(const char *windowName,
                          int width,
                          int height);

bool visWindow_ShouldClose();

void visWindow_NewFrame();

void visWindow_EndFrame();

void visWindow_ChangeFieldOfView(float fovInDegrees);

const Mat4f* visWindow_GetProjectionMatrix();
#endif