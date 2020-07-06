#ifndef TEST_H
#define TEST_H

#include "vga.h"

namespace test {

template<typename Test>
void run_test(const char* name, Test t) {
  auto color = terminal_getcolor();
  if (t()) {
    terminal_setcolor(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
    terminal_writestring("[PASS ");
    terminal_writestring(name);
    terminal_writestring("]\n");
  }
  else {
    terminal_setcolor(VGA_COLOR_RED, VGA_COLOR_BLACK);
    terminal_writestring("[FAIL ");
    terminal_writestring(name);
    terminal_writestring("]\n");
  }
  terminal_setcolor(color);
}

}

#endif
