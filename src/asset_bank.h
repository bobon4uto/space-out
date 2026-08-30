#ifdef    MONO_BUILD
#define ASSET_BANK_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _ASSET_BANK_H_
#define    _ASSET_BANK_H_
// asset_bank interface

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h> // Emscripten library
#endif
#include "no.util/core.h"
#include "rl.h"


#define SPRITES(T) \
T(Anatomically_Inaccurate_Skeleton, "resources/sprites/obj/anatomically_inaccurate_skeleton.png") \
T(Angelus,            "resources/sprites/obj/angelus.png")              \
T(Arrakis,            "resources/sprites/obj/arrakis.png")              \
T(Asphodene,          "resources/sprites/obj/asphodene.png")            \
T(Black_Sun,          "resources/sprites/obj/black_sun.png")            \
T(Broken_Sat,         "resources/sprites/obj/broken_sat.png")           \
T(Crab_Ctulhu,        "resources/sprites/obj/crab_ctulhu.png")          \
T(Divinity_Generator, "resources/sprites/obj/divinity_generator.png")   \
T(Earth,              "resources/sprites/obj/earth.png")                \
T(Erase,              "resources/sprites/obj/erase.png")                \
T(Eridani,            "resources/sprites/obj/eridani.png")              \
T(Ship,               "resources/sprites/obj/generic_ship.png")         \
T(Job_Application,    "resources/sprites/obj/job_application.png")      \
T(Junk,               "resources/sprites/obj/junk.png")                 \
T(Kega,               "resources/sprites/obj/kega.png")                 \
T(Mach1ne,            "resources/sprites/obj/mach1ne.png")              \
T(Man4,               "resources/sprites/obj/man4.png")                 \
T(Mercury,            "resources/sprites/obj/mercury.png")              \
T(Moon,               "resources/sprites/obj/moon.png")                 \
T(Ouroboros_Worm,     "resources/sprites/obj/ouroboros_worm.png")       \
T(Rock,               "resources/sprites/obj/rock.png")                 \
T(Sadala,             "resources/sprites/obj/sadala.png")               \
T(Sat,                "resources/sprites/obj/sat.png")                  \
T(Sun,                "resources/sprites/obj/sun.png")                  \
T(Super_Cat,          "resources/sprites/obj/super_cat.png")            \
T(Venus,              "resources/sprites/obj/venus.png")                \
T(Wire,               "resources/sprites/obj/wire.png")                 \
T(Zeppelin,           "resources/sprites/obj/zeppelin.png")             \
\
T(General_Start,      "resources/sprites/bg/general_start.png")         \
T(Multiverse_Space,   "resources/sprites/bg/multiverse_space.png")      \
T(Solar_System_Swag,  "resources/sprites/bg/solar_system_sun_swag.png") \
T(Star_Space,         "resources/sprites/bg/star_space.png")            \
T(Stellar_Space,      "resources/sprites/bg/stellar_space.png")


typedef enum {
#define UNROLL_SPRITE_ENUM(SPRITE, FILENAME) SPRITE_##SPRITE,
  SPRITES(UNROLL_SPRITE_ENUM)
  SPRITE_COUNT,
} SpriteID;


typedef struct sAssetBank {
  Texture sprites[SPRITE_COUNT];
} AssetBank;


const char* ab_name_from_sprite(SpriteID sprite);
SpriteID  ab_sprite_from_name(StringView name);
void ab_load_sprites(AssetBank* ab);
Texture ab_get_sprite(AssetBank ab, SpriteID id);

#ifdef      ASSET_BANK_IMPLEMENTATION
// asset_bank implementation
const char* ab_name_from_sprite(SpriteID sprite) {
#define UNROLL_NAME_FROM_SPRITE(SPRITE, FILENAME) if (sprite==SPRITE_##SPRITE) {return #SPRITE;}
  SPRITES(UNROLL_NAME_FROM_SPRITE)
    return "UNNKNOWN";
}
SpriteID  ab_sprite_from_name(StringView name) {
#define UNROLL_SPRITE_FROM_NAME(SPRITE, FILENAME) if ( sv_is_equal_str(name, #SPRITE)) {return SPRITE_##SPRITE;}
  SPRITES(UNROLL_SPRITE_FROM_NAME)

  return SPRITE_COUNT;
}

void ab_load_sprites(AssetBank* ab) {
  
  //Image load_
//bool export_image_as_code(, )

#define UNROLL_LOAD_SPRITE(SPRITE, FILENAME) \
  ab->sprites[SPRITE_##SPRITE] = load_texture(FILENAME); \

  SPRITES(UNROLL_LOAD_SPRITE)
}
Texture ab_get_sprite(AssetBank ab, SpriteID id) {
  return ab.sprites[id];
}


#endif   // ASSET_BANK_IMPLEMENTATION
#endif   //_ASSET_BANK_H_

