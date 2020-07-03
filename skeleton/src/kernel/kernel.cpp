/**
 * Note: This will be a freestanding C code, i.e. with no access to the C
 * runtime. However, freestanding code has access to header-only defns.
 */

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


extern "C" {
__attribute__((fastcall))
int kernel_early_main(const multiboot_info_t *info) {
  unsigned long flags = info->flags;
  if (!(flags & MULTIBOOT_INFO_MEM_MAP)) {
    return 1;
  }
  uint32_t mmap_length = info->mmap_length;
  uint32_t mmap_addr = info->mmap_addr;
  PhysMemAllocator::parse_mmap_to_bitmap(mmap_length, mmap_addr);
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
