#include "keyboard_state.h"
#include "shell.h"

namespace app {

void Shell::key_down(KeyCode code, uint8_t lock_state) {
  char c = key_map.to_ascii(code, lock_state);
  if (c) {
    term.putc(c);
  }
  else if (code == KeyCode::backspace) {
    term.retreat_char();
    term.putc(' ', false);
  }
  term.set_cursor_to_pos();
}
void Shell::key_up(KeyCode, uint8_t) {}

void Shell::main() {
  // FORNOW: just hlt for keyboard input
  while (true) { asm volatile ("hlt"::); }
}

}
