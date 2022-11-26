#include "cvis/vis.h"

static const Mat4f *current_view_;
static const Mat4f *current_projection_;

void vis_PushCamera(const visCamera *camera) {
  current_view_ = &camera->view;
}

void vis_PushProjection(const Mat4f *projection) {
  current_projection_ = projection;
}

const Mat4f* vis_GetCurrentView() {
  return current_view_;
}

const Mat4f* vis_GetCurrentProjection() {
  return current_projection_;
}