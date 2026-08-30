#include "rl.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h> // Emscripten library
#endif

#define ASSET_BANK_IMPLEMENTATION
#define CAM_IMPLEMENTATION
#define GAME_IMPLEMENTATION
#define INPUT_MODULE_IMPLEMENTATION
#define MENU_MANAGER_IMPLEMENTATION
#define LEVEL_SELECT_IMPLEMENTATION
#define RAUTILS_IMPLEMENTATION
#define REF_IMPLEMENTATION
  #define NOU_STRING_BUILDER_IMPLEMENTATION
  #define NOU_STRING_VIEW_IMPLEMENTATION
#include "no.util.h"
#include "no.util/core.h"
#include "asset_bank.h"
#include "cam.h"
#include "game.h"
#include "input_module.h"
#include "level_select.h"
#include "menu_manager.h"
#include "rautils.h"
#include "ref.h"
#include "rl.h"

// :main
GameState gs = {0};
void all_update_draw();
int main(void) {
  set_trace_log_level(LOG_ALL);

  const int screen_x_ =1080;
  const int screen_y_ = 1080;
  init_window(screen_x_,screen_y_,"THE GAME");
  init_audio_device();
  gs = gs_init();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(all_update_draw, 0, 1);
#else
  set_target_f_p_s(60);
  while ( !window_should_close() )
  {
    all_update_draw();
  }
#endif


  gs_free(&gs);

  return 0;
}
void all_update_draw() {
  gs_update(&gs);
  gs_draw(gs);
}
