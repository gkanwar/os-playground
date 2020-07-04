/**
 * Note: This will be a freestanding C code, i.e. with no access to the C
 * runtime. However, freestanding code has access to header-only defns.
 */

#include "debug_serial.h"
#include "multiboot.h"
#include "phys_mem_allocator.h"
#include "vga.h"

using namespace std;

// Put these checks in once to ensure linux or x64 targets crash out
#if defined(__linux__)
#error "Must use a cross-compiler!"
#endif
#if !defined(__i386__)
#error "Kernel only supports 32-bit ix86 targets!"
#endif


// TODO: Need to accept some kind of test-mode flag from multiboot
class Test {
 public:
  Test() {
    a = 'X';
  }
  char a;
};

Test test;

extern uint8_t physical_mem_bitmap[1 << 17];


extern "C" {
__attribute__((fastcall))
int kernel_early_main(const multiboot_info_t *info) {
  debug::init_serial();
  debug::serial_printf("begin kernel_early_main\n");
  unsigned long flags = info->flags;
  if (!(flags & MULTIBOOT_INFO_MEM_MAP)) {
    debug::serial_printf("multiboot mmap structures required to boot\n");
    return 1;
  }
  uint32_t mmap_length = info->mmap_length;
  uint32_t mmap_addr = info->mmap_addr;
  PhysMemAllocator::parse_mmap_to_bitmap(mmap_length, mmap_addr);
  debug::serial_printf("mmap structure loaded into phys mem bitmap\n");
  // check whether kernel is marked alloc'd
  // debug::serial_printf("kernel pages: %d\n", physical_mem_bitmap[1 << 5]);
  // debug::serial_printf("kernel pages: %d\n", physical_mem_bitmap[(1 << 5) + 1]);
  // debug::serial_printf("kernel pages: %d\n", physical_mem_bitmap[(1 << 5) + 2]);
  // debug::serial_printf("kernel pages: %d\n", physical_mem_bitmap[(1 << 5) + 3]);
  debug::serial_printf("phys mem bitmap:\n");
  for (int i = 0; i < (1 << 8); ++i) {
    debug::serial_printf("%08d ", ((uint32_t*)physical_mem_bitmap)[i]);
  }
  debug::serial_printf("end kernel_early_main\n");
  return 0;
}
  
void kernel_main(void) 
{
  terminal_initialize();
  terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  terminal_writestring("Welcome to the barebones kernel!\n");
  terminal_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_writestring("More features to come, tests for some specific ones run below.\n");
  terminal_writestring("Use alt-2 (or equivalent) to get to QEMU console and shutdown.\n");
  // check global ctors
  if (test.a == 'X') {
    terminal_writestring("[PASS global_ctors]");
  }
  else {
    terminal_writestring("[FAIL global_ctors]");
  }
}
}
