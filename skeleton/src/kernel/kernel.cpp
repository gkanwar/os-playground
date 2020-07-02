/**
 * Note: This will be a freestanding C code, i.e. with no access to the C
 * runtime. However, freestanding code has access to header-only defns.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vga.h"

#if defined(__linux__)
#error "Must use a cross-compiler!"
#endif

#if !defined(__i386__)
#error "Kernel only supports 32-bit ix86 targets!"
#endif 

extern "C" {
void kernel_main(void) 
{
  terminal_initialize();
  terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
  terminal_writestring("Welcome to the barebones kernel!\n");
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_writestring("More features to come, we just have printing for now.\n");
  terminal_writestring("Use alt-2 (or equivalent) to get to QEMU console and shutdown.\n");
}
}
