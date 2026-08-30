#ifdef    MONO_BUILD
#define MENU_MANAGER_IMPLEMENTATION
#undef    MONO_BUILD
#endif // MONO_BUILD
#ifndef    _MENU_MANAGER_H_
#define    _MENU_MANAGER_H_
// menu_manager interface

#include "input_module.h"
#include "level_select.h"
#include "asset_bank.h"
#include "rautils.h"
#include "rl.h"
#include "no.util/core.h"

// :type
typedef enum {
  MENU_TITLE,
  MENU_CREDITS,
  MENU_LEVEL_SELECT,
  MENU_GAMEPLAY,
  MENU_COUNT,
} MenuID;

#define UI_LVL_BTN(T, NUM, X_START, X_END, Y_START, Y_END) \
  T(UI_BUTTON_LEVEL_SELECT_##NUM, MENU_LEVEL_SELECT, \
      X_START, X_END, Y_START, Y_END, #NUM, {gs->level_select.level = LEVEL_##NUM; level_reload(gs_get_current_level(gs), gs->level_select.sources[LEVEL_##NUM] ); gs->menu.current=MENU_GAMEPLAY; } )

// Im very sorry to anyone reading that bloat
//
#define UI_BUTTONS(T) \
  T(UI_BUTTON_TITLE_PLAY,            MENU_TITLE, 0.2, 0.8, 0.48, 0.68, "Start", {gs->menu.current=MENU_LEVEL_SELECT;} ) \
  T(UI_BUTTON_TITLE_CREDITS,         MENU_TITLE, 0.2, 0.8, 0.7, 0.9, "Credits", {gs->menu.current=MENU_CREDITS;} ) \
  T(UI_BUTTON_CREDITS_BACK,        MENU_CREDITS, 0.2, 0.8, 0.7, 0.9, "Back",    {gs->menu.current=MENU_TITLE;} ) \
  T(UI_BUTTON_CREDITS_LICENSE,        MENU_CREDITS, 0.2, 0.5, 0.1, 0.2, "copy license link",    {set_clipboard_text("https://creativecommons.org/licenses/by/4.0/");} ) \
  T(UI_BUTTON_CREDITS_MUSIC,        MENU_CREDITS, 0.5, 0.8, 0.1, 0.2, "copy music link",    {set_clipboard_text("https://opengameart.org/content/lunar-shop-trevor-lentz-cleyton-kauffman-migfus-francisleemusic");} ) \
  UI_LVL_BTN(T, TUTORIAL, 0.3, 0.7, 0.1, 0.2) \
  UI_LVL_BTN(T, 1, 0.2, 0.3, 0.2, 0.3) \
  UI_LVL_BTN(T, 2, 0.35, 0.45, 0.28, 0.38) \
  UI_LVL_BTN(T, 3, 0.5, 0.6, 0.36, 0.46) \
  UI_LVL_BTN(T, 4, 0.65, 0.75, 0.44, 0.54) \
  UI_LVL_BTN(T, 5, 0.5, 0.6, 0.52, 0.62) \
  T(UI_BUTTON_LEVEL_SELECT_CUSTOM, MENU_LEVEL_SELECT, \
      0.2, 0.4, 0.44, 0.54, "CUSTOM", { \
      char* src=(char*)get_clipboard_text(); \
      gs->level_select.level = LEVEL_CUSTOM; \
      if (src) { \
        level_reload(gs_get_current_level(gs), sv_from_str(src) ); \
      } \
      gs->menu.current=MENU_GAMEPLAY; \
      } ) \
  T(UI_BUTTON_LEVEL_SELECT_BACK,MENU_LEVEL_SELECT, 0.2, 0.8, 0.7, 0.9, "Back",    {gs->menu.current=MENU_TITLE;} ) \
  T(UI_BUTTON_GAMEPLAY_TO_LEVEL_SELECT,MENU_GAMEPLAY, 0.05, 0.2, 0.01, 0.04, "Levels",    { \
      gs->menu.current=MENU_LEVEL_SELECT; \
      level_free(gs_get_current_level(gs)); \
      } ) \
  T(UI_BUTTON_GAMEPLAY_SHARE,MENU_GAMEPLAY, 0.85, 0.95, 0.01, 0.04, "share",    { \
      level_save(gs_get_current_level(gs)); \
      } ) \
  T(UI_BUTTON_GAMEPLAY_FRAME,MENU_GAMEPLAY, 0.05, 0.95, 0.05, 0.83, "LEVEL",    {gs->placing=true;} ) \
  T(UI_BUTTON_GAMEPLAY_WIRE,MENU_GAMEPLAY, 0.05, 0.35, 0.85, 0.99, "",     {gs->selected=TOOL_WIRE;} ) \
  T(UI_BUTTON_GAMEPLAY_SAT,MENU_GAMEPLAY, 0.35, 0.65, 0.85, 0.99, "",       {gs->selected=TOOL_SAT;} ) \
  T(UI_BUTTON_GAMEPLAY_ERASE,MENU_GAMEPLAY, 0.65, 0.95, 0.85, 0.99, "",    {gs->selected=TOOL_ERASE;} ) \
// end macro
#define UI_BUTTON_ENUM_UNWRAP(ID, ACTIVE_AT, X_START, X_END, Y_START, Y_END, THE_TEXT, THE_CODE) ID,


typedef enum {
  UI_BUTTONS(UI_BUTTON_ENUM_UNWRAP)
  UI_BUTTON_COUNT,
} UIButtonID;

typedef struct sUIButton {
  MenuID active_on;
  Rectangle rec;
  StringBuilder text;
  SpriteID sprite_id;
  ButtonState state;
  bool hover;
} UIButton;

typedef struct sMenuManager {
  UIButton buttons[UI_BUTTON_COUNT];
  MenuID current;
} MenuManager;

// :func

// :fUIButton
bool ui_button_is_pressed(UIButton self, Input input);
void ui_button_draw(UIButton self, MenuID current, AssetBank ab);


// :fUILabel

// :fMenuManager
MenuManager menu_manager_init(Rectangle screen_rec);
void menu_manager_update(MenuManager* self, Input input);
void menu_manager_draw(MenuManager self, AssetBank ab);
bool menu_manager_is_button_pressed(MenuManager self, UIButtonID ui_button_id);

#ifdef      MENU_MANAGER_IMPLEMENTATION
// menu_manager implementation

// :iUIButton
UIButton ui_button_init(MenuID active_on, Rectangle rec, const char* text, SpriteID sprite_id ) {
  return (UIButton){
    .active_on=active_on,
    .rec=rec,
    .text=sb_from_str_keep_null(text), // so that we wont have to allocate during draw.
    .sprite_id=sprite_id,
  };
}
UIButton ui_button_init_text(MenuID active_on, Rectangle rec, const char* text) {
  return ui_button_init(active_on, rec, text, 0);
}
void ui_button_free(UIButton* self) {
  sb_free(&self->text);
}
void ui_button_update(UIButton* self, MenuID current_menu, Input input) {
  if (self->active_on != current_menu) {
    self->hover            = false;
    self->state.is_pressed = false;
    return;
  }
  bool hover = check_collision_point_rec(input.pointer.position, self->rec);
  self->hover = hover;

  if ( hover ) {
    self->state.is_pressed = input.pointer.click.is_pressed;
    // I dont need others in this game
  } else {
    self->state.is_pressed = false;
  }
}
void ui_button_draw(UIButton self, MenuID current, AssetBank ab) {
  (void)ab;
  if (self.active_on != current) {
    return;
  }
  Color c = self.hover ? BLUE : BLACK;
  draw_rectangle_rounded(self.rec, 0.4f, 12, c);
  Vector2 pos = rul_rectangle_center_text_inside_offset(get_font_default(), self.rec, self.text.items, 30);
  draw_text(self.text.items, pos.x, pos.y, 30, WHITE);


}


// :iMenuManager
MenuManager menu_manager_init(Rectangle screen_rec) {
  MenuManager self = {0};

#define UI_BUTTON_INIT_UNWRAP(ID, ACTIVE_AT, X_START, X_END, Y_START, Y_END, THE_TEXT, THE_CODE) \
  self.buttons[ID] = ui_button_init_text(ACTIVE_AT, rul_rectangle_by_constraint(screen_rec, X_START, X_END, Y_START, Y_END), THE_TEXT);

  UI_BUTTONS(UI_BUTTON_INIT_UNWRAP)

  self.current = MENU_TITLE;
  return self;
}
void menu_manager_free(MenuManager* self) {
  for (int i = 0; i < UI_BUTTON_COUNT; ++i) {
    ui_button_free(&self->buttons[i]);
  }
}
void menu_manager_update(MenuManager* self, Input input) {
  for (int i = 0; i < UI_BUTTON_COUNT; ++i) {
    ui_button_update(&self->buttons[i], self->current, input);
  }
}
void menu_manager_draw(MenuManager self, AssetBank ab) {
  (void)(ab);
  (void)(self);
  for (int i = 0; i < UI_BUTTON_COUNT; ++i) {
    ui_button_draw(self.buttons[i], self.current, ab);
  }

}
bool menu_manager_is_button_pressed(MenuManager self, UIButtonID ui_button_id) {
  return self.buttons[ui_button_id].state.is_pressed;
}

#endif   // MENU_MANAGER_IMPLEMENTATION
#endif   //_MENU_MANAGER_H_

