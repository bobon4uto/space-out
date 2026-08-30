#ifdef    MONO_BUILD
#define REF_IMPLEMENTATION
#undef MONO_BUILD
#endif // MONO_BUILD
#ifndef    _REF_H_
#define    _REF_H_
// ref interface

#include "no.util/core.h"

#define REF_INACTIVE SIZE_MAX
#define Ref(TYPE) TYPE##Ref
#define ref_inactive(TYPE) ( Ref(TYPE) ){.id=REF_INACTIVE,.index_hint=REF_INACTIVE}
#define MAKE_REF_FACE(TYPE) \
typedef struct s##TYPE##Ref { \
  u id; \
  u index_hint; \
} Ref(TYPE); \
u ref_find_id_##TYPE(u id, DA_(TYPE)* container); \
void ref_fix_##TYPE(Ref(TYPE)* ref, DA_(TYPE)* container); \
TYPE* ref_get_##TYPE(Ref(TYPE)* ref, DA_(TYPE)* container)

#define MAKE_REF_IMPL(TYPE) \
u ref_find_id_##TYPE(u id, DA_(TYPE)* container) { \
  for (u i = 0; i < container->count; ++i ) { \
    if (container->items[i].id == id) { \
      return i; \
    } \
  } \
  return REF_INACTIVE; \
} \
void ref_fix_##TYPE(Ref(TYPE)* ref, DA_(TYPE)* container) { \
  bool in_bounds           = ref->index_hint < container->count; \
  bool mismatched_id       = true; \
  if (in_bounds) { \
    mismatched_id = ref->id != container->items[ref->index_hint].id; \
  } \
  if ( mismatched_id ) { \
    ref->index_hint = ref_find_id_##TYPE(ref->id, container); \
    if (ref->index_hint == REF_INACTIVE) { \
      ref->id = REF_INACTIVE; \
    } \
  } \
} \
TYPE* ref_get_##TYPE(Ref(TYPE)* ref, DA_(TYPE)* container) { \
  ref_fix_##TYPE(ref, container); \
  if (ref->id != REF_INACTIVE) { \
    return &container->items[ref->index_hint]; \
  } \
  return NULL; \
}


#ifdef      REF_IMPLEMENTATION
// ref implementation



#endif   // REF_IMPLEMENTATION
#endif   //_REF_H_

