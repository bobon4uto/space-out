#ifdef    MONO_BUILD
#define UNIT_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _UNIT_H_
#define    _UNIT_H_
// unit interface

#include "rl.h"

typedef struct sUnit {
  Vector2 position;
} Unit;

#ifdef      UNIT_IMPLEMENTATION
// unit implementation



#endif   // UNIT_IMPLEMENTATION
#endif   //_UNIT_H_

