/**
 * Note: This will be a freestanding C code, i.e. with no access to the C
 * runtime. However, freestanding code has access to header-only defns.
 */

#include <cstring>

#include "debug_serial.h"
#include "heap_allocator.h"
#include "kernel.h"
#include "multiboot.h"
#include "phys_mem_allocator.h"
#include "test.h"
#include "vga.h"
#include "virt_mem_allocator.h"

using namespace std;

// Put these checks in once to ensure linux or x64 targets crash out
#if defined(__linux__)
#error "Must use a cross-compiler!"
#endif
#if !defined(__i386__)
#error "Kernel only supports 32-bit ix86 targets!"
#endif


// PMA, VMA, HA held globally because they exist before the heap exists
PhysMemAllocator physMemAlloc;
VirtMemAllocator virtMemAlloc;
HeapAllocator heapAlloc;

extern uint32_t init_page_dir[1024];
extern uint32_t init_page_tables[1024*1024];

extern "C" {

// pre-paging helper to set up paging
// WARNING: should not call into any .text code, since that is not yet mapped.
// WARNING: .bss, etc memory references must be preprocessed with
//          KERNEL_VIRT_TO_PHYS since they are not yet mapped.
#define assert(x) if (!(x)) { asm volatile ("cli; hlt"::); }
__attribute__((section(".pretext")))
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t *p_page_table) {
  size_t page_pt_index = VIRT_TO_PT_INDEX(virt_addr);
  uint32_t page_pt_entry = phys_addr | PT_FLAGS;
  p_page_table[page_pt_index] = page_pt_entry;
}

__attribute__((fastcall))
__attribute__((section(".pretext")))
void setup_paging(const multiboot_info_t* info) {
  uint32_t* p_init_page_dir = (uint32_t*)KERNEL_VIRT_TO_PHYS((uint32_t)init_page_dir);
  uint32_t* p_init_page_tables = (uint32_t*)KERNEL_VIRT_TO_PHYS((uint32_t)init_page_tables);
  
  // map initial 4MiB page table block with both identity mapping and higher
  // half mapping, so we can keep executing but soon jump into higher half addrs
  uint32_t kernel_pd_entry = (uint32_t)p_init_page_tables | PD_FLAGS;
  size_t ident_pd_index = VIRT_TO_PD_INDEX(KERNEL_VIRT_TO_PHYS(&_kernel_start));
  size_t highhalf_pd_index = VIRT_TO_PD_INDEX(&_kernel_start);
  p_init_page_dir[ident_pd_index] = kernel_pd_entry;
  p_init_page_dir[highhalf_pd_index] = kernel_pd_entry;

  // scan kernel pages and insert into common 4MiB page table blocks
  uint32_t kernel_page = KERNEL_VIRT_TO_PHYS(&_kernel_start) & PAGE_MASK;
  uint32_t* cur_p_page_table = p_init_page_tables;
  size_t cur_ident_pd_index = ident_pd_index;
  size_t cur_highhalf_pd_index = highhalf_pd_index;
  for (; kernel_page < KERNEL_VIRT_TO_PHYS(&_kernel_end); kernel_page += PAGE_SIZE) {
    size_t new_ident_pd_index = VIRT_TO_PD_INDEX(kernel_page);
    if (new_ident_pd_index > cur_ident_pd_index) {
      kernel_pd_entry += PAGE_SIZE;
      cur_p_page_table += PAGE_SIZE / (sizeof(uint32_t)/sizeof(uint8_t));
      cur_ident_pd_index = new_ident_pd_index;
      cur_highhalf_pd_index = highhalf_pd_index + cur_ident_pd_index - ident_pd_index;
      p_init_page_dir[cur_ident_pd_index] = kernel_pd_entry;
      p_init_page_dir[cur_highhalf_pd_index] = kernel_pd_entry;
    }
    map_page(kernel_page, kernel_page, cur_p_page_table);
  }

  // map BIOS buffers into higher half memory
  size_t vga_pd_index = VIRT_TO_PD_INDEX(VIRT_TERM_BUFFER);
  assert(vga_pd_index == highhalf_pd_index); // need to reuse PT
  map_page(VIRT_TERM_BUFFER, TERM_BUFFER, p_init_page_tables);

  // map multiboot info and mmap info into higher half memory
  uint32_t multiboot_addr = (uint32_t)info;
  size_t multiboot_pd_index = VIRT_TO_PD_INDEX(KERNEL_BASE + multiboot_addr);
  assert(multiboot_pd_index == highhalf_pd_index); // need to reuse PT
  uint32_t multiboot_page = multiboot_addr & PAGE_MASK;
  for (; multiboot_page < multiboot_addr + sizeof(multiboot_info_t);
       multiboot_page += PAGE_SIZE) {
    map_page(KERNEL_BASE + multiboot_page, multiboot_page, p_init_page_tables);
  }
  multiboot_pd_index = VIRT_TO_PD_INDEX(KERNEL_BASE + info->mmap_addr);
  assert(multiboot_pd_index == highhalf_pd_index); // need to reuse PT
  uint32_t mmap_page = info->mmap_addr & PAGE_MASK;
  for (; mmap_page < info->mmap_addr + info->mmap_length;
       mmap_page += PAGE_SIZE) {
    map_page(KERNEL_BASE + mmap_page, mmap_page, p_init_page_tables);
  }
}
#undef assert
  
__attribute__((fastcall))
int kernel_early_main(const multiboot_info_t *info) {
  debug::init_serial();
  debug::serial_printf("begin kernel_early_main\n");
  debug::serial_printf("init page dir:\n");
  for (int i = 0; i < 1024; ++i) {
    debug::serial_printf("%08x ", init_page_dir[i]);
  }
  debug::serial_printf("\n");
  uint32_t* page_table = (uint32_t*)(init_page_dir[0] & 0xfffffc00);
  debug::serial_printf("init page table:\n");
  for (int i = 0; i < 1024; ++i) {
    debug::serial_printf("%08x ", page_table[i]);
  }
  debug::serial_printf("\n");
  
  unsigned long flags = info->flags;
  if (!(flags & MULTIBOOT_INFO_MEM_MAP)) {
    debug::serial_printf("multiboot mmap structures required to boot\n");
    return 1;
  }
  uint32_t mmap_length = info->mmap_length;
  uint32_t mmap_addr = info->mmap_addr + KERNEL_BASE;
  physMemAlloc.init_mmap(mmap_length, mmap_addr);
  debug::serial_printf("phys mem bitmap:\n");
  for (int i = 0; i < (1 << 8); ++i) {
    debug::serial_printf("%08x ", ((uint32_t*)physMemAlloc.mem_bitmap)[i]);
  }
  debug::serial_printf("...\n");
  debug::serial_printf("init page dir...\n");
  virtMemAlloc.init_page_dir(init_page_dir, init_page_tables);
  debug::serial_printf("done\n");
  debug::serial_printf("init heap pages...\n");
  heapAlloc.init_heap_pages(physMemAlloc, virtMemAlloc);
  debug::serial_printf("done\n");
  debug::serial_printf("end kernel_early_main\n");
  return 0;
}
  
void kernel_main(void) 
{
  VirtMemAllocator::get().clear_ident_map();

  terminal_initialize();
  terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  terminal_writestring("Welcome to the " PROJ_NAME " kernel!\n");
  terminal_setcolor(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_writestring("More features to come, tests for some specific ones run below.\n");
  terminal_writestring("Use alt-2 (or equivalent) to get to QEMU console and shutdown.\n");
  
  test::run_test("malloc(128)", [&]()->bool {
    uint8_t* mem_chunk = (uint8_t*)HeapAllocator::get().malloc(128);
    if (!mem_chunk) return false;
    memset((void*)mem_chunk, 0xff, 128);
    uint8_t byte = mem_chunk[77];
    return byte == 0xff;
  });
  test::run_test("malloc(8)", []()->bool {
    uint8_t* mem_chunk = (uint8_t*)HeapAllocator::get().malloc(8);
    if (!mem_chunk) return false;
    memset((void*)mem_chunk, 0xab, 8);
    uint8_t byte = mem_chunk[5];
    return byte == 0xab;
  });
  test::run_test("malloc(16)", []()->bool {
    uint8_t* mem_chunk = (uint8_t*)HeapAllocator::get().malloc(16);
    if (!mem_chunk) return false;
    memset((void*)mem_chunk, 0xcd, 16);
    uint8_t byte = mem_chunk[13];
    return byte == 0xcd;
  });
  test::run_test("malloc(32)", []()->bool {
    uint8_t* mem_chunk = (uint8_t*)HeapAllocator::get().malloc(32);
    if (!mem_chunk) return false;
    memset((void*)mem_chunk, 0x88, 32);
    uint8_t byte = mem_chunk[27];
    return byte == 0x88;
  });
}
}
