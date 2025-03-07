#include <stdio.h>
//awa

typedef enum {

  NO_ERROR,
  NULL_PTR_INIT,
  OUT_OF_MEMORY,
  RANDOM_ERROR1,
  RANDOM_ERROR2

} AllocatorError;

// 12 byte alignment??
typedef struct header {
    size_t block_len;
    size_t block_capacity;
    struct header *next;
} header;

typedef struct ArenaAllocator {
  struct {
    void *(*alloc)(struct ArenaAllocator*, size_t);
    void (*free)(struct ArenaAllocator* );
  } vtable;

  size_t total_length;
  struct header* head;

} ArenaAllocator;

ArenaAllocator ArenaAllocatorInit();
void ArenaAllocatorDeinit(ArenaAllocator *);
