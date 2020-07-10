#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include <stddef.h>
#include "phys_mem_allocator.h"
#include "virt_mem_allocator.h"

#define CHUNK_PAGES 32
#define HEAP_PAGES 1024 // 4MiB heap

class HeapAllocator {
 public:
  HeapAllocator();
  static HeapAllocator& get();
  void init_heap_pages(PhysMemAllocator&, VirtMemAllocator&);
  void* malloc(size_t);
 private:
  void* slab8_malloc();
  void* slab16_malloc();
  void* slab32_malloc();
  void* block_malloc(size_t);
  union HeapBlock;
  void* phys_heap_chunks[HEAP_PAGES / 32];
  HeapBlock* heap;
  // Avoid really tiny mallocs hitting the main heap, using "slab"
  // suballocators for primitive small types. Each suballoc is given one chunk.
  struct Slab8 { uint8_t data[8]; };
  struct Slab16 { uint8_t data[16]; };
  struct Slab32 { uint8_t data[32]; };
  Slab8* slab8;
  Slab16* slab16;
  Slab32* slab32;
  uint8_t slab8_bitmap[PAGE_SIZE * CHUNK_PAGES / (sizeof(Slab8)*8)];
  uint8_t slab16_bitmap[PAGE_SIZE * CHUNK_PAGES / (sizeof(Slab16)*8)];
  uint8_t slab32_bitmap[PAGE_SIZE * CHUNK_PAGES / (sizeof(Slab32)*8)];
};

#endif
