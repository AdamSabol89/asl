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

  ArenaAllocator arena;
  AllocatorError err = ArenaAllocatorInit(&arena);

  if (err != NIL){
    printf("Unable to allocate arena, erro_code %u \n", err);
  }

  printf("size of arena %zu \n", arena.head->block_len);
  arena.vtable.free(&arena);
}

void test_allocate_arena_alloc() {
  ArenaAllocator arena;
  AllocatorError err = ArenaAllocatorInit(&arena);

  if (err != NIL){
    printf("Unable to allocate arena, error_code %u \n", err);
  }

  size_t allocation_size = 100000;
  void* result;
  arena.vtable.alloc(&arena, &result, allocation_size);

  printf("size of arena %zu \n", arena.block_num);

  ASSERT(arena.block_num == 2);
  ArenaAllocatorDeinit(&arena);
}

void test_use_allocation() {
  ArenaAllocator arena; 
  AllocatorError err;

  err = ArenaAllocatorInit(&arena);
  if (err != NIL){
    printf("Unable to allocate arena, error_code %u \n", err);
  }

  size_t allocation_size = 8; 
  void* result_ptr;
  err = arena.vtable.alloc(&arena, &result_ptr, allocation_size);
  if (err != NIL){
    printf("Unable to allocate arena, error_code %u \n", err);
  }

  size_t* uresult_ptr = (size_t*) result_ptr;
  *uresult_ptr = 12;
  ASSERT(*uresult_ptr == 12);

  allocation_size = 100000;
  void* second_result_ptr;
  err = arena.vtable.alloc(&arena, &second_result_ptr, allocation_size);
  if (err != NIL){
    printf("Unable to allocate arena, error_code %u \n", err);
  }

  size_t* usecond_result_ptr = (size_t*) second_result_ptr;
  *usecond_result_ptr = 12;
  ASSERT(*usecond_result_ptr == 12);

  ArenaAllocatorDeinit(&arena);
}

int main() {

  init_page_size();
  test_init_arena_alloc();
  test_allocate_arena_alloc();
  test_use_allocation();

}
