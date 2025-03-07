#include "allocators.h"
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>


/// HELPERS /// 

void *call_mmap(void *addr, size_t len) {
  //@TODO: handle mmap errors
  return mmap(addr, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
};

size_t round_to_pagesize(size_t size) {
  size_t pagesize = sysconf(_SC_PAGESIZE);
  return (size + pagesize - 1) & ~(pagesize - 1);
}

/// ARENA ALLOCARTOR
// TODO: need to settle on/fix this api. i think these should return ALLOCATION_ERRORS 
// and pass in a pointer to a void_ptr
void ArenaAllocatorFree(ArenaAllocator *arena) {}

void *ArenaAllocatorAlloc(ArenaAllocator *arena, size_t size) {
  header *curr = arena->head;

  for (;;) {
    size_t block_capacity = curr->block_capacity;
    size_t block_len = curr->block_len;

    if (size <= block_capacity-block_len ) {
      void *blk_start = (char *)curr + curr->block_len;

      curr->block_len += size;
      arena->total_length += size;

      return blk_start;
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

    void* new_addr = (char*)curr + curr->block_capacity;

    void *data_ptr = call_mmap(new_addr, new_size);
    header *header_root = (header *)data_ptr;

    curr->next = header_root;
    header_root->block_capacity = new_size;
    header_root->block_len = min_size;

    data_ptr = (char*)data_ptr + sizeof(header);

    arena->total_length += new_size;
    return data_ptr;
  }
}

ArenaAllocator ArenaAllocatorInit() {
  ArenaAllocator arena_raw;

  size_t default_page_size = round_to_pagesize(1);

  void *data_ptr = call_mmap(NULL, default_page_size);
  header *header_root = (header *)data_ptr;

  arena_raw.vtable.free = ArenaAllocatorFree;
  arena_raw.vtable.alloc = ArenaAllocatorAlloc;

  arena_raw.head = header_root;

  header_root->next = NULL;
  header_root->block_len = sizeof(header);
  header_root->block_capacity = default_page_size;

  arena_raw.total_length = default_page_size;

  return arena_raw;
};

void ArenaAllocatorDeinit(ArenaAllocator *arena) {
  header *head = arena->head;

  while (head) {
    size_t len = head->block_capacity;
    header *temp_head = head->next;

    munmap(head, len);
    head = temp_head;
  }
    arena -> total_length = 0;
    arena -> head = NULL;
}

/// END ARENA_ALLOCATOR 
