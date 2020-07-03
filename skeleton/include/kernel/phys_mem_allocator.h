#ifndef PHYS_MEM_ALLOCATOR_H
#define PHYS_MEM_ALLOCATOR_H

#include <stdint.h>

extern uint8_t physical_mem_bitmap[1 << 17];

class PhysMemAllocator {
 public:
  static void parse_mmap_to_bitmap(uint32_t mmap_length, uint32_t mmap_addr);
};

#endif
