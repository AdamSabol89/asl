#include <stdio.h>
// TBD: maybe we dont do this.
// ### API ### 
// ArenaAllocator arena; 
// AllocatorErrror err = ArenaAllocatorInit(&arena);
//
// void* my_ptr;
// err = arena.alloc(&arena, &my_ptr, 128);
//
// This is unfortunate but it is the "responsible" thing to do since an allocation can fail.
// ###########

typedef enum {

  NIL,
  NULL_PTR_INIT,
  OUT_OF_MEMORY,
  MMAP_ERROR,

} AllocatorError;

typedef struct header {
    size_t block_len;
    size_t block_capacity;
    struct header *next;
    struct header *prev;
} header;


typedef struct ArenaAllocator {
  struct {
    AllocatorError (*alloc)(struct ArenaAllocator*, void**, size_t);
    void (*free)(struct ArenaAllocator* );
  } vtable;

  size_t block_num;
  struct header* head;
  struct header* tail;

  /* do we keep a tail and check?
  struct header* tail;
  */

} ArenaAllocator;

AllocatorError ArenaAllocatorInit(ArenaAllocator *);
void ArenaAllocatorDeinit(ArenaAllocator *);
