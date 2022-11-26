#ifndef CVIS_INCLUDE_CVIS_VIS_H_
#define CVIS_INCLUDE_CVIS_VIS_H_

#include "cvis/window.h"
#include "cvis/camera3d.h"
#include "cvis/shader.h"
#include "cvis/grid.h"
#include "cvis/waypoints.h"

void vis_PushCamera(const visCamera *camera);

void vis_PushProjection(const Mat4f *projection);

const Mat4f* vis_GetCurrentView();

const Mat4f* vis_GetCurrentProjection();

#endif