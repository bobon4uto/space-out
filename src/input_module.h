#ifdef    MONO_BUILD
#define INPUT_MODULE_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _INPUT_MODULE_H_
#define    _INPUT_MODULE_H_
// input_module interface

#include "rl.h"
/// :fStruct stands for inter`f`ace of a `Struct`
/// :iStruct stands for `i`mplementation of a `Struct`

#include <stdio.h> // TODO: remove dependency
#include <stdlib.h> // TODO: remove dependency
#include <assert.h> // TODO: remove dependency
// :macro
#define BTN_BACK_COLOR        DARKGRAY
#define BTN_PRESSED_COLOR     BLUE
#define BTN_DOWN_COLOR        RED
#define BTN_RELEASED_COLOR    WHITE
#define BTN_UP_COLOR          GRAY
#define TRG_BACK_COLOR        GRAY
#define TRG_PRESS_COLOR       RED
#define JOY_BACK_COLOR        DARKGRAY
#define JOY_FORTH_COLOR       GRAY

#define TODO(mess) do { \
  fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, mess); \
  exit(1); \
} while (0)


typedef enum {
  INPUT_METHOD_KEYBOARD_AND_MOUSE,
  INPUT_METHOD_TOUCH,
  INPUT_METHOD_GAMEPAD,
} InputMethod;

typedef struct sInputDefine {
  int device;
  int button;
} InputDefine;


typedef struct sInputTouchDefine {
  Vector2 position;
  float radius;
  int captured_touch_id;
} InputTouchDefine;

typedef struct sInputLink {
  InputDefine      gamepad;
  InputDefine      keyboard;
  InputTouchDefine touch;
} InputLink;

typedef struct sButtonState {
  InputLink link;
  bool is_down, is_pressed, is_released, is_up;
} ButtonState;

typedef struct sInputJoyLink {
  bool invert_x, invert_y;
  InputDefine      gamepad_x_axis;
  InputDefine      gamepad_y_axis;

  InputDefine      keyboard_l;
  InputDefine      keyboard_u;
  InputDefine      keyboard_r;
  InputDefine      keyboard_d;

  Vector2 position;
  float radius;
  float radius_of_thumb;
  float retract_speed;
  float deadzone;
  int captured_touch_id;
} InputJoyLink;

typedef struct sJoyState {
  InputJoyLink link;
  Vector2 position;
} JoyState;


typedef struct sTriggerState {
  InputLink link;
  float press;
} TriggerState;


typedef enum {

  // usually the d-pad
  BTN_L_FACE_L,
  BTN_L_FACE_U,
  BTN_L_FACE_D,
  BTN_L_FACE_R,

  // usually control buttons
  BTN_R_FACE_L,
  BTN_R_FACE_U,
  BTN_R_FACE_D,
  BTN_R_FACE_R,

  BTN_L_JOY_BUTTON,
  BTN_R_JOY_BUTTON,

  BTN_SELECT,
  BTN_START,
  BTN_SPECIAL,


  BTN_L_BUMPER,
  BTN_R_BUMPER,

  BTN_COUNT,
} ButtonStateEnum;

/// Virtual gamepad that has translated input from actual gamepad
/// or touchscreen gamepad or keyboard and mouse.
typedef struct sVirtualGamepad {

  ButtonState buttons[BTN_COUNT];

  JoyState l_joy_stick;
  JoyState r_joy_stick;

  TriggerState l_trigger;
  TriggerState r_trigger;

} VirtualGamepad;

/// Virtual pointer that has translated input from actual pointer (mouse touch etc) or gamepad or keyboard
/// or touchscreen gamepad or keyboard and mouse.
typedef struct sVirtualPointer {
  Vector2  position;
  ButtonState click;
} VirtualPointer;


/// Abstracted struct that contains virtual gamepad and pointer
typedef struct sInput {
  InputMethod    method;
  bool           virtual_gamepad_active;
  VirtualGamepad gamepad;
  VirtualPointer pointer;
  float delta_time;
} Input;



// :fButtonState
ButtonState button_state_init(int gamepad_button, int keyboard_key, int x, int y, float radius);
void button_state_update(ButtonState* self, Input* input); // reasoning: in touch buttons can claim touch ids so we have to be able to change input
void button_state_draw(ButtonState self);
void button_state_set_gamepad(ButtonState* self, int gamepad);
int button_state_captured_touch_exists( ButtonState* self, Input* input );
int button_state_capture_touch(ButtonState* self, Input* input );
// :fJoyState;
JoyState joy_state_init(
    int gamepad_axis_x,int gamepad_axis_y,
    int keyboard_l,int keyboard_u,int keyboard_r,int keyboard_d,
    int x, int y, float radius
    );
void joy_state_update(JoyState* self, Input* input);
void joy_state_draw(JoyState self);
void joy_state_set_gamepad(JoyState* self, int gamepad);
int joy_state_captured_touch_exists( JoyState* self, Input* input );
int joy_state_capture_touch(JoyState* self, Input* input );

// :fTriggerState;
TriggerState trigger_state_init(int gamepad_axis, int keyboard_key,int x,int y, int height);
void trigger_state_update(TriggerState* self, Input* input);
void trigger_state_draw(TriggerState self);
void trigger_state_set_gamepad(TriggerState* self, int gamepad);
int trigger_state_captured_touch_exists( TriggerState* self, Input* input );
int trigger_state_capture_touch(TriggerState* self, Input* input );

// :fVirtualGamepad
VirtualGamepad virtual_gamepad_init_default();
void virtual_gamepad_update(VirtualGamepad* self, Input* input);
void virtual_gamepad_draw(VirtualGamepad self);
// :fVirtualPointer
VirtualPointer virtual_pointer_init_default();
void virtual_pointer_update(VirtualPointer* self, Input* input);
void virtual_pointer_draw(VirtualPointer self);
int virtual_pointer_captured_touch_exists(VirtualPointer* self, Input* input);
int virtual_pointer_capture_touch(VirtualPointer* self, Input* input);
// :fInput
Input input_init_default();
void input_deduce_input_method(Input* self);
void input_update(Input* self);
void input_draw(Input self);
void input_set_gamepad_to_all(Input* self, int gamepad);
bool input_is_captured(Input* self, int id);

#ifdef      INPUT_MODULE_IMPLEMENTATION
// input_module implementation
// :impl
static bool vector2_is_zero(Vector2 v) {
  return vector2_equals(v, vector2_zero());
}
static Vector2 vector2_normclamp(Vector2 unfixed, float deadzone, float maximum) {
  float distance = vector2_distance(vector2_zero(), unfixed);
  if (distance < deadzone) {
    return vector2_zero();
  }
  if (distance > maximum) {
    return vector2_scale(vector2_normalize(unfixed), maximum);
  }
  return unfixed;
}

float float_move_towards(float f, float target, float max_distance) {
    float result = 0.0f;

    float dist = target - f;

    if ((dist == 0) || ((max_distance >= 0) && (fabs(dist) <= max_distance))) return target;

    if (f<target) {
      result = f + max_distance;
    } else {
      result = f - max_distance;
    }

    return result;
}



// :iButtonState
ButtonState button_state_init(int gamepad_button, int keyboard_key, int x, int y, float radius) {
  ButtonState self = {0};
  self.link.touch.position.x = (float)x;
  self.link.touch.position.y = (float)y;
  self.link.touch.radius = radius;
  self.link.gamepad.device = 0;
  self.link.gamepad.button = gamepad_button;
  self.link.keyboard.device = 0;
  self.link.keyboard.button = keyboard_key;

  self.link.touch.captured_touch_id = -1;

  return self;
}
void button_state_update(ButtonState* self, Input* input) {
  switch (input->method) {
    case INPUT_METHOD_KEYBOARD_AND_MOUSE: {
      self->is_down     = is_key_down    (self->link.keyboard.button);
      self->is_pressed  = is_key_pressed (self->link.keyboard.button);
      self->is_released = is_key_released(self->link.keyboard.button);
      self->is_up       = is_key_up      (self->link.keyboard.button);
    } break;
    case INPUT_METHOD_TOUCH: {
      int captured_index = button_state_captured_touch_exists( self, input );
      if ( captured_index == -1 ) {
        captured_index = button_state_capture_touch( self, input );
      }
      if ( captured_index != -1 ) {
        // NOTE: button is "sticky" - it will not let go of touch even if it leaves the radius. Only when you let go completely.
        self->is_pressed = self->is_up;
        self->is_up = false;
        self->is_released = false;
        self->is_down = true;
      } else {
        self->is_released = self->is_down;
        self->is_down = false;
        self->is_pressed = false;
        self->is_up = true;
      }
    } break;
    case INPUT_METHOD_GAMEPAD: {
      self->is_down     = is_gamepad_button_down    (self->link.gamepad.device, self->link.gamepad.button);
      self->is_pressed  = is_gamepad_button_pressed (self->link.gamepad.device, self->link.gamepad.button);
      self->is_released = is_gamepad_button_released(self->link.gamepad.device, self->link.gamepad.button);
      self->is_up       = is_gamepad_button_up      (self->link.gamepad.device, self->link.gamepad.button);
    } break;
  }
}
void button_state_draw(ButtonState self) {
  int x = (int)self.link.touch.position.x;
  int y = (int)self.link.touch.position.y;
  float radius = self.link.touch.radius;

  draw_circle(x, y, radius, BTN_BACK_COLOR);
  if (self.is_up) {
    draw_circle(x, y, radius, BTN_UP_COLOR);
  }
  if (self.is_down) {
    draw_circle(x, y, radius, BTN_DOWN_COLOR);
  }
  if (self.is_pressed) {
    draw_circle(x, y, radius, BTN_PRESSED_COLOR);
  }
  if (self.is_released) {
    draw_circle(x, y, radius, BTN_RELEASED_COLOR);
  }

}
void button_state_set_gamepad(ButtonState* self, int gamepad) {
  self->link.gamepad.device = gamepad;
}
int button_state_captured_touch_exists( ButtonState* self, Input* input ) {
  (void)input;
  int count = get_touch_point_count();
  if (self->link.touch.captured_touch_id==-1) {
    return -1;
  }

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if (id == self->link.touch.captured_touch_id) {
      return i;
    }
  }

  self->link.touch.captured_touch_id = -1;
  return -1;
}
int button_state_capture_touch(ButtonState* self, Input* input ) {
  int count = get_touch_point_count();

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if ( !input_is_captured(input, id) ) {
      Vector2 touch_pos = get_touch_position(i);
      float distance = vector2_distance(self->link.touch.position, touch_pos);
      if (distance < self->link.touch.radius) {
        self->link.touch.captured_touch_id = id;
        return i;
      }
    } else {
    }
  }

  self->link.touch.captured_touch_id = -1;
  return -1;
}



// :iJoyState;
JoyState joy_state_init(
    int gamepad_axis_x,int gamepad_axis_y,
    int keyboard_l,int keyboard_u,int keyboard_r,int keyboard_d,
    int x, int y, float radius
    ) {
  JoyState self = {0};
  self.link.gamepad_x_axis.device = 0;
  self.link.gamepad_x_axis.button = gamepad_axis_x;

  self.link.gamepad_y_axis.device = 0;
  self.link.gamepad_y_axis.button = gamepad_axis_y;

  self.link.keyboard_l.device = 0;
  self.link.keyboard_l.button = keyboard_l;
  self.link.keyboard_u.device = 0;
  self.link.keyboard_u.button = keyboard_u;
  self.link.keyboard_r.device = 0;
  self.link.keyboard_r.button = keyboard_r;
  self.link.keyboard_d.device = 0;
  self.link.keyboard_d.button = keyboard_d;

  self.link.position = (Vector2){x,y};
  self.link.radius   = radius;
  self.link.retract_speed = 30.0f;

  self.link.deadzone = 0.2f;
  self.link.captured_touch_id = -1;

  return self;
}
void joy_state_update(JoyState* self, Input* input) {
  switch (input->method) {
    case INPUT_METHOD_KEYBOARD_AND_MOUSE: {
      Vector2 move_position = {0};
      move_position.x = 0.0f;
      move_position.y = 0.0f;

      move_position.x -= is_key_down(self->link.keyboard_l.button) ? 1.0f : 0.0f;
      move_position.x += is_key_down(self->link.keyboard_r.button) ? 1.0f : 0.0f;
      move_position.y -= is_key_down(self->link.keyboard_u.button) ? 1.0f : 0.0f;
      move_position.y += is_key_down(self->link.keyboard_d.button) ? 1.0f : 0.0f;

      if ( vector2_equals( self->position, vector2_zero() ) && !vector2_equals(move_position, vector2_zero() ) ) {
        self->position = vector2_scale(move_position, self->link.deadzone);
      }
      self->position = vector2_move_towards(self->position, move_position, self->link.retract_speed*input->delta_time);

    } break;
    case INPUT_METHOD_TOUCH: {
      int captured_index = joy_state_captured_touch_exists( self, input );
      if ( captured_index == -1 ) {
        captured_index = joy_state_capture_touch( self, input );
      }
      if ( captured_index != -1 ) {
        Vector2 touch_pos = get_touch_position(captured_index);
        Vector2 dir_vec = vector2_scale( vector2_subtract( touch_pos, self->link.position ), 1/self->link.radius );
        self->position = dir_vec;
      } else {
        self->position = vector2_move_towards(self->position, vector2_zero(), self->link.retract_speed*input->delta_time);
      }
    } break;
    case INPUT_METHOD_GAMEPAD: {
      self->position.x = get_gamepad_axis_movement(self->link.gamepad_x_axis.device, self->link.gamepad_x_axis.button);
      self->position.y = get_gamepad_axis_movement(self->link.gamepad_y_axis.device, self->link.gamepad_y_axis.button);
    } break;
  }
  self->position = vector2_normclamp(self->position, self->link.deadzone, 1.0f);
}
void joy_state_draw(JoyState self) {
  draw_circle_v(self.link.position, self.link.radius, JOY_BACK_COLOR);
  draw_circle_v( vector2_add( vector2_scale(self.position, self.link.radius), self.link.position), self.link.radius*0.3f, JOY_FORTH_COLOR);
}
void joy_state_set_gamepad(JoyState* self, int gamepad) {
  self->link.gamepad_x_axis.device = gamepad;
  self->link.gamepad_y_axis.device = gamepad;
}
int joy_state_captured_touch_exists( JoyState* self, Input* input ) {
  (void)input;
  int count = get_touch_point_count();
  if (self->link.captured_touch_id==-1) {
    return -1;
  }

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if (id == self->link.captured_touch_id) {
      return i;
    }
  }

  self->link.captured_touch_id = -1;
  return -1;
}
int joy_state_capture_touch(JoyState* self, Input* input ) {
  int count = get_touch_point_count();

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if ( !input_is_captured(input, id) ) {
      Vector2 touch_pos = get_touch_position(i);
      float distance = vector2_distance(self->link.position, touch_pos);
      if (distance < self->link.radius) {
        self->link.captured_touch_id = id;
        return i;
      }
    } else {
    }
  }

  self->link.captured_touch_id = -1;
  return -1;
}
// :iTriggerState;
TriggerState trigger_state_init(int gamepad_axis, int keyboard_key,int x,int y, int height) {
  TriggerState self = {0};

  self.link.gamepad.device = 0;
  self.link.gamepad.button = gamepad_axis;

  self.link.keyboard.device = 0;
  self.link.keyboard.button = keyboard_key;

  self.link.touch.position.x = x;
  self.link.touch.position.y = y;
  self.link.touch.radius = height;

  self.link.touch.captured_touch_id = -1;

  return self;
}
void trigger_state_update(TriggerState* self, Input* input) {
  switch (input->method) {
    case INPUT_METHOD_KEYBOARD_AND_MOUSE: {
      self->press = is_key_down(self->link.keyboard.button) ? 1.0f : 0.0f;
    } break;
    case INPUT_METHOD_TOUCH: {
      int captured_index = trigger_state_captured_touch_exists( self, input );
      if ( captured_index == -1 ) {
        captured_index = trigger_state_capture_touch( self, input );
      }
      if ( captured_index != -1 ) {
        // NOTE: trigger is sticky.
        self->press = float_move_towards(self->press, 1.0f, 10.0f*input->delta_time ); //TODO: add config instead of 10.0f
      } else {
        self->press = float_move_towards(self->press, 0.0f, 10.0f*input->delta_time ); //TODO: add config instead of 10.0f
      }
    } break;
    case INPUT_METHOD_GAMEPAD: {
      self->press = get_gamepad_axis_movement(self->link.gamepad.device, self->link.gamepad.button);
    } break;
  }
}
void trigger_state_draw(TriggerState self) {
  int x = (int)self.link.touch.position.x;
  int y = (int)self.link.touch.position.y;
  float height = self.link.touch.radius;

  draw_rectangle(x - height/4, y-height, height/2, height, TRG_BACK_COLOR );
  draw_rectangle(x - height/4, (float)y-(height*self.press), height/2, self.press*height, TRG_PRESS_COLOR );
}
void trigger_state_set_gamepad(TriggerState* self, int gamepad) {
  self->link.gamepad.device = gamepad;
}
int trigger_state_captured_touch_exists( TriggerState* self, Input* input ) {
  (void)input;
  int count = get_touch_point_count();
  if (self->link.touch.captured_touch_id==-1) {
    return -1;
  }

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if (id == self->link.touch.captured_touch_id) {
      return i;
    }
  }

  self->link.touch.captured_touch_id = -1;
  return -1;
}
int trigger_state_capture_touch(TriggerState* self, Input* input ) {
  int count = get_touch_point_count();

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if ( !input_is_captured(input, id) ) {
      Vector2 touch_pos = get_touch_position(i);

      int x = (int)self->link.touch.position.x;
      int y = (int)self->link.touch.position.y;
      float height = self->link.touch.radius;

      if (check_collision_point_rec(touch_pos, (Rectangle){x - height/4, y-height, height/2, height }) ) {
        self->link.touch.captured_touch_id = id;
        return i;
      }
    } else {
    }
  }

  self->link.touch.captured_touch_id = -1;
  return -1;
}

// :iVirtualGamepad
VirtualGamepad virtual_gamepad_init_default() {
  VirtualGamepad self = {0};
  float s = 20.0f;
  self.buttons[BTN_L_BUMPER] = button_state_init(GAMEPAD_BUTTON_LEFT_TRIGGER_1, KEY_Q, s*6, s*4, s);
  self.buttons[BTN_R_BUMPER] = button_state_init(GAMEPAD_BUTTON_RIGHT_TRIGGER_1, KEY_E, s*(10+6), s*4, s);

  self.buttons[BTN_SELECT]   = button_state_init(GAMEPAD_BUTTON_MIDDLE_LEFT, KEY_TAB, s*8.5f, s*14, s);
  self.buttons[BTN_SPECIAL]  = button_state_init(GAMEPAD_BUTTON_MIDDLE, KEY_GRAVE, s*11, s*14, s);
  self.buttons[BTN_START]    = button_state_init(GAMEPAD_BUTTON_MIDDLE_RIGHT, KEY_SPACE, s*13.5f, s*14, s);

  self.buttons[BTN_L_FACE_L] = button_state_init(GAMEPAD_BUTTON_LEFT_FACE_LEFT, KEY_LEFT, s*4, s*10, s);
  self.buttons[BTN_L_FACE_U] = button_state_init(GAMEPAD_BUTTON_LEFT_FACE_UP,   KEY_UP, s*6, s*8, s);
  self.buttons[BTN_L_FACE_D] = button_state_init(GAMEPAD_BUTTON_LEFT_FACE_DOWN, KEY_DOWN, s*6, s*12, s);
  self.buttons[BTN_L_FACE_R] = button_state_init(GAMEPAD_BUTTON_LEFT_FACE_RIGHT,KEY_RIGHT, s*8, s*10, s);

  self.buttons[BTN_R_FACE_L] = button_state_init(GAMEPAD_BUTTON_RIGHT_FACE_LEFT, KEY_ONE,  s*10 + s*4, s*10, s);
  self.buttons[BTN_R_FACE_U] = button_state_init(GAMEPAD_BUTTON_RIGHT_FACE_UP, KEY_I,    s*10 + s*6, s*8, s);
  self.buttons[BTN_R_FACE_D] = button_state_init(GAMEPAD_BUTTON_RIGHT_FACE_DOWN, KEY_TWO,  s*10 + s*6, s*12, s);
  self.buttons[BTN_R_FACE_R] = button_state_init(GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, KEY_THREE, s*10 + s*8, s*10, s);

  self.buttons[BTN_L_JOY_BUTTON] = button_state_init(GAMEPAD_BUTTON_LEFT_THUMB,  KEY_X,  s*6, s*24, s);
  self.buttons[BTN_R_JOY_BUTTON] = button_state_init(GAMEPAD_BUTTON_RIGHT_THUMB, KEY_COMMA, s*10 + s*6, s*24, s);

  self.l_joy_stick = joy_state_init(
      GAMEPAD_AXIS_LEFT_X,GAMEPAD_AXIS_LEFT_Y,
      KEY_A, KEY_W, KEY_D, KEY_S,
      s*6, s*18, 64.0f
      );

  self.r_joy_stick = joy_state_init(
      GAMEPAD_AXIS_RIGHT_X,GAMEPAD_AXIS_RIGHT_Y,
      KEY_J, KEY_I, KEY_L, KEY_K,
      s*16, s*18, 64.0f
      );

  self.l_trigger = trigger_state_init(GAMEPAD_AXIS_LEFT_TRIGGER, KEY_ONE , s*4, s*4, 60);
  self.r_trigger = trigger_state_init(GAMEPAD_AXIS_RIGHT_TRIGGER, KEY_THREE , s*18, s*4, 60);

  return self;
}
void virtual_gamepad_update(VirtualGamepad* self, Input* input) {

  for (int i = 0; i < BTN_COUNT; ++i) {
    button_state_update(self->buttons+i, input);
  }
  joy_state_update(&self->l_joy_stick, input);
  joy_state_update(&self->r_joy_stick, input);

  trigger_state_update(&self->l_trigger, input);
  trigger_state_update(&self->r_trigger, input);
}
void virtual_gamepad_draw(VirtualGamepad self) {
  for (int i = 0; i < BTN_COUNT; ++i) {
    button_state_draw(self.buttons[i]);
  }
  joy_state_draw(self.l_joy_stick);
  joy_state_draw(self.r_joy_stick);

  trigger_state_draw(self.l_trigger);
  trigger_state_draw(self.r_trigger);
}
// :iVirtualPointer
VirtualPointer virtual_pointer_init_default() {
  VirtualPointer self = {0};
  return self;
}
void virtual_pointer_update(VirtualPointer* self, Input* input) {
  switch (input->method) {
    case INPUT_METHOD_KEYBOARD_AND_MOUSE: {
      self->position          = get_mouse_position();
      self->click.is_down     = is_mouse_button_down    (MOUSE_BUTTON_LEFT);
      self->click.is_pressed  = is_mouse_button_pressed (MOUSE_BUTTON_LEFT);
      self->click.is_released = is_mouse_button_released(MOUSE_BUTTON_LEFT);
      self->click.is_up       = is_mouse_button_up      (MOUSE_BUTTON_LEFT);
    } break;
    case INPUT_METHOD_TOUCH: {
      int captured_index = virtual_pointer_captured_touch_exists( self, input );
      if ( captured_index == -1 ) {
        captured_index = virtual_pointer_capture_touch( self, input );
      }
      if ( captured_index != -1 ) {
        Vector2 touch_pos = get_touch_position(captured_index);
        self->position = touch_pos;
      } else {
      }

      if ( captured_index != -1 ) {
        self->click.is_pressed = self->click.is_up;
        self->click.is_up = false;
        self->click.is_released = false;
        self->click.is_down = true;
      } else {
        self->click.is_released = self->click.is_down;
        self->click.is_down = false;
        self->click.is_pressed = false;
        self->click.is_up = true;
      }
    } break;
    case INPUT_METHOD_GAMEPAD: {
      // TODO("INPUT_METHOD_GAMEPAD");
    } break;
  }

}
void virtual_pointer_draw(VirtualPointer self) {
  self.click.link.touch.position = self.position;
  self.click.link.touch.radius = 10.0f;
  button_state_draw(self.click);
}
int virtual_pointer_captured_touch_exists(VirtualPointer* self, Input* input) {
  (void)input;
  int count = get_touch_point_count();
  if (self->click.link.touch.captured_touch_id==-1) {
    return -1;
  }

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if (id == self->click.link.touch.captured_touch_id) {
      return i;
    }
  }

  self->click.link.touch.captured_touch_id = -1;
  return -1;
}
int virtual_pointer_capture_touch(VirtualPointer* self, Input* input) {
  int count = get_touch_point_count();

  for (int i=0; i<count; ++i) {
    int id = get_touch_point_id(i);
    if ( !input_is_captured(input, id) ) {
      Vector2 touch_pos = get_touch_position(i);
      self->click.link.touch.captured_touch_id = id;
      self->position = touch_pos;
      return i;
    } else {
    }
  }

  self->click.link.touch.captured_touch_id = -1;
  return -1;

}
// :iInput
Input input_init_default() {
  Input self = {0};

  self.method = INPUT_METHOD_KEYBOARD_AND_MOUSE;
  self.gamepad = virtual_gamepad_init_default();
  self.pointer = virtual_pointer_init_default();


  return self;
}

void input_deduce_input_method(Input* self) {
  // should be called on logo screen update or something similar where user will make first inputs.

  if (get_key_pressed() != 0) {
    self->method = INPUT_METHOD_KEYBOARD_AND_MOUSE;
  }

  if ( !vector2_is_zero( get_mouse_delta() ) ) {
    if (self->method != INPUT_METHOD_TOUCH) {
      self->method = INPUT_METHOD_KEYBOARD_AND_MOUSE;
    }
  }

    #define MAX_GAMEPADS 4
    #define MAX_GAMEPAD_BUTTONS 32
  for (int ig = 0; ig<MAX_GAMEPADS; ++ig) {
    if (is_gamepad_available(ig)) {
      for (int i = 0; i<MAX_GAMEPAD_BUTTONS; ++i) {
        if ( is_gamepad_button_pressed(ig, i) ) {
          self->method = INPUT_METHOD_GAMEPAD;
          input_set_gamepad_to_all(self, ig);
        }
      }
    }
  }
  if (get_touch_point_count() > 0) {
    self->method = INPUT_METHOD_TOUCH;
  }
}
void input_update(Input* self) {
  self->delta_time = get_frame_time();
  virtual_gamepad_update(&self->gamepad, self);
  virtual_pointer_update(&self->pointer, self); // MUST be after for touch to work.
}
void input_draw(Input self) {
  virtual_gamepad_draw(self.gamepad);
  virtual_pointer_draw(self.pointer);
  int count = get_touch_point_count();
  for (int i =0; i<count; ++i) {
    Vector2 pos = get_touch_position(i);
    draw_text( text_format("%d(%d)",i, get_touch_point_id(i) ), pos.x+50, pos.y-50, 10, WHITE );
  }
}
void input_set_gamepad_to_all(Input* self, int gamepad) {
  for (int i = 0; i < BTN_COUNT; ++i) {
    button_state_set_gamepad(&self->gamepad.buttons[i], gamepad);
  }
  joy_state_set_gamepad(&self->gamepad.l_joy_stick, gamepad);
  joy_state_set_gamepad(&self->gamepad.r_joy_stick, gamepad);

  trigger_state_set_gamepad(&self->gamepad.l_trigger, gamepad);
  trigger_state_set_gamepad(&self->gamepad.r_trigger, gamepad);
}
bool input_is_captured(Input* self, int id) {

  for (int i = 0; i<BTN_COUNT; ++i) {
    if (self->gamepad.buttons[i].link.touch.captured_touch_id == id) { return true; }
  }

  if (self->gamepad.l_joy_stick.link.captured_touch_id == id) { return true; }
  if (self->gamepad.r_joy_stick.link.captured_touch_id == id) { return true; }

  if (self->gamepad.r_trigger.link.touch.captured_touch_id == id) { return true; }
  if (self->gamepad.l_trigger.link.touch.captured_touch_id == id) { return true; }

  if (self->pointer.click.link.touch.captured_touch_id==id) { return true; }

  return false;
}


#endif   // INPUT_MODULE_IMPLEMENTATION
#endif   //_INPUT_MODULE_H_

