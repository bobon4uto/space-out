#ifdef    MONO_BUILD
#define LEVEL_SELECT_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _LEVEL_SELECT_H_
#define    _LEVEL_SELECT_H_
// level_select interface

#include "rl.h"
#include "rautils.h"
#include "no.util/core.h"
#include "ref.h"
#include "asset_bank.h"

#define SAT_PRICE 10.0f

typedef enum {
  LEVEL_TUTORIAL,
  LEVEL_1,
  LEVEL_2,
  LEVEL_3,
  LEVEL_4,
  LEVEL_5,
  LEVEL_CUSTOM,
  LEVEL_COUNT,
} LevelID;


typedef struct sLevelDef {
} LevelDef;

typedef struct sHazard Hazard;
DA_MAKE(Hazard);
MAKE_REF_FACE(Hazard);
struct sHazard {
  u id;
  Vector2 position;
  float radius;
  float transmission_power; // up to 1.0f
  bool connectable;
  bool player_placed;
  float energy;


  Ref(Hazard) wire;

  bool is_wire;
  Ref(Hazard) wired_to1;
  Ref(Hazard) wired_to2;
  bool is_start;
  bool is_finish;

  bool is_sat;

  StringBuilder name;

  SpriteID sprite;
};

typedef struct sLevel {
  u last_id;
  DA_(Hazard) hazards;
  SpriteID bg;
  float budget;
  StringBuilder title;
} Level;


typedef struct sLevelSelect {
  LevelID level;
  Level levels[LEVEL_COUNT];
  StringView sources[LEVEL_COUNT];

} LevelSelect;



LevelSelect level_select_init();
// :fLevel
Level level_init_from_sv(StringView level_source);
void level_draw(Level self, AssetBank ab);
Ref(Hazard) level_find_hazard(Level* self, Vector2 position, float radius, bool(*filter_fn)(Hazard));
void level_clear_broken_wires(Level* self);
float level_calc_price(Level* level);
void level_calc_energy(Level* level);
void level_save(Level* level);
void level_reload(Level* level, StringView source);
void level_free(Level* level);

// :fHazard
Ref(Hazard) hazard_push(Level* level, Hazard hazard);
Hazard hazard_new(Vector2 position, float radius, float transmission_power, SpriteID sprite);
void hazard_free(Hazard* self);
void hazard_draw(Hazard self, Level* level, AssetBank ab);
bool hazard_is_not_wire(Hazard self);
bool hazard_check_collision(Hazard self, Level* level, Vector2 position, float radius);
void hazard_wire_get_placed_in_middle(Hazard* self, Level* level);
float hazard_calc_price(Hazard self, Level* level);
float hazard_wire_price( float distance );


#ifdef      LEVEL_SELECT_IMPLEMENTATION
// level_select implementation



LevelSelect level_select_init() {
  LevelSelect self = {0};
  self.level = LEVEL_TUTORIAL;
  self.sources[LEVEL_TUTORIAL] = sv_from_str(
"LEVEL Reach_for_the_moon 50 Solar_System_Swag\n"
"start named Earth 100 500 200\n"
"hazard Junk 450 300 120\n"
"unnamed Broken_Sat 500 500 60\n"
"sat 230 730\n"
"end named Moon 800 100 80\n"
        );


  self.sources[LEVEL_1] = sv_from_str(
"LEVEL Black_Sun_Space 60 General_Start\n"
"start named Earth 453 765 30\n"
"hazard Junk 650 571\n"
"end named Black_Sun 112 93 200\n"
"hazard Junk 248 454 30\n"
"hazard Junk 300 532 30\n"
"hazard Rock 183 516 30\n"
"hazard Junk 215 598 30\n"
"hazard Junk 111 575 30\n"
"hazard Junk 104 654 30\n"
"hazard Rock 329 434 30\n"
"hazard Junk 394 478 30\n"
"hazard Junk 110 481 30\n"
"hazard Junk 169 428 30\n"
"hazard Junk 268 385 30\n"
"unnamed Mercury 694 467 30\n"
"unnamed Venus 573 61  30\n"
"hazard Sun 453 365 200\n"
"wire 14 15\n"

        );


  self.sources[LEVEL_2] = sv_from_str(
"LEVEL Lone_Twins 50 Stellar_Space\n"
"start named Asphodene 880 600 200\n"
"end named Eridani 90 250 200\n"
"\n"
        );


  self.sources[LEVEL_3] = sv_from_str(
"LEVEL Spicy_Space 50 Star_Space\n"
"hazard Ouroboros_Worm 500 500 200\n"
"start named Arrakis 500 500 50\n"
"sat 750 500 50\n"

"end named Ship 90 50 50\n"
"\n"
        );

  self.sources[LEVEL_4] = sv_from_str(
"LEVEL Ballin_Space 1 General_Start\n"
"start named Sadala 125 500 100\n"
"end named Super_Cat 800 500 100\n"
"unnamed Crab_Ctulhu 300 500 30\n"
"unnamed Crab_Ctulhu 262 436 30\n"
"unnamed Crab_Ctulhu 342 552 30\n"
"unnamed Crab_Ctulhu 394 477 30\n"
"unnamed Crab_Ctulhu 438 556 30\n"
"unnamed Crab_Ctulhu 472 472 30\n"
"unnamed Crab_Ctulhu 514 566 30\n"
"unnamed Crab_Ctulhu 550 486 30\n"
"unnamed Crab_Ctulhu 595 546 30\n"
"unnamed Crab_Ctulhu 626 472 30\n"
"unnamed Crab_Ctulhu 658 530 30\n"
"unnamed Crab_Ctulhu 254 552 30\n"
"unnamed Crab_Ctulhu 337 428 30\n"
"unnamed Crab_Ctulhu 419 419 30\n"
"unnamed Crab_Ctulhu 384 604 30\n"
"unnamed Crab_Ctulhu 312 612 30\n"
"unnamed Crab_Ctulhu 460 620 30\n"
"unnamed Crab_Ctulhu 564 609 30\n"
"unnamed Crab_Ctulhu 507 419 30\n"
"unnamed Crab_Ctulhu 589 414 30\n"
        );

  self.sources[LEVEL_5] = sv_from_str(
"LEVEL Idle_Space 50 Multiverse_Space\n"
"start named Ship 450 100 50\n"
"end named Divinity_Generator 484 802 100\n"
"hazard Asphodene 0 800 400\n"
"hazard Eridani 972 800 400\n"
"hazard Junk  565 677 30\n"
"hazard Rock 422 650 30\n"
"hazard Junk  515 574 30\n"
"hazard Rock 603 593 30\n"
"hazard Junk  298 564 30\n"
"hazard Anatomically_Inaccurate_Skeleton  295 700 30\n"
"hazard Junk  407 565 30\n"
"hazard Kega  588 475 30\n"
"hazard Mach1ne  672 515 30\n"
"hazard Junk  447 496 30\n"
"hazard Rock 264 504 30\n"
"hazard Junk  373 454 30\n"
"hazard Rock 504 404 30\n"
"hazard Junk  673 444 30\n"
"hazard Junk  744 460 30\n"
"hazard Rock 597 391 30\n"
"hazard Junk  324 378 30\n"
"hazard Rock 401 373 30\n"
"hazard Junk  265 429 30\n"
"hazard Rock 169 532 30\n"
"hazard Junk  222 367 30\n"
"hazard Junk  325 311 30\n"
"hazard Job_Application  431 292 30\n"
"hazard Junk  532 307 30\n"
"hazard Rock 691 320 30\n"
"hazard Junk  743 380 30\n"
"hazard Junk  820 423 30\n"
"hazard Junk  607 252 30\n"
"hazard Junk  506 234 30\n"
"hazard Junk  364 230 30\n"
"hazard Man4 271 261 30\n"
"hazard Junk  204 290 30\n"
"hazard Junk  148 345 30\n"
"hazard Junk  145 418 30\n"
"hazard Junk  219 609 30\n"
"hazard Junk  636 663 30\n"
"hazard Junk  709 628 30\n"
"hazard Angelus  449 742 30\n"
        );


  return self;
}
// :iLevel
Level level_init_test(StringView level_source) {
  Level self = {0};
  (void)level_source;

  hazard_push(&self, hazard_new( (Vector2){100.0f,0.0f}, 50.0f, 1.0f, 0));
  hazard_push(&self, hazard_new( (Vector2){0.0f,100.0f}, 10.0f, 1.0f, 0));

  hazard_push(&self, hazard_new( (Vector2){0.0f,0.0f}, 10.0f, 1.0f, 0));

  Hazard start  =  hazard_new( (Vector2){-200.0f,0.0f}, 50.0f, 1.0f, 0);
  start.is_start = true;
  start.name = sb_from_str("Eridani");
  Hazard finish =  hazard_new( (Vector2){200.0f,0.0f}, 50.0f, 1.0f, 0);
  finish.is_finish = true;
  finish.name = sb_from_str("Asphodene");

  hazard_push(&self, start);
  hazard_push(&self, finish);

  self.budget = 100.0f;
  return self;
}

void sb_replace_underscore_with_space(StringBuilder* sb) {
  for (u i = 0; i<sb->count; ++i) {
    if (sb->items[i]=='_') {
      sb->items[i]=' ';
    }
  }
}
void sb_replace_space_with_underscore(StringBuilder* sb) {
  for (u i = 0; i<sb->count; ++i) {
    if (sb->items[i]==' ') {
      sb->items[i]='_';
    }
  }
}



Level level_init_from_sv(StringView level_source) {
  Level self = {0};

  DA_(StringView) words = sv_splitl_no_delim(level_source, sv_chop_by_space_fn);

  bool player_placed = false;
  foreach (StringView as word in words) {
// 972 843
    if( sv_is_equal_str( word, "PLAYER_MOVES") ) {
      player_placed = true;
      continue;
    }
    if( sv_is_equal_str( word, "SCORE") ) {
      break;
    }
    if( sv_is_equal_str( word, "LEVEL") && end - current > 3 ) {
      StringView title  = *(current+1);
      StringView budget = *(current+2);
      StringView sprite = *(current+3);
      self.title =  sb_from_sv(title);
      sb_replace_underscore_with_space(&self.title);
      self.bg = ab_sprite_from_name(sprite);
      self.budget = sv_to_f(budget);
      current = current+3;
      continue;
    }
    if( sv_is_equal_str( word, "named")&& end - current > 4 ) {
      StringView start_end = *(current-1);
      StringView name = *(current+1);
      StringView pos_x = *(current+2);
      StringView pos_y = *(current+3);
      StringView radius = *(current+4);
      Hazard to_push ={0};
      to_push.player_placed = player_placed;
      to_push.name = sb_from_sv(name);
      sb_replace_underscore_with_space(&to_push.name);
      to_push.sprite = ab_sprite_from_name(name);
      to_push.position.x = sv_to_f(pos_x);
      to_push.position.y = sv_to_f(pos_y);
      to_push.radius = sv_to_f(radius);
      to_push.connectable = true;

      to_push.is_start  = sv_is_equal_str(start_end, "start");
      to_push.is_finish = sv_is_equal_str(start_end, "end");

      hazard_push(&self, to_push);
      current = current+4;
      continue;
    }
    if( sv_is_equal_str( word, "hazard") && end - current > 4 ) {
      StringView sprite  = *(current+1);
      StringView pos_x = *(current+2);
      StringView pos_y = *(current+3);
      StringView radius = *(current+4);
      Hazard to_push ={0};
      to_push.player_placed = player_placed;
      (void)sprite;
      to_push.sprite = ab_sprite_from_name(sprite);
      to_push.position.x = sv_to_f(pos_x);
      to_push.position.y = sv_to_f(pos_y);
      to_push.radius = sv_to_f(radius);
      to_push.connectable = false;

      hazard_push(&self, to_push);
      current = current+4;
      continue;
    }

    if( sv_is_equal_str( word, "unnamed") && end - current > 4) {
      StringView sprite  = *(current+1);
      StringView pos_x = *(current+2);
      StringView pos_y = *(current+3);
      StringView radius = *(current+4);
      Hazard to_push ={0};
      to_push.player_placed = player_placed;
      (void)sprite;
      to_push.sprite = ab_sprite_from_name(sprite);
      to_push.position.x = sv_to_f(pos_x);
      to_push.position.y = sv_to_f(pos_y);
      to_push.radius = sv_to_f(radius);
      to_push.connectable = true;

      hazard_push(&self, to_push);
      current = current+4;
      continue;
    }


    if( sv_is_equal_str( word, "sat") && end - current > 2 ) {
      StringView pos_x = *(current+1);
      StringView pos_y = *(current+2);
      Hazard to_push = hazard_new((Vector2){0.0f,0.0f}, 30.0f, 0.9f, SPRITE_Sat);
      to_push.player_placed = player_placed;
      to_push.is_sat = true;

      to_push.position.x = sv_to_f(pos_x);
      to_push.position.y = sv_to_f(pos_y);
      to_push.connectable = true;

      hazard_push(&self, to_push);
      current = current+2;
      continue;
    }



    if( sv_is_equal_str( word, "wire") && end - current > 2 ) {
      StringView target1 = *(current+1);
      StringView target2 = *(current+2);
      Hazard to_push = hazard_new((Vector2){0.0f,0.0f}, 10.0f, 0.9f, SPRITE_Wire);
      to_push.player_placed = player_placed;
      to_push.is_wire = true;

      u index1 = sv_to_u(target1);
      u index2 = sv_to_u(target2);

      to_push.wired_to1.id = index1;
      to_push.wired_to2.id = index2;

      to_push.wired_to1.index_hint = index1;
      to_push.wired_to2.index_hint = index2;


      to_push.connectable = false;

      hazard_wire_get_placed_in_middle(&to_push, &self);
      hazard_push(&self, to_push);
      current = current+2;
      continue;
    }




  }


  return self;
}
StringBuilder level_to_sb(Level level) {
  StringBuilder sb = {0};

  bool player_placed = false;
  float energy = 0.0f;
  sb_replace_space_with_underscore(&level.title);
  sb_push_fmt(&sb,"LEVEL %.*s %.0f %s\n", SB_FMT(level.title), level.budget, ab_name_from_sprite(level.bg) );

  sb_replace_underscore_with_space(&level.title);
  foreach (Hazard as hz in level.hazards) {
    if (hz.player_placed) {
      if (!player_placed) {
        sb_push_fmt(&sb,"\nPLAYER_MOVES\n");
        player_placed = true;
      }
    }
      if (hz.name.count>0) {
        if (hz.is_start) {
          sb_push_fmt(&sb,"start ");
        }
        if (hz.is_finish) {
          sb_push_fmt(&sb,"end ");
          energy = hz.energy;
        }
        sb_push_fmt(&sb,"named %.*s %g %g %g\n",  SB_FMT(hz.name), hz.position.x, hz.position.y, hz.radius);
        continue;
      }

      if (hz.is_sat) {
        sb_push_fmt(&sb,"sat %g %g\n", hz.position.x, hz.position.y);
        continue;
      }

      if (hz.is_wire) {
        sb_push_fmt(&sb,"wire %zu %zu\n", hz.wired_to1.index_hint, hz.wired_to2.index_hint);
        continue;
      }


      if (!hz.connectable && !hz.is_wire) {
        sb_push_fmt(&sb,"hazard %s %g %g %g\n",  ab_name_from_sprite(hz.sprite), hz.position.x, hz.position.y, hz.radius);
        continue;
      }

      if (hz.connectable && hz.name.count==0) {
        sb_push_fmt(&sb,"unnamed %s %g %g %g\n",  ab_name_from_sprite(hz.sprite), hz.position.x, hz.position.y, hz.radius);
        continue;
      }
  }


  float price = level_calc_price(&level);
  if (energy<0.5f || price > level.budget) {
    sb_push_fmt(&sb,"\nSCORE = LOST\n");
  } else {
    sb_push_fmt(&sb,"\nSCORE = WON\n");
  }
  sb_push_fmt(&sb,"Coverage: %g\n", energy*100.0f);
  sb_push_fmt(&sb,"Money left: %g\n", level.budget - price);


  return sb;
}
void level_save(Level* level) {
  (void)level;
  StringBuilder sb = level_to_sb(*level);
  set_clipboard_text(sb_to_str(&sb));
  save_file_text("saved_level.txt", sb_to_str(&sb));
//set_clipboard_text("hi");
  sb_free(&sb);
}
void level_reload(Level* level, StringView source) {
  level_free(level);
  *level = level_init_from_sv(source);
}
void level_free(Level* level) {
  foreach (Hazard in level->hazards) {
    hazard_free(&item);
  }
  da_free(&level->hazards);
  sb_free(&level->title);
}

void level_draw(Level self, AssetBank ab) {
  draw_texture(ab_get_sprite(ab, self.bg), 0,0, WHITE );
  foreach (Hazard as hazard in self.hazards) {
    hazard_draw(hazard, &self, ab);
  }
}
Ref(Hazard) level_find_hazard(Level* self, Vector2 position, float radius, bool(*filter_fn)(Hazard)) {
  for (u i = 0; i < self->hazards.count; ++i) {
    u i_rev = self->hazards.count - 1 - i; // reverse because player placed objects have priority, and they are always later.
    Hazard hazard = self->hazards.items[i_rev];
    if ( hazard_check_collision(hazard, self, position, radius) ) {
      if (filter_fn) {
        if (!filter_fn(hazard)) {
          continue;
        }
      }
      return (Ref(Hazard)){hazard.id, i_rev};
    }
  }

  return ref_inactive(Hazard);

}
void level_clear_broken_wires(Level* self) {
  for (u i = 0; i < self->hazards.count; ++i) {
    Hazard maybe_wire = self->hazards.items[i];
    if (maybe_wire.is_wire) {
      Hazard* h1 = ref_get_Hazard(&maybe_wire.wired_to1, &self->hazards);
      Hazard* h2 = ref_get_Hazard(&maybe_wire.wired_to2, &self->hazards);
      if (h1&&h2) {
        // all good
      } else {
        da_remove_at(&self->hazards, i);
        i--;
        // nuhuh
      }
    }
  }
}
float level_calc_price(Level* level) {
  float price_total = 0.0f;
  foreach (Hazard in level->hazards) {
    price_total += hazard_calc_price(item, level);
  }
  return price_total;
}
void level_calc_energy(Level* level) {

  // naive approach?

  for (u i = 0; i<level->hazards.count; ++i) {
    Hazard* h1 = &level->hazards.items[i];
    h1->energy = 0.0f;
    if (h1->is_start) {
      h1->energy = 1.0f;
    }
  }

  for (u repeat = 0; repeat < 10; ++repeat) {
    for (u i = 0; i<level->hazards.count; ++i) {
      Hazard* h1 = &level->hazards.items[i];
      if (h1->is_wire) {
        Hazard* h01 = ref_get_Hazard(&h1->wired_to1, &level->hazards);
        Hazard* h2 = ref_get_Hazard(&h1->wired_to2, &level->hazards);
        if (h01&&h2) {
          float energy = rul_max_float(h01->energy,h2->energy);
          h01->energy = energy;
          h2->energy = energy;
        }
      }
      if (h1->is_sat) {
        for (u j = 0; j<level->hazards.count; ++j) {
          Hazard* h2 = &level->hazards.items[j];
          if (h2->is_wire || !h2->connectable) {
            continue;
          }
         // if (h1->energy<0.5f && h2->energy<0.5f) {
         //   continue;
         // }
          if (h2==h1) {
            continue;
          }
          float* target = &h2->energy;
          float* giver = &h1->energy;
          if (h2->energy >= h1->energy) {
            target = &h1->energy;
            giver = &h2->energy;
          }
          float dist = vector2_distance(h1->position, h2->position) - h2->radius - h1->radius;



          float unit = 50.0f;
          if (dist <= unit) {
            *target = *giver;
            continue;
          } else if (dist < unit*2.0f) {
            float mul = 1.0f - (dist-unit)/(unit*4.0f);
            *target = rul_max_float(mul * *giver, *target);
          } else if (dist < unit*3.0f) {
            float mul = 1.0f - (dist-unit)/(unit*2.0f);
            *target = rul_max_float(mul * *giver, *target);
          } else {
            continue;
          }

         // for (u k = 0; k<level->hazards.count; ++k) {
         //   Hazard* h_disturb = &level->hazards.items[k];
         //   if (h_disturb==h1 || h_disturb==h2) {
         //     continue;
         //   }
         //   if ( check_collision_circle_line(h_disturb->position, h_disturb->radius, h1->position, h2->position) ) {
         //     kpd = kpd*h_disturb->transmission_power;
         //   }
         // }
         // float total_energy = kpd / rul_max_float(dist / 1000.0f, 1.0f);
         // if (*target < total_energy) {
         //   *target = total_energy;
         // }
        }
      }
    }
  }
}


// :iHazard
Ref(Hazard) hazard_push(Level* level, Hazard hazard) {
  hazard.id = level->last_id++;
  da_push(&level->hazards, hazard);
  return (Ref(Hazard)){hazard.id, level->hazards.count - 1};
}
Hazard hazard_new(Vector2 position, float radius, float transmission_power, SpriteID sprite) {
  Hazard self = {0};

  self.id = 0; // will be set when we push
  self.position = position;
  self.radius = radius;
  self.transmission_power = transmission_power;
  self.sprite = sprite;

  self.connectable   = true;
  self.player_placed = false;

  self.wire = ref_inactive(Hazard);

  self.is_wire = false;
  self.wired_to1 = ref_inactive(Hazard);
  self.wired_to2 = ref_inactive(Hazard);

  return self;
}
void hazard_free(Hazard* self) {
  sb_free(&self->name);
}
void hazard_draw(Hazard self, Level* level, AssetBank ab) {
  (void)ab;


  if (self.is_wire) {
    Hazard* h1 = ref_get_Hazard(&self.wired_to1, &level->hazards);
    Hazard* h2 = ref_get_Hazard(&self.wired_to2, &level->hazards);
    if (h1 && h2) {
      draw_line_ex(h1->position, h2->position, 10.0f, fade(GREEN,0.5));
    }
  }
  if (self.is_sat) {

    for (int i =0; i<18; ++i) {
      draw_ring_lines(self.position, self.radius+50.0f, self.radius+50.0f, i*20.0f+10.0f, i*20.0f+20.0f, 16, WHITE);
    }
  }

  //draw_circle_v(self.position, self.radius, GRAY);
  rul_texture_draw_in_circle( ab_get_sprite(ab, self.sprite), self.position, self.radius );
  if (self.name.count > 0) {
    const char* name = text_format("%.*s",SB_FMT(self.name));
    Vector2 name_pos = rul_circle_center_text_under( get_font_default(), self.position, self.radius, name, 30);
    draw_text( name, name_pos.x, name_pos.y, 30, BLUE);

  }

  if (!self.is_wire) {
    if (self.energy>0.5f) {
      const char* energy = text_format("%.0f%%",self.energy*100);
      Vector2 energy_pos = rul_circle_center_text_inside( get_font_default(), self.position, energy, 30);
      draw_text( energy, energy_pos.x, energy_pos.y, 30, BLUE);
    }
  }
}
bool hazard_is_not_wire(Hazard self) {
  return !self.is_wire;
}
bool hazard_check_collision(Hazard self, Level* level, Vector2 position, float radius)  {
  if (self.is_wire) {
    Hazard* h1 = ref_get_Hazard(&self.wired_to1, &level->hazards);
    Hazard* h2 = ref_get_Hazard(&self.wired_to2, &level->hazards);
    if (h1 && h2) {
      return check_collision_circle_line(position, radius, h1->position, h2->position);
    } else {
      return false;
      // wire is broken, it will get cleaned eventually
    }
  } else {
    // this is not a wire, jsut a circle then.
    return check_collision_circles(self.position, self.radius, position, radius);
  }
}
void hazard_wire_get_placed_in_middle(Hazard* self, Level* level) {
  if (self->is_wire) {
    // should be wire
    Hazard* h1 = ref_get_Hazard(&self->wired_to1, &level->hazards);
    Hazard* h2 = ref_get_Hazard(&self->wired_to2, &level->hazards);
    if (h1&&h2) {
      self->position = vector2_lerp(h1->position, h2->position, 0.5);
    }
  }
}
float hazard_calc_price(Hazard self, Level* level) {
  if (self.player_placed) {
    if (self.is_wire) {
      // you know the drill
      Hazard* h1 = ref_get_Hazard(&self.wired_to1, &level->hazards);
      Hazard* h2 = ref_get_Hazard(&self.wired_to2, &level->hazards);
      if (h1&&h2) {
        return hazard_wire_price( vector2_distance(h1->position, h2->position) - h1->radius - h2->radius );
      } else {
        return 0.0f; // broken wire
      }
    } else {
      return SAT_PRICE;
    }
  } else {
    return 0.0f; // non player object werent bought
  }
}
float hazard_wire_price( float distance ) {
  return (distance*distance)/2000.0f;
}

MAKE_REF_IMPL(Hazard)




#endif   // LEVEL_SELECT_IMPLEMENTATION
#endif   //_LEVEL_SELECT_H_

