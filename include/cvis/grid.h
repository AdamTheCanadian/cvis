#ifndef CVIS_INCLUDE_CVIS_GRID_H_
#define CVIS_INCLUDE_CVIS_GRID_H_

#include "cvis/camera3d.h"

void visGrid_InitDefault();

void visGrid_InitWithSpacing(float spacing);

void visGrid_Draw(const Mat4f *view,
                  const Mat4f *projection);

#endif
