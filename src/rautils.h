#ifdef    MONO_BUILD
#define RAUTILS_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _RAUTILS_H_
#define    _RAUTILS_H_
// rautils interface

#include "rl.h"

// :macro

// :type

// :func

// :fNum
float rul_max_float(float self, float other);
float rul_min_float(float self, float other);

// :fTexture
void rul_texture_draw_centered_inside_rectangle(Texture tex, Rectangle rect);
void rul_texture_draw_in_circle( Texture tex, Vector2 center, float radius);


// :fRectangle
#ifdef EXAMPLE
        +---+
 +-+    |   |
 |0| -> | 0 |
 +-+    |   |
        +---+
#endif // EXAMPLE
Rectangle rul_rectangle_extrude(Rectangle rect, float extrusion);
Rectangle rul_rectangle_intrude(Rectangle rect, float intrusion);
// effectively treats left top as 0,0 (no change in normal drawing)
Rectangle rul_rectangle_of_left_top(float screen_x_, float screen_y_, Rectangle rect);
// effectively treats right top as 0,0 (x is flipped)
Rectangle rul_rectangle_of_right_top(float screen_x_, float screen_y_, Rectangle rect);

Rectangle rul_rectangle_by_constraint(Rectangle base, float x_start, float x_end, float y_start, float y_end);


Vector2 rul_rectangle_pos(Rectangle rec);

Vector2 rul_rectangle_center_text_inside_offset(Font font, Rectangle rec, const char* text, float font_size);

Vector2 rul_circle_center_text_inside(Font font, Vector2 center, const char* text, float font_size);
Vector2 rul_circle_center_text_under(Font font, Vector2 center, float radius, const char* text, float font_size);


#ifdef      RAUTILS_IMPLEMENTATION
// rautils implementation

// :impl

// :iNum
float rul_max_float(float self, float other) {
  return self > other ? self : other;
}
float rul_min_float(float self, float other) {
  return self < other ? self : other;
}
float rul_float_get_ration(float on, float under) {
  return on / under; // insane math
}


// :iTexture
void rul_texture_draw_centered_inside_rectangle(Texture tex, Rectangle rect) {
  float tex_x_ = tex.width;
  float tex_y_ = tex.height;

  float rect_x_ = rect.width;
  float rect_y_ = rect.height;

  float scale_by = 1.0f;
  if (tex_x_ > rect_x_ || tex_y_ > rect_y_) {
    scale_by = 1/rul_max_float( tex_x_/rect_x_, tex_y_/rect_y_ );
  }
  float scaled_tex_x_ = tex_x_*scale_by;
  float scaled_tex_y_ = tex_y_*scale_by;

  float offset_x_ = (rect_x_ - scaled_tex_x_)/2;
  float offset_y_ = (rect_y_ - scaled_tex_y_)/2;
  Rectangle srcrec = (Rectangle) { 0,0,tex_x_,tex_y_ };
  Rectangle dstrec = (Rectangle) { rect.x+offset_x_, rect.y+offset_y_,scaled_tex_x_,scaled_tex_y_ };
  draw_texture_pro(tex, srcrec, dstrec, vector2_zero(), 0.0f, WHITE);


}
void rul_texture_draw_in_circle( Texture tex, Vector2 center, float radius) {
  float diameter = radius*2;
  float tex_x_ = tex.width;
  float tex_y_ = tex.height;
  if (tex_x_ < 0.1f) {
    tex_x_ = 0.1f;
  }
  if (tex_y_ < 0.1f) {
    tex_y_ = 0.1f;
  }
  float scale_by = rul_min_float( diameter/tex_x_, diameter/tex_y_);
  float scaled_tex_x_ = tex_x_*scale_by;
  float scaled_tex_y_ = tex_y_*scale_by;

  float corner_x = center.x - radius;
  float corner_y = center.y - radius;

  float offset_x_ = (diameter - scaled_tex_x_)/2;
  float offset_y_ = (diameter - scaled_tex_y_)/2;

  Rectangle srcrec = (Rectangle) { 0,0,tex_x_,tex_y_ };
  Rectangle dstrec = (Rectangle) { corner_x+offset_x_, corner_y+offset_y_,scaled_tex_x_,scaled_tex_y_ };
  draw_texture_pro(tex, srcrec, dstrec, vector2_zero(), 0.0f, WHITE);

  
}




// :iRectangle
Rectangle rul_rectangle_of_left_top(float screen_x_, float screen_y_, Rectangle rect) {
  (void)(screen_x_);
  (void)(screen_y_);
  return rect;
}
Rectangle rul_rectangle_of_right_top(float screen_x_, float screen_y_, Rectangle rect) {
  (void)(screen_y_);
  rect.x = screen_x_ - rect.x - rect.width;
  return rect;
}
Rectangle rul_rectangle_by_constraint(Rectangle base, float x_start, float x_end, float y_start, float y_end) {
  Rectangle self = {0};
  self.x = base.x + base.width * x_start;
  self.width = base.width * (x_end - x_start);

  self.y = base.y + base.height* y_start;
  self.height = base.height * (y_end - y_start);
  return self;
}
Vector2 rul_rectangle_pos(Rectangle rec) {
  return (Vector2){rec.x, rec.y};
}
Vector2 rul_rectangle_center_text_inside_offset(Font font, Rectangle rec, const char* text, float font_size) {
  Vector2 size =  measure_text_ex(font, text, font_size, font_size/10);
  return (Vector2){ (rec.width - size.x) / 2 + rec.x, (rec.height - size.y) / 2 + rec.y };
}
Vector2 rul_circle_center_text_under(Font font, Vector2 center, float radius, const char* text, float font_size) {
  Vector2 size =  measure_text_ex(font, text, font_size, font_size/10);
  return (Vector2){center.x - size.x/2,  center.y + radius};
}
Vector2 rul_circle_center_text_inside(Font font, Vector2 center, const char* text, float font_size) {
  Vector2 size =  measure_text_ex(font, text, font_size, font_size/10);
  return (Vector2){center.x - size.x/2,  center.y - size.y/2};
}


#endif   // RAUTILS_IMPLEMENTATION
#endif   //_RAUTILS_H_

