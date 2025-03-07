#include "allocators.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

/// HELPERS ///

AllocatorError call_mmap(void *addr, void **ptr, size_t len) {
  //@TODO: handle mmap errors

  void *mmap_result = mmap(addr, len, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (mmap_result == MAP_FAILED) {
    return MMAP_ERROR;
  }

  *ptr = mmap_result;
  return NIL;
};

size_t round_to_pagesize(size_t size) {
  size_t pagesize = sysconf(_SC_PAGESIZE);
  return (size + pagesize - 1) & ~(pagesize - 1);
}

/// ARENA ALLOCARTOR
// TODO: need to settle on/fix this api. i think these should return
// ALLOCATION_ERRORS and pass in a pointer to a void_ptr
void ArenaAllocatorFree(ArenaAllocator *arena) {}

AllocatorError ArenaAllocatorAlloc(ArenaAllocator *arena, void** data_ptr, size_t size) {
  header *curr = arena->head;

  for (;;) {
    size_t block_capacity = curr->block_capacity;
    size_t block_len = curr->block_len;

    if (size <= block_capacity - block_len) {
      void *blk_start = (char *)curr + curr->block_len;

      curr->block_len += size;

      *data_ptr = blk_start;
      return NIL;
    }

    if (curr->next) {
      curr = curr->next;
      continue;
    }

    // Max(block_len*2, size)
    size_t min_size = size + sizeof(header);
    size_t proposed_size = (min_size > curr->block_capacity * 2)
                               ? min_size
                               : curr->block_capacity * 2;

    size_t new_size = round_to_pagesize(proposed_size);

    void *new_addr = (char *)curr + curr->block_capacity;

    void* block_ptr;
    AllocatorError err = call_mmap(new_addr, &block_ptr, new_size);
    if (err != NIL){
      return err;
    }

    header *header_root = (header *)block_ptr;

    curr->next = header_root;
    header_root->block_capacity = new_size;
    header_root->block_len = min_size;

    block_ptr = (char *)block_ptr + sizeof(header);

    arena->ll_length += 1;
    *data_ptr = block_ptr;
    return NIL;
  }
}

AllocatorError ArenaAllocatorInit(ArenaAllocator* arena) {
  size_t default_page_size = round_to_pagesize(1);

  void *data_ptr;
  AllocatorError err = call_mmap(NULL, &data_ptr, default_page_size);

  if (err != NIL){
    return err;
  }

  header *header_root = (header *)data_ptr;

  arena->vtable.free = ArenaAllocatorFree;
  arena->vtable.alloc = ArenaAllocatorAlloc;

  arena->head = header_root;

  header_root->next = NULL;
  header_root->block_len = sizeof(header);
  header_root->block_capacity = default_page_size;

  arena->ll_length = 1;

  return NIL;
};

void ArenaAllocatorDeinit(ArenaAllocator *arena) {
  header *head = arena->head;

  while (head) {
    size_t len = head->block_capacity;
    header *temp_head = head->next;

    munmap(head, len);
    head = temp_head;
  }
  arena->ll_length = 0;
  arena->head = NULL;
}

/// END ARENA_ALLOCATOR
