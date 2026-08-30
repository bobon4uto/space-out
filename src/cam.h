#ifdef    MONO_BUILD
#define CAM_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _CAM_H_
#define    _CAM_H_
// cam interface

#include "rl.h"
#include "unit.h"

Camera2D cam_init(int screen_x_, int screen_y_);
void cam_update(Camera2D* self, Vector2 target);

#ifdef      CAM_IMPLEMENTATION
// cam implementation

Camera2D cam_init(int screen_x_, int screen_y_) {
  (void)screen_x_;
  (void)screen_y_;
  Camera2D self = {0};
  //self.offset =   (Vector2){screen_x_/2, screen_y_/2};
  self.offset =   (Vector2){0.0f, 0.0f};
  self.target = (Vector2){0.0f,0.0f};
  self.rotation = 0.0f;
  self.zoom = 1.0f;
  return self;
}
void cam_update(Camera2D* self, Vector2 target) {
  self->target = target;
}


#endif   // CAM_IMPLEMENTATION
#endif   //_CAM_H_

