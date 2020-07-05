#ifndef PHYS_MEM_ALLOCATOR_H
#define PHYS_MEM_ALLOCATOR_H

#include <stdint.h>
#include "kernel.h"

class PhysMemAllocator;
static PhysMemAllocator* pma_inst;

class PhysMemAllocator {
 public:
  PhysMemAllocator() { pma_inst = this; }
  static PhysMemAllocator& get() { return *pma_inst; }
  void parse_mmap_to_bitmap(uint32_t mmap_length, uint32_t mmap_addr);
  uint8_t mem_bitmap[NUM_PAGES / 8];
};

#endif
