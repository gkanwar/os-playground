#include "phys_mem_allocator.h"

#include <cstring>
#include "multiboot.h"

#define PAGE_ADDR_NBITS 12
#define PAGE_ADDR_MASK 0xfff
#define PAGE_SIZE 0x1000

using namespace std;

static unsigned long page_addr_to_bit(uint64_t addr) {
  return (uint32_t) (addr >> PAGE_ADDR_NBITS);
}
static uint64_t round_addr_to_page(uint64_t addr) {
  addr &= ~(PAGE_ADDR_MASK);
  return addr;
}
static void mark_unavail_pages(const multiboot_memory_map_t& mmap, uint8_t *mem_bitmap) {
  // NOTE: we reclaim all ACPI memory at this boot stage
  if (mmap.type == MULTIBOOT_MEMORY_AVAILABLE ||
      mmap.type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
    return;
  }
  uint64_t addr = round_addr_to_page(mmap.addr);
  // TODO: could be more efficient by holding byte (or word) in tmp and making
  // all changes before writeback.
  for (; addr < mmap.addr + mmap.len; addr += PAGE_SIZE) {
    uint32_t i = page_addr_to_bit(addr);
    mem_bitmap[i/8] |= 1 << (i%8);
  }
}

void PhysMemAllocator::parse_mmap_to_bitmap(uint32_t mmap_length, uint32_t mmap_addr) {
  memset(physical_mem_bitmap, 0, sizeof(physical_mem_bitmap));
  multiboot_memory_map_t *mmap;
  for (mmap = (multiboot_memory_map_t*) mmap_addr;
       (uint32_t) mmap < mmap_addr + mmap_length;
       mmap = (multiboot_memory_map_t*) ((uint32_t) mmap + mmap->size + sizeof(mmap->size))) {
    mark_unavail_pages(*mmap, physical_mem_bitmap);
  }
}
