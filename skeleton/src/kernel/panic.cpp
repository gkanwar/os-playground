#include "kernel.h"
#include "panic.h"
#include "vga.h"

void panic(const char* msg) {
  terminal_initialize();
  terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_MAGENTA);
  terminal_clear();
  terminal_writestring("\n\n\n\n\n\n\n\n");
  const char title[] = "== " PROJ_NAME " kernel panic ==";
  unsigned off = (VGA_WIDTH - sizeof(title) - 2)/2;
  for (unsigned i = 0; i < off; ++i) {
    terminal_writestring(" ");
  }
  terminal_writestring(title);
  terminal_setindent(4);
  terminal_writestring("\n\n");
  terminal_writestring("We're having too much Fun, something broke!\n\n");
  terminal_writestring("Error: ");
  terminal_writestring(msg);
  while (true) { asm volatile ("cli; hlt"::); }
}
