typedef struct sGameState GameState;

#ifndef HOOK_ONLY

#ifdef    MONO_BUILD
#define GAME_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _GAME_H_
#define    _GAME_H_
// game interface



#include "rl.h"
#include "cam.h"
#include "input_module.h"
#include "menu_manager.h"
#include "level_select.h"

// :macro
#define SAT_RADIUS 100

// :type
typedef enum {
  TOOL_WIRE,
  TOOL_SAT,
  TOOL_ERASE,
  TOOL_COUNT,
} ToolID;

struct sGameState {
  MenuManager menu;
  RenderTexture2D game_screen;
  Rectangle screen_rec;
  Rectangle game_screen_rec;
  Vector2 game_screen_pos;
  LevelSelect level_select;
  Camera2D cam;
  Input input;
  ToolID selected;
  bool cant_place;
  bool placing;
  AssetBank ab;
  Hazard base_sat;
  Hazard base_erase;
  Hazard base_wire;
  Ref(Hazard) current_wire;
  float calculated_price;
  Music music;
};


// :func
// :fGameState
GameState gs_init();
void gs_free  (GameState* self);
void gs_update(GameState* gs);
void gs_draw  (GameState gs);
Level* gs_get_current_level(GameState* self);
void gs_inspect_change(GameState* gs);



#ifdef      GAME_IMPLEMENTATION
// game implementation


// :glob
static const char* TUTORIAL_FOR_CUSTOM = 
"You can get custom levels from other people and play them\n"
"here. To create your own custom level\n"
"press 'share'(or space on keyboard) in one of the main levels,\n"
"it will put the level in your clipboard. Paste the level in a\n"
"text editor and edit it. You can open this level,\n"
"put sattelites in it and share to see exact coordinates.\n"
"(thats how I developed levels btw)\n"
"\n";

// :impl
// :iGameState
GameState gs_init() {
  GameState self = {0};

  const int screen_x_ =1080;
  const int screen_y_ = 1080;
  Rectangle screen_rec = (Rectangle){ 0.0f,0.0f, screen_x_, screen_y_};



  self.input = input_init_default();
  self.screen_rec= screen_rec;

  Rectangle game_screen_rec = rul_rectangle_by_constraint(screen_rec, 0.05, 0.95, 0.05, 0.83);
  self.game_screen = load_render_texture(game_screen_rec.width, game_screen_rec.height);
  self.game_screen_rec = game_screen_rec;
  self.cam = cam_init(game_screen_rec.width, game_screen_rec.height);
  self.game_screen_rec.height = -self.game_screen_rec.height; // weird magical OpenGL
  self.game_screen_pos = rul_rectangle_pos(self.game_screen_rec);
  self.game_screen_rec.x = 0.0f;
  self.game_screen_rec.y = 0.0f;
  self.menu = menu_manager_init(screen_rec);
  self.level_select = level_select_init();

  self.cant_place = false;
  self.selected = TOOL_SAT;
  self.base_sat = hazard_new((Vector2){0.0f,0.0f}, 30.0f, 0.9f, SPRITE_Sat);
  self.base_sat.player_placed = true;
  self.base_sat.is_sat = true;
  self.base_erase = hazard_new((Vector2){0.0f,0.0f}, 30.0f, 0.9f, SPRITE_Erase);
  self.base_wire = hazard_new((Vector2){0.0f,0.0f}, 10.0f, 0.9f, SPRITE_Wire);
  self.base_wire.player_placed = true;
  self.base_wire.is_wire=true;
  self.base_wire.connectable=false;
  self.base_wire.energy=0.0f;
  self.base_erase.player_placed = true;
  self.placing = false;
  self.current_wire = ref_inactive(Hazard);


  self.music = load_music_stream("resources/musics/lunarshopmix.ogg");
  play_music_stream(self.music);

  ab_load_sprites(&self.ab);

  self.calculated_price = 0.0f;

  return self;
}
void gs_free(GameState* self) {
  (void)self;
  close_audio_device();
  close_window();
}
void gs_update(GameState* gs) {
  (void)gs;
  update_music_stream(gs->music);
  input_update(&gs->input);
  menu_manager_update(&gs->menu, gs->input);

  gs->cam.target = vector2_add(gs->cam.target, gs->input.gamepad.l_joy_stick.position);
  //gs->cam.target = vector2_scale( vector2_add(gs->cam.target, gs->input.gamepad.r_joy_stick.position), 10.0f);

#define UI_BUTTON_CODE_UNWRAP(ID, ACTIVE_AT, X_START, X_END, Y_START, Y_END, THE_TEXT, THE_CODE) \
  if ( menu_manager_is_button_pressed(gs->menu, ID) ) THE_CODE

    UI_BUTTONS(UI_BUTTON_CODE_UNWRAP)




  if (gs->menu.current == MENU_GAMEPLAY) {
    Hazard* current_wire = ref_get_Hazard(&gs->current_wire,&gs_get_current_level(gs)->hazards);


    Vector2 pointer_game_screen_pos = vector2_subtract( get_screen_to_world2_d(gs->input.pointer.position, gs->cam), gs->game_screen_pos);
    if (current_wire) {
      current_wire->position = pointer_game_screen_pos;
    }

    if (gs->selected==TOOL_SAT) {
      Ref(Hazard) possible_collision = level_find_hazard(gs_get_current_level(gs),  pointer_game_screen_pos, gs->base_sat.radius, hazard_is_not_wire);
      if ( ref_get_Hazard(&possible_collision, &gs_get_current_level(gs)->hazards ) ) {
        gs->cant_place = true;
      } else {
        gs->cant_place = false;
        if (gs->placing) {
          gs->base_sat.position = pointer_game_screen_pos;
          hazard_push(gs_get_current_level(gs), gs->base_sat);
          gs_inspect_change(gs);
        }
      }
    }


    if (gs->selected==TOOL_WIRE) {
      Ref(Hazard) possible_collision = level_find_hazard(gs_get_current_level(gs),  pointer_game_screen_pos, gs->base_wire.radius, hazard_is_not_wire);
      Hazard* to_wire = ref_get_Hazard(&possible_collision, &gs_get_current_level(gs)->hazards );
      if ( to_wire ) {
        if (to_wire->connectable) {
          if (gs->placing) {
            if (current_wire) {
              // we already started connecting, finish the job
              current_wire->wired_to2 = possible_collision;
              hazard_wire_get_placed_in_middle(current_wire, gs_get_current_level(gs));
              gs->current_wire = ref_inactive(Hazard);
              current_wire = NULL;
              gs_inspect_change(gs);
            } else {
              // push new wire and connect one end
              gs->base_wire.wired_to1 = possible_collision;
              gs->base_wire.position = pointer_game_screen_pos;
              gs->current_wire = hazard_push(gs_get_current_level(gs), gs->base_wire);
              current_wire = ref_get_Hazard(&gs->current_wire,&gs_get_current_level(gs)->hazards);
              if (current_wire) {
                current_wire->wired_to2 = gs->current_wire; // wired to self to draw line while dragging
              }

              gs->base_wire.wired_to1 = ref_inactive(Hazard);
            }
          }

        } else {
          gs->cant_place = true;
        }
      } else {
        gs->cant_place = true;
        // nothing to wire
      }
    }




    if (gs->selected==TOOL_ERASE) {
      if (gs->input.pointer.click.is_down) {
        Ref(Hazard) possible_collision = level_find_hazard(gs_get_current_level(gs),  pointer_game_screen_pos, gs->base_erase.radius, NULL);
        Hazard* to_erase = ref_get_Hazard(&possible_collision, &gs_get_current_level(gs)->hazards );
        if ( to_erase ) {
          // erase them mehehehehe
          if (to_erase->player_placed) {
            da_remove_at(&gs_get_current_level(gs)->hazards, possible_collision.index_hint);
            gs_inspect_change(gs);
          }
        } else {
          // nothing to erase
        }
      }
    }
    level_clear_broken_wires(gs_get_current_level(gs));
  }

  gs->base_sat.position = gs->input.pointer.position;//sus
  gs->base_erase.position = gs->input.pointer.position;

  gs->placing = false;


  if (gs->input.gamepad.buttons[BTN_START].is_pressed) { level_save(gs_get_current_level(gs)); }

  if (gs->input.gamepad.buttons[BTN_R_FACE_L].is_pressed) {
    gs->selected=TOOL_WIRE;
  }
  if (gs->input.gamepad.buttons[BTN_R_FACE_D].is_pressed) {
    gs->selected=TOOL_SAT;
  }
  if (gs->input.gamepad.buttons[BTN_R_FACE_R].is_pressed) {
    gs->selected=TOOL_ERASE;
  }

  gs_inspect_change(gs);
}
void gs_inspect_change(GameState* gs) {
  gs->calculated_price = level_calc_price(gs_get_current_level(gs));

  level_calc_energy(gs_get_current_level(gs));
}
void gs_draw(GameState gs) {
  (void)gs;

  begin_texture_mode(gs.game_screen);
    begin_mode2_d(gs.cam);
      clear_background(WHITE);
  if (gs.menu.current == MENU_GAMEPLAY) {
      level_draw(gs.level_select.levels[gs.level_select.level], gs.ab);
  }
    end_mode2_d();
  end_texture_mode();

  begin_drawing();

  clear_background(GRAY);
  //input_draw(gs.input);
  menu_manager_draw(gs.menu, gs.ab);
  if (gs.menu.current == MENU_CREDITS) {
    draw_text(
        "Music\nLunar shop \n(Trevor Lentz, Cleyton Kauffman, Migfus & FrancisLeeMusic)\n "
        "Author: glitchart\n"
        "License: CC-BY 4.0\n"
        "\n\nImages drawn by me. Some of them are from my old games:\n"
        "Man3 (actually a snail) from 'Don't Knock Off'\n"
        "Kega from 'swiss_army_bag'\n"
        "Anatomically_Inaccurate_Skeleton from 'see_no_evil'\n"
        "Mach1ne from 'brok_n-wor_d'\n"
        "Job_Application from 'job application the game'\n"
        , 100,200,30, BLACK
        );

  }
  if (gs.menu.current == MENU_LEVEL_SELECT) {
    if (gs.menu.buttons[UI_BUTTON_LEVEL_SELECT_CUSTOM].hover) {
    draw_text("opening 'custom'\nwith clipboard conten\nthat is not text\nmay crash the game\nMUST ENABLE CLIPBOARD USAGE OR WILL CRASH",
         gs.menu.buttons[UI_BUTTON_LEVEL_SELECT_CUSTOM].rec.x, 
         gs.menu.buttons[UI_BUTTON_LEVEL_SELECT_CUSTOM].rec.y + gs.menu.buttons[UI_BUTTON_LEVEL_SELECT_CUSTOM].rec.height, 30, RED);
    }
  }

  if (gs.menu.current == MENU_GAMEPLAY) {
    {

      const char* txt = text_format("%.2f$", gs_get_current_level(&gs)->budget - gs.calculated_price);
      Vector2 txt_pos =
  rul_rectangle_center_text_inside_offset(
      get_font_default(),
      rul_rectangle_by_constraint(gs.screen_rec, 0.7f, 0.85f, 0.01f, 0.04f),
      txt, 30);
      draw_text(txt, txt_pos.x, txt_pos.y, 30, BLACK );
      //draw_text( text_format("%.0f %.0f", txt_pos.x, txt_pos.y), 500, 0, 30, BLACK );

    }

    {
      const char* txt = text_format("%.*s", SB_FMT( gs_get_current_level(&gs)->title ) );
      Vector2 txt_pos =
  rul_rectangle_center_text_inside_offset(
      get_font_default(),
      rul_rectangle_by_constraint(gs.screen_rec, 0.25f, 0.55f, 0.01f, 0.04f),
      txt, 30);
      draw_text(txt, txt_pos.x, txt_pos.y, 30, BLACK );
    }


    {
      Hazard finish = {0};
      foreach (Hazard in gs_get_current_level(&gs)->hazards) {
        if (item.is_finish) {
          finish = item;
          break;
        }
      }
      const char* txt = text_format("%.0f%%", finish.energy*100.0f);
      Vector2 txt_pos =
  rul_rectangle_center_text_inside_offset(
      get_font_default(),
      rul_rectangle_by_constraint(gs.screen_rec, 0.55f, 0.75f, 0.01f, 0.04f),
      txt, 30);
      draw_text(txt, txt_pos.x, txt_pos.y, 30, BLACK );
      //draw_text( text_format("%.0f %.0f", txt_pos.x, txt_pos.y), 500, 0, 30, BLACK );

    }


    //draw_texture_v(gs.game_screen.texture, gs.game_screen_pos, WHITE);
    draw_texture_rec(gs.game_screen.texture, gs.game_screen_rec, gs.game_screen_pos, WHITE);

    //draw_text( text_format("%f %f",gs.game_screen_rec.width, gs.game_screen_rec.height),gs.game_screen_rec.x, gs.game_screen_rec.y, 30, BLUE);

    Rectangle rec_wire = gs.menu.buttons[UI_BUTTON_GAMEPLAY_WIRE].rec;
    rul_texture_draw_centered_inside_rectangle(ab_get_sprite(gs.ab, SPRITE_Wire), rec_wire);
    Rectangle rec_sat= gs.menu.buttons[UI_BUTTON_GAMEPLAY_SAT].rec;
    rul_texture_draw_centered_inside_rectangle(ab_get_sprite(gs.ab, SPRITE_Sat), rec_sat);
    Rectangle rec_erase= gs.menu.buttons[UI_BUTTON_GAMEPLAY_ERASE].rec;
    rul_texture_draw_centered_inside_rectangle(ab_get_sprite(gs.ab, SPRITE_Erase), rec_erase);

    if (gs.selected==TOOL_WIRE) {
      Rectangle rec = gs.menu.buttons[UI_BUTTON_GAMEPLAY_WIRE].rec;
      draw_rectangle_rounded_lines_ex(rec, 0.4, 12, 3.0f, GREEN);
    }
    if (gs.selected==TOOL_SAT) {
      hazard_draw(gs.base_sat, gs_get_current_level(&gs) ,gs.ab);
      if (gs.cant_place) {
          draw_circle_v(gs.base_sat.position, gs.base_sat.radius, fade(RED,0.5f) );
      }
      Rectangle rec = gs.menu.buttons[UI_BUTTON_GAMEPLAY_SAT].rec;
      draw_rectangle_rounded_lines_ex(rec, 0.4, 12, 3.0f, GREEN);
    }
    if (gs.selected==TOOL_ERASE) {
      hazard_draw(gs.base_erase, gs_get_current_level(&gs),  gs.ab);
      Rectangle rec = gs.menu.buttons[UI_BUTTON_GAMEPLAY_ERASE].rec;
      draw_rectangle_rounded_lines_ex(rec, 0.4, 12, 3.0f, GREEN);
    }
    if ( gs.level_select.level == LEVEL_CUSTOM ) {
      if (gs_get_current_level(&gs)->hazards.count == 0) {
        draw_text(TUTORIAL_FOR_CUSTOM, gs.game_screen_pos.x,gs.game_screen_pos.y, 30,BLACK);
      }
    }
    if ( gs.level_select.level == LEVEL_TUTORIAL ) {
      if (gs_get_current_level(&gs)->hazards.count == 5) {
        draw_text(
            "To win you must connect the moon to wifi.\n"
            , gs.game_screen_pos.x,gs.game_screen_pos.y + 60.0f, 30,BLUE);
        draw_text(
            "% shows\n coverage on the moon.\nIt must be at least 50%.\n"
            , gs.game_screen_pos.x+600.0f,gs.game_screen_pos.y + 300.0f, 30,BLUE);
        draw_text(
            "Use wire(left tool) to connect 2 objects.\n"
            "Earth and a broken sattelite for example\n"
            "choose wire, tap on Earth, tap on satellite\n"
            "costs exponentially more the longer it gets.\n"
            , gs.game_screen_pos.x+280.0f,gs.game_screen_pos.y + 580.0f, 30,BLUE);
        draw_text(
            "use satellite for wireless connection.\nAlways costs 10$\n"
            , gs.game_screen_pos.x+300.0f,gs.game_screen_pos.y + 750.0f, 30,BLUE);
      } else if (gs_get_current_level(&gs)->hazards.count < 5+3) {
        draw_text(
            "You can share your result with others\n"
            "by pressing share button or SPACE bar.\n"
            "it will copy your moves and\n"
            " results in the clipboard.\n"
            , gs.game_screen_pos.x,gs.game_screen_pos.y + 60.0f, 30,BLUE);
        draw_text(
            "try to get cost as low as possible\n"
            "by spacing out the satellites\n"
            "while still keeping 50% coverage.\n"
            "(or try to get 100% coverage, but I\n"
            "don't know if it is possible!).\n"
            , gs.game_screen_pos.x+280.0f,gs.game_screen_pos.y + 580.0f, 30,BLUE);
        draw_text(
            "use erasing bucket(right tool) to\n remove wires and satelites.\n Refunds all the money.\n"
            , gs.game_screen_pos.x+300.0f,gs.game_screen_pos.y + 750.0f, 30,BLUE);
        draw_text(
            "keyboard buttons 1 2 3\n choose tools too\n"
            , gs.game_screen_pos.x+600.0f,gs.game_screen_pos.y + 300.0f, 30,BLUE);

      }
    }
  }




  end_drawing();
}
Level* gs_get_current_level(GameState* self) {
  return &self->level_select.levels[self->level_select.level];
}

#endif   // GAME_IMPLEMENTATION
#endif   //_GAME_H_

#endif //HOOK_ONLY
