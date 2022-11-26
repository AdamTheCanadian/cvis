#ifndef CVIS_INCLUDE_CVIS_ROBOT_H_
#define CVIS_INCLUDE_CVIS_ROBOT_H_

void visRobot_Init(double length,
                   double width,
                   double height);

void visRobot_Draw();

void visRobot_ChangeSize(double length,
                         double width,
                         double height);

void visRobot_UpdatePosition(Vec3f pose);

#endif
