#include "allocators.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// awa

#define ASSERT(expr)                                                           \
  do {                                                                         \
    int result = (expr);                                                       \
    if (!result) {                                                             \
      fprintf(stderr, "Assertion failed: %s at %s:%d\n", #expr, __FILE__,      \
              __LINE__);                                                       \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

static size_t PAGE_SIZE;
void init_page_size() {
    PAGE_SIZE = sysconf(_SC_PAGESIZE);
}

void test_init_arena_alloc() {
  ArenaAllocator arena = ArenaAllocatorInit();

  printf("size of arena %zu \n", arena.head->block_len);
  arena.vtable.free(&arena);
}

void test_allocate_arena_alloc() {
  ArenaAllocator arena = ArenaAllocatorInit();

  size_t allocation_size = 100000;
  arena.vtable.alloc(&arena, allocation_size);

  printf("size of arena %zu \n", arena.total_length);

  ASSERT(arena.total_length % PAGE_SIZE == 0);
  ArenaAllocatorDeinit(&arena);
}

int main() {
  init_page_size();
  test_init_arena_alloc();
  test_allocate_arena_alloc();
}
