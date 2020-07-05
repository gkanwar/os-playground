#include "phys_mem_allocator.h"

#include <cstring>
#include "kernel.h"
#include "multiboot.h"

using namespace std;

static unsigned page_addr_to_bit(uint64_t addr) {
  return (uint32_t) (addr >> PAGE_ADDR_NBITS);
}
#include "debug_serial.h"
static void mark_unavail_pages(const multiboot_memory_map_t& mmap, uint8_t *mem_bitmap) {
  // NOTE: we reclaim all ACPI memory at this boot stage
  if (mmap.type == MULTIBOOT_MEMORY_AVAILABLE ||
      mmap.type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
    return;
  }
  uint64_t addr = mmap.addr & PAGE_MASK;
  // TODO: could be more efficient by holding byte (or word) in tmp and making
  // all changes before writeback.
  for (; addr < mmap.addr + mmap.len; addr += PAGE_SIZE) {
    unsigned i = page_addr_to_bit(addr);
    mem_bitmap[i/8] |= 1 << (i%8);
  }
}

void PhysMemAllocator::parse_mmap_to_bitmap(uint32_t mmap_length, uint32_t mmap_addr) {
  memset(mem_bitmap, 0, sizeof(mem_bitmap));
  multiboot_memory_map_t *mmap;
  for (mmap = (multiboot_memory_map_t*) mmap_addr;
       (uint32_t) mmap < mmap_addr + mmap_length;
       mmap = (multiboot_memory_map_t*) ((uint32_t) mmap + mmap->size + sizeof(mmap->size))) {
    mark_unavail_pages(*mmap, mem_bitmap);
  }
  // mark the kernel code allocated too, otherwise BAD THINGS can happen
  multiboot_memory_map_t kernel_mmap;
  // TODO: better way to avoid sign extension issues?
  uint32_t kernel_start_addr = (uint32_t)&_kernel_start;
  uint32_t kernel_end_addr = (uint32_t)&_kernel_end;
  kernel_mmap.addr = KERNEL_VIRT_TO_PHYS(kernel_start_addr);
  kernel_mmap.len = kernel_end_addr - kernel_start_addr;
  kernel_mmap.type = MULTIBOOT_MEMORY_RESERVED;
  mark_unavail_pages(kernel_mmap, mem_bitmap);
}
