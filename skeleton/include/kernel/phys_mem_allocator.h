#ifndef PHYS_MEM_ALLOCATOR_H
#define PHYS_MEM_ALLOCATOR_H

#include <stdint.h>

class PhysMemAllocator {
 public:
  static void parse_mmap_to_bitmap(uint32_t mmap_length, uint32_t mmap_addr);
};

#endif
