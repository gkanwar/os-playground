/**
 * Note: This will be a freestanding C code, i.e. with no access to the C
 * runtime. However, freestanding code has access to header-only defns.
 */

#include "debug_serial.h"
#include "kernel.h"
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


// PMA is held globally because it exists before VM allocation exists.
PhysMemAllocator pma;

extern uint32_t init_page_dir[1024];
extern uint32_t init_page_tables[1024];

extern "C" {

// pre-paging helper to set up paging
// WARNING: should not call into any .text code, since that is not yet mapped.
// WARNING: .bss, etc memory references must be preprocessed with
//          KERNEL_VIRT_TO_PHYS since they are not yet mapped.
#define PD_FLAGS 0x1
#define PT_FLAGS 0x1
#define assert(x) if (!(x)) { asm volatile ("cli; hlt"::); }
__attribute__((section(".pretext")))
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t *p_page_table) {
  size_t page_pt_index = (virt_addr >> 12) & 0x3ff;
  uint32_t page_pt_entry = phys_addr | PT_FLAGS;
  p_page_table[page_pt_index] = page_pt_entry;
}

__attribute__((fastcall))
__attribute__((section(".pretext")))
void setup_paging(const multiboot_info_t* info) {
  uint32_t *p_init_page_dir = (uint32_t*)KERNEL_VIRT_TO_PHYS((uint32_t)init_page_dir);
  uint32_t *p_init_page_table = (uint32_t*)KERNEL_VIRT_TO_PHYS((uint32_t)init_page_tables);
  
  // map initial 4MiB page table block with both identity mapping and higher
  // half mapping, so we can keep executing but soon jump into higher half addrs
  uint32_t kernel_pd_entry = (uint32_t)p_init_page_table | PD_FLAGS;
  size_t ident_pd_index = KERNEL_VIRT_TO_PHYS((uint32_t)(&_kernel_start)) >> 22;
  size_t highhalf_pd_index = (uint32_t)(&_kernel_start) >> 22;
  p_init_page_dir[ident_pd_index] = kernel_pd_entry;
  p_init_page_dir[highhalf_pd_index] = kernel_pd_entry;

  // scan kernel pages and insert into common 4MiB page table block
  uint32_t kernel_page = KERNEL_VIRT_TO_PHYS((uint32_t)(&_kernel_start)) & PAGE_MASK;
  for (; kernel_page < KERNEL_VIRT_TO_PHYS((uint32_t)(&_kernel_end));
       kernel_page += PAGE_SIZE) {
    map_page(kernel_page, kernel_page, p_init_page_table);
  }

  // map BIOS buffers into higher half memory
  size_t vga_pd_index = VIRT_TERM_BUFFER >> 22;
  assert(vga_pd_index == highhalf_pd_index); // need to reuse PT
  map_page(VIRT_TERM_BUFFER, TERM_BUFFER, p_init_page_table);

  // map multiboot info and mmap info into higher half memory
  uint32_t multiboot_addr = (uint32_t)info;
  size_t multiboot_pd_index = (KERNEL_BASE + multiboot_addr) >> 22;
  assert(multiboot_pd_index == highhalf_pd_index); // need to reuse PT
  uint32_t multiboot_page = multiboot_addr & PAGE_MASK;
  for (; multiboot_page < multiboot_addr + sizeof(multiboot_info_t);
       multiboot_page += PAGE_SIZE) {
    map_page(KERNEL_BASE + multiboot_page, multiboot_page, p_init_page_table);
  }
  multiboot_pd_index = (KERNEL_BASE + info->mmap_addr) >> 22;
  assert(multiboot_pd_index == highhalf_pd_index); // need to reuse PT
  uint32_t mmap_page = info->mmap_addr & PAGE_MASK;
  for (; mmap_page < info->mmap_addr + info->mmap_length;
       mmap_page += PAGE_SIZE) {
    map_page(KERNEL_BASE + mmap_page, mmap_page, p_init_page_table);
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
  pma.parse_mmap_to_bitmap(mmap_length, mmap_addr);
  debug::serial_printf("phys mem bitmap:\n");
  for (int i = 0; i < (1 << 8); ++i) {
    debug::serial_printf("%08x ", ((uint32_t*)pma.mem_bitmap)[i]);
  }
  debug::serial_printf("...\n");
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
}
}
